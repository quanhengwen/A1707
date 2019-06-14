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
#include "drv_watchdog.h"
#include "lidar_hall.h"


//需要根据各个pcb重新编写的函数如下
//extern u32 CAN_Paramset_Read_Construct(u8* buf , u32 len);
//extern u32 CAN_Automatic_Check_Construct(u8* buf, u32 len);
//extern void CAN_Paramset_Receive_Analyse(u8* buf);


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
//				rt_kprintf("LEN:%d\n", rx.DLC);
//				for(i = 0; i< rx.DLC;i++)
//				{
//					rt_kprintf("%02X ", rx.Data[i]);
//				}
//				rt_kprintf("\n");
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
*\Function      CAN_Heartbeat_Package_Send
*\Description   //默认向0x0101平台控制板发送心跳包
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.03.14    Ver 1.0    庞国瑞
*               创建函数。
***************************************************************************************************/
void CAN_Heartbeat_Package_Send(void)
{
	u32 data_len = 8;
	u8 buf[8];
	u32 i = 0;
	
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID
	buf[i++] = realtime_param.heartbeat_count;//序号
	buf[i++] = dev_param.version& 0xFF;//app版本低
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = (u8)realtime_param.current;//电流低
	buf[i++] = (u8)(realtime_param.current >> 8);//电流高
	canbus_middle_send(&CAN, 0x600, buf, data_len);
}
/***************************************************************************************************
*\Function      CAN_Paramset_Read_Construct
*\Description   设备参数的读取
*\Parameter     buf
*\Parameter     id_buf  目标id
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.04.22    Ver 1.0    mcc
*               创建函数。
***************************************************************************************************/
u32 CAN_Paramset_Read_Construct(u8* buf , u32 len)//需要每人根据自己的项目组包
{
	u32 data_len = 36;
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
	buf[i++] = (u8)Sys_Run_Time;// 运行时间0
	buf[i++] = (u8)(Sys_Run_Time >> 8);// 运行时间8
	buf[i++] = (u8)(Sys_Run_Time >> 16);// 运行时间16
	buf[i++] = (u8)(Sys_Run_Time >> 24);// 运行时间24
	buf[i++] = (u8)dev_param.version;//app版本低;
	buf[i++] = (u8)(dev_param.version >> 8);// app版本高
	buf[i++] = (u8)iap_param.swversion;// iap版本低
	buf[i++] = (u8)(iap_param.swversion >> 8);// iap版本高
	buf[i++] = iap_param.hwversion;// 硬件版本
	buf[i++] = (u8)OAG_data.lidar_broad_dist_th;//距离上报阈值低
	buf[i++] = (u8)(OAG_data.lidar_broad_dist_th>>8);//距离上报阈值高
	buf[i++] = OAG_data.lidar_config_enable;//配置参数状态
	buf[i++] = OAG_data.lidar_config_mode;//配置测距模式
	buf[i++] = OAG_data.lidar_config_mode_gear;//配置测距档位
	buf[i++] = (u8)OAG_data.lidar_config_period;//配置测距模组数据周期输出间隔
	buf[i++] = (u8)(OAG_data.lidar_config_period>>8);
	buf[i++] = 0x00;
	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;

	return i;
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
	u32 data_len = 15;
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
	buf[i++] = OAG_data.lidar_failure_state;//测距故障状态
	buf[i++] = OAG_data.hall_failure_state;//霍尔元件故障状态
	buf[i++] = (u8)realtime_param.current;// 电流低
	buf[i++] = (u8)(realtime_param.current >> 8);// 电流高
	buf[i++] = 0x01;//can通信正常
	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;

	return i;
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
	
	OAG_data.lidar_broad_dist_th = (buf[27]<<8) | buf[26];//距离上报阈值设置
	OAG_data.lidar_config_enable = buf[28];  //配置使能
	OAG_data.lidar_config_mode = buf[29]; //配置测距模式
	OAG_data.lidar_config_mode_gear = buf[30];   //测距档位设置
	OAG_data.lidar_config_period = (buf[32]<<8) | buf[31]; //设置数据输出周期
}
