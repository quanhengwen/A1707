/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          canbus_middle.c
*\Description   
*\Note          
*\Log           2018.08.29    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#include "canbus_middle.h"
#include "protocol_anylise_frame.h"
#include "param.h"
#include "iap_if.h"
#include "flash_if.h"
#include "string.h"

/***************************************************************************************************
*\Function      canbus_middle_send
*\Description   CAN数据发送
*\Parameter     can
*\Parameter     ID
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note          
*\Log           2018.08.30    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
u32 canbus_middle_send(CANBus_type* can, u32 stdID, u8* buf, u32 len)
{
    CanTxMsg tx;
    u32 i = 0;
    u32 num = 0;
    u32 len_left = 0;

    if (!can || len == 0 || !buf)
    {
        return 0;
    }

    num = len / 8;
    len_left = len % 8;

    tx.StdId = stdID;
    tx.IDE = CAN_Id_Standard;
    tx.RTR = CAN_RTR_Data;

    //每次发送8字节
    for (i = 0; i < num; i++)
    {
        tx.DLC = 8;
        memcpy(tx.Data, buf + i * 8, 8);
        if (!stm32_CAN_send(can, &tx))
        {
            return i * 8;
        }
        if (i % 10 == 0)
        {
            rt_thread_delay(1);
        }
    }

    //发送多余的字节
    if (len_left)
    {
        tx.DLC = len_left;
        memcpy(tx.Data, buf + num * 8, len_left);
        if (!stm32_CAN_send(can, &tx))
        {
            return num * 8;
        }
    }

    return len;
}

/***************************************************************************************************
*\Function      canbus_middle_thread_entry
*\Description   
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.08.30    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void canbus_middle_thread_entry(void* p)
{
    CanRxMsg rx;
    u32 delay = 0;

	  can_ota_init();

    rt_thread_delay(200);
    stm32_CAN_clear(&CAN);

    while (1)
    {
        if (stm32_CAN_recv(&CAN, &rx))
        {
            delay++;
            if (delay % 10 == 0)
            {
                rt_thread_delay(1);
                delay = 0;
            }
            if (rx.IDE != CAN_Id_Standard || rx.RTR != CAN_RTR_Data)
            {
                continue;
            }
            switch (rx.StdId)
            {
            case 0x500:
                AddDataToBuf(&CAN_BUF500, rx.Data, rx.DLC);
//			{
//				u32  i = 0;
//				printf("LEN:%d\n", rx.DLC);
//				for(i = 0; i< rx.DLC;i++)
//				{
//					printf("%02X ", rx.Data[i]);
//				}
//				printf("\n");
//			}
                break;
			
            default:
                rt_kprintf("StdID[0x%03X] is no support!\n", rx.StdId);
                break;
            }
        }
        else
        {
            delay = 0;
            rt_thread_delay(1);
        } 
    }
}


/***************************************************************************************************
*\Function      CAN_Paramset_Receive_Analyse
*\Description   数据解析
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.03.08    Ver 1.0    baron
*               创建函数。
***************************************************************************************************/
void CAN_Paramset_Receive_Analyse(u8* buf)//需要根据自己的协议解析函数
{
	iap_param.ID=((buf[9] << 8) | buf[8]);//ID
	dev_param.mswork_mode=buf[10];// 主从模式
	realtime_param.can_debug_flag=buf[11];// 调试信息
	dev_param.analog_dial_switch=buf[12];// 模拟拨码开关
	dev_param.EXP1_EN=buf[26];
  dev_param.EXP2_EN=buf[27];
	dev_param.DC12EN1=buf[28];
	dev_param.DC12EN2=buf[29];
	dev_param.DC12EN3=buf[30];
	dev_param.DC12EN4=buf[31];
	dev_param.DC12EN5=buf[32];
	
	if(dev_param.EXP1_EN) EXP1_EN_H ; else EXP1_EN_L;
	if(dev_param.EXP2_EN) EXP2_EN_H ; else EXP2_EN_L;
	
	if(dev_param.DC12EN1) DC12EN1_H ; else DC12EN1_L;
	if(dev_param.DC12EN2) DC12EN2_H ; else DC12EN2_L;
	if(dev_param.DC12EN3) DC12EN3_H ; else DC12EN3_L;
	if(dev_param.DC12EN4) DC12EN4_H ; else DC12EN4_L;
	if(dev_param.DC12EN5) DC12EN5_H ; else DC12EN5_L;
	
}




//心跳包 
void CAN_Heartbeat_Package_Send(void)
{
	u32 data_len = 8;
	u8 buf[8];
	u32 i = 0;
	
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID
	buf[i++] = realtime_param.heartbeat_count;//序号
	buf[i++] = dev_param.version& 0xFF;//app版本低
	buf[i++] = 0;//保留
	buf[i++] = 0;//保留
	buf[i++] = (u8)realtime_param.current;       //电流低
	buf[i++] = (u8)(realtime_param.current >> 8);//电流高
	canbus_middle_send(&CAN, 0x600, buf, data_len);
}



/***************************************************************************************************
*\Function      CAN_AUTOMATIC_CHECK_ACK_Construct
*\Description   自动检测发送回复
*\Parameter     buf
*\Parameter     id_buf  目标id
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.03.08    Ver 1.0    baron
*               创建函数。
***************************************************************************************************/
u32 CAN_Automatic_Check_Construct(u8* buf, u32 len)//根据不同的设备进行组包
{
	u32 data_len = 24;
	u32 i = 0;

	if (len < data_len)
	{
		return 0;
	}

	buf[i++] = CAN_COMM_PROTOCOL_HEAD;
	buf[i++] = (u8)(data_len);
	buf[i++] = (u8)((data_len) >> 8);
	buf[i++] = FUC_DEVICE_SEARCH;
	buf[i++] = (u8)iap_param.remoteID;//remoteID
	buf[i++] = (u8)(iap_param.remoteID >> 8);//remoteID
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID

	buf[i++] = (u8)realtime_param.current_EX1;    
	buf[i++] = (u8)(realtime_param.current_EX1 >> 8);//电流高
	
	buf[i++] = (u8)realtime_param.current_EX2;    
	buf[i++] = (u8)(realtime_param.current_EX2 >> 8);//电流高
	
	buf[i++] = (u8)realtime_param.current;    
	buf[i++] = (realtime_param.current >> 8);//电流高
	buf[i++] =  1; //CAN通信 标志
	buf[i++] =  Get_Sum8(buf + 1, data_len - 3);
	buf[i++] =  CAN_COMM_PROTOCOL_TAIL;
	
	return i;
}


/***************************************************************************************************
*\Function      CAN_Paramset_Read_Construct
*\Description   设备参数的读取
*\Parameter     buf
*\Parameter     id_buf  目标id
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.03.07    Ver 1.0    baron
*               创建函数。
***************************************************************************************************/
u32 CAN_Paramset_Read_Construct(u8* buf , u32 len)//需要每人根据自己的项目组包
{
	u32 data_len = 32;
	u32 i = 0;

	if (len < data_len)
	{
		return 0;
	}

	buf[i++] = CAN_COMM_PROTOCOL_HEAD;
	buf[i++] = (u8)(data_len);
	buf[i++] = (u8)((data_len) >> 8);
	buf[i++] = FUC_PARAMSET_READ;
	buf[i++] = (u8)iap_param.remoteID;//remoteID
	buf[i++] = (u8)(iap_param.remoteID >> 8);//remoteID
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID
	buf[i++] = dev_param.mswork_mode;// 主从模式
	buf[i++] = realtime_param.can_debug_flag;// 调试信息
	buf[i++] = dev_param.analog_dial_switch;// 模拟拨码开关
	buf[i++] = (u8)realtime_param.voltage;// 电压低
	buf[i++] = (u8)(realtime_param.voltage >> 8);// 电压高
	buf[i++] = (u8)realtime_param.current;// 电流低
	buf[i++] = (u8)(realtime_param.current >> 8);// 电流高
	buf[i++] = (u8) Sys_Run_Time;// 运行时间0
	buf[i++] = (u8)(Sys_Run_Time >> 8);// 运行时间8
	buf[i++] = (u8)(Sys_Run_Time >> 16);// 运行时间16
	buf[i++] = (u8)(Sys_Run_Time >> 24);// 运行时间24
	buf[i++] = (u8)dev_param.version;//app版本低;
	buf[i++] = (u8)(dev_param.version >> 8);// app版本高
	buf[i++] = (u8)iap_param.swversion;// iap版本低
	buf[i++] = (u8)(iap_param.swversion >> 8);// iap版本高
	buf[i++] = iap_param.hwversion;// 硬件版本
	
	buf[i++] =dev_param.EXP1_EN;
  buf[i++] =dev_param.EXP2_EN;
	buf[i++] =dev_param.DC12EN1;
	buf[i++] =dev_param.DC12EN2;
	buf[i++] =dev_param.DC12EN3;
	buf[i++] =dev_param.DC12EN4;
	buf[i++] =dev_param.DC12EN5;
		
	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;
	
	return i;
	
}








