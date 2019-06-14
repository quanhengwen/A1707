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


//需要根据各个pcb重新编写的函数如下
//extern u32 CAN_Paramset_Read_Construct(u8* buf , u32 len);
//extern u32 CAN_Automatic_Check_Construct(u8* buf, u32 len);
//extern void CAN_Paramset_Receive_Analyse(u8* buf);

extern rt_uint32_t Sys_Run_Time;//系统运行时间

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
	rt_err_t rt_err;
    CanTxMsg tx;
    u32 i = 0;
    u32 num = 0;
    u32 len_left = 0;

    if (!can || len == 0 || !buf)
    {
        return 0;
    }

	
	rt_err = rt_mutex_take(mutex_can_send, RT_WAITING_FOREVER);
//	if(rt_err != RT_EOK)
//	{
//		return 0;
//	}
	
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
	
	rt_mutex_release(mutex_can_send);

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
	u32 data_len = 54;
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
	buf[i++] = (u8)dev_param.pitch_threshold_value;//俯仰角报警阈值低
	buf[i++] = (u8)(dev_param.pitch_threshold_value >> 8);//俯仰角报警阈值高
	buf[i++] = (u8)dev_param.roll_threshold_value;//横滚角报警阈值低
	buf[i++] = (u8)(dev_param.roll_threshold_value >> 8);//横滚角报警阈值高
	buf[i++] = (u8)dev_param.yaw_threshold_value;//偏航角报警阈值低
	buf[i++] = (u8)(dev_param.yaw_threshold_value >> 8);//偏航角报警阈值高
	buf[i++] = (u8)realtime_param.pitch;//俯仰角低
	buf[i++] = (u8)(realtime_param.pitch >> 8);//俯仰角高
	buf[i++] = (u8)realtime_param.roll;//横滚角低
	buf[i++] = (u8)(realtime_param.roll >> 8);//横滚角高
	buf[i++] = (u8)realtime_param.yaw;//偏航角低
	buf[i++] = (u8)(realtime_param.yaw >> 8);//偏航角高
	buf[i++] = (u8)realtime_param.acc_x;//加速度值X轴低
	buf[i++] = (u8)(realtime_param.acc_x >> 8);//加速度值X轴高
	buf[i++] = (u8)realtime_param.acc_y;//加速度值Y轴低
	buf[i++] = (u8)(realtime_param.acc_y >> 8);//加速度值Y轴高
	buf[i++] = (u8)realtime_param.acc_z;//加速度值Z轴低
	buf[i++] = (u8)(realtime_param.acc_z >> 8);//加速度值Z轴高

	buf[i++] = 0;//步进电机上锁开锁命令
	buf[i++] = (u8)dev_param.stepper_motor_distance;// 步进电机每次执行的固定距离低
	buf[i++] = (u8)(dev_param.stepper_motor_distance >> 8);// 步进电机每次执行的固定距离高
	buf[i++] = 0;//电机号
	buf[i++] = 0;//电机指令
	buf[i++] = 0;//距离低
	buf[i++] = 0;//距离高
	
	
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
	uint16_t  motor_distance;
	
	iap_param.ID=((buf[9] << 8) | buf[8]);//ID
	dev_param.mswork_mode=buf[10];// 主从模式
	realtime_param.can_debug_flag=buf[11];// 调试信息
	dev_param.analog_dial_switch=buf[12];// 模拟拨码开关
	
	/*存储姿态角报警阈值*/
	if(buf[26] != 0)
	{
		memcpy(&dev_param.pitch_threshold_value, &buf[26], 2);
		memcpy(&dev_param.roll_threshold_value, &buf[28], 2);
		memcpy(&dev_param.yaw_threshold_value, &buf[30], 2);		
	}

	
	/*存储步进电机行驶的距离*/
	if(buf[45] != 0)
	{
		memcpy(&dev_param.stepper_motor_distance, &buf[45], 2);
	}
	
	/*控制步进电机的命令，0x55是上锁，电机正转伸出插销固定距离，0xAA是解锁，电机反转缩回插销固定距离*/
	if(buf[43] == 0x55)
	{
		/*执行正转*/
		motor_ctrl(1, dev_param.stepper_motor_distance);
	}
	else if(buf[43] == 0xAA)
	{
		/*执行反转*/
		motor_ctrl(2, dev_param.stepper_motor_distance);
	}
	
	/*控制步进电机 可以单独控制，且需要控制距离。*/
	if(buf[47] != 0)
	{
		memcpy(&motor_distance, &buf[49], 2);
		
		if(buf[47] == 1)
		{
			if(buf[48] == 1)//正转
			{
				motor1_ctrl(1, motor_distance);
			}
			else if(buf[48] == 2)//反转
			{
				motor1_ctrl(2, motor_distance);
			}
			else if(buf[48] == 3)//停止
			{
				motor1_stop();
			}
		}
		else if(buf[47] == 2)
		{
			if(buf[48] == 1)//正转
			{
				motor2_ctrl(1, motor_distance);
			}
			else if(buf[48] == 2)//反转
			{
				motor2_ctrl(2, motor_distance);
			}
			else if(buf[48] == 3)//停止
			{
				motor2_stop();
			}			
		}
		else if(buf[47] == 3)
		{
			if(buf[48] == 1)//正转
			{
				motor_ctrl(1, motor_distance);
			}
			else if(buf[48] == 2)//反转
			{
				motor_ctrl(2, motor_distance);
			}
			else if(buf[48] == 3)//停止
			{
				motor_stop();
			}			
		}
	}	
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

	buf[i++] = 0x00;     //具体需检测参数单独添加
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;

	return i;
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
	buf[i++] = (u8)realtime_param.current;//电流低
	buf[i++] = (u8)(realtime_param.current >> 8);//电流高
	buf[i++] = (u8)realtime_param.voltage;//电压低
	buf[i++] = (u8)(realtime_param.voltage >> 8);//电压高	
	canbus_middle_send(&CAN, 0x600, buf, data_len);
}

