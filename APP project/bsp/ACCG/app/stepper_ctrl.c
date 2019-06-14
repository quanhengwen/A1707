/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          stepper_ctrl.c
*\Description   两相四线步进电机驱动
*\Note          
*\Log           2019.04.17    Ver 1.0    杨贝贝
*               创建文件。
***************************************************************************************************/
#include "stepper_ctrl.h"
#include "drv_stepper_motor.h"
#include "app_board.h"


rt_mailbox_t mailbox_motor1;
rt_mailbox_t mailbox_motor2;


/***************************************************************************************************
*\Function      motor1_stop
*\Description   电机1停止
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    杨贝贝
*               创建函数。
***************************************************************************************************/
void motor1_stop(void)
{
	stepper_stop(&stepper_motor1);
	report_data.stepper_status &= 0xF0;
}

/***************************************************************************************************
*\Function      motor2_stop
*\Description   电机2停止
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    杨贝贝
*               创建函数。
***************************************************************************************************/
void motor2_stop(void)
{
	stepper_stop(&stepper_motor2);
	report_data.stepper_status &= 0x0F;
}

/***************************************************************************************************
*\Function      motor_stop
*\Description   电机停止
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    杨贝贝
*               创建函数。
***************************************************************************************************/
void motor_stop(void)
{
	stepper_stop(&stepper_motor1);
	stepper_stop(&stepper_motor2);
}

/***************************************************************************************************
*\Function      motor1_ctrl
*\Description   电机1控制
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    杨贝贝
*               创建函数。
***************************************************************************************************/
void motor1_ctrl(uint8_t direction , uint16_t distance)
{
	uint8_t send_data[4];
	/*方向，1是正转，2是反转，3是停车*/
	send_data[0] = 1;
	send_data[1] = direction;
	memcpy(send_data + 2, &distance, 2);
	rt_mb_send(mailbox_motor1, (rt_uint32_t)send_data);
}

/***************************************************************************************************
*\Function      motor1_ctr2
*\Description   电机2控制
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    杨贝贝
*               创建函数。
***************************************************************************************************/
void motor2_ctrl(uint8_t direction , uint16_t distance)
{
	uint8_t send_data[4];
	/*方向，1是正转，2是反转，3是停车*/
	send_data[0] = 2;
	send_data[1] = direction;
	memcpy(send_data + 2, &distance, 2);
	rt_mb_send(mailbox_motor2, (rt_uint32_t)send_data);
}

/***************************************************************************************************
*\Function      motor_ctrl
*\Description   电机控制
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    杨贝贝
*               创建函数。
***************************************************************************************************/
void motor_ctrl(uint8_t direction , uint16_t distance)
{
	motor1_ctrl(direction, distance);
	motor2_ctrl(direction, distance);
}

/***************************************************************************************************
*\Function      motor1_ctrl_thread_entry
*\Description   电机1控制控制线程
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    杨贝贝
*               创建函数。
***************************************************************************************************/
void motor1_ctrl_thread_entry(void* p)
{
	rt_err_t rt_err;
	uint8_t rev_data[4];
	uint16_t distance;

	/*执行停止命令和因某种原因没完成执行的操作，都会将状态设置为0未知状态*/
	report_data.stepper_status =0x22;
	
	while(1)
	{
		rt_err = rt_mb_recv (mailbox_motor1, (rt_uint32_t *)rev_data, RT_WAITING_FOREVER);
		if(rt_err == RT_EOK)
		{
			/*电机1*/
			if(rev_data[0] == 1)
			{
				if(rev_data[1] == 1)
				{
					/*正转*/
					memcpy(&distance, rev_data + 2, 2);
					/* distance 单位是mm,主轴转一圈，上升或者下降 0.7mm，实际测试 750个周期主轴转一圈*/
					if(stepper_4_beat_forward_ctrl(&stepper_motor1, (uint32_t)(distance / 0.7 * 750)) == 0)
					{
						/*返回0表示操作成功*/
						report_data.stepper_status &= 0xF0;
						report_data.stepper_status |= 0x01;
					}
					else
					{
						report_data.stepper_status &= 0xF0;
					}
				}
				else if(rev_data[1] == 2)
				{
					/*反转*/
					memcpy(&distance, rev_data + 2, 2);
					/* distance 单位是mm,主轴转一圈，上升或者下降 0.7mm，实际测试 750个周期主轴转一圈*/
					if(stepper_4_beat_backforward_ctrl(&stepper_motor1, (uint32_t)(distance / 0.7 * 750)) == 0)
					{
						/*返回0表示操作成功*/
						report_data.stepper_status &= 0xF0;
						report_data.stepper_status |= 0x02;
					}
					else
					{
						report_data.stepper_status &= 0xF0;
					}
				}
				else if(rev_data[1] == 3)
				{
					/*停止*/
					stepper_stop(&stepper_motor1);
					report_data.stepper_status &= 0xF0;
				}
			}
		}
	}
	
}

/***************************************************************************************************
*\Function      motor2_ctrl_thread_entry
*\Description   电机2控制控制线程
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    杨贝贝
*               创建函数。
***************************************************************************************************/
void motor2_ctrl_thread_entry(void* p)
{
	rt_err_t rt_err;
	uint8_t rev_data[4];
	uint16_t distance;

	
	while(1)
	{
		rt_err = rt_mb_recv (mailbox_motor2, (rt_uint32_t *)rev_data, RT_WAITING_FOREVER);
		if(rt_err == RT_EOK)
		{
			/*电机2*/
			if(rev_data[0] == 2)
			{
				if(rev_data[1] == 1)
				{
					/*正转*/
					memcpy(&distance, rev_data + 2, 2);
					/* distance 单位是mm,主轴转一圈，上升或者下降 0.7mm，实际测试 750个周期主轴转一圈*/
					if(stepper_4_beat_forward_ctrl(&stepper_motor2, (uint32_t)(distance / 0.7 * 750)) == 0)
					{
						/*返回0表示操作成功*/
						report_data.stepper_status &= 0x0F;
						report_data.stepper_status |= 0x10;
					}
					else
					{
						report_data.stepper_status &= 0x0F;
					}
				}
				else if(rev_data[1] == 2)
				{
					/*反转*/
					memcpy(&distance, rev_data + 2, 2);
					/* distance 单位是mm,主轴转一圈，上升或者下降 0.7mm，实际测试 750个周期主轴转一圈*/
					if(stepper_4_beat_backforward_ctrl(&stepper_motor2, (uint32_t)(distance / 0.7 * 750)) == 0)	
					{
						/*返回0表示操作成功*/
						report_data.stepper_status &= 0x0F;
						report_data.stepper_status |= 0x20;
					}
					else
					{
						report_data.stepper_status &= 0x0F;
					}					
				}
				else if(rev_data[1] == 3)
				{
					/*停止*/
					stepper_stop(&stepper_motor2);
					report_data.stepper_status &= 0x0F;
				}
			}
		}
	}
	
}

/***************************************************************************************************
*\Function      motor_ctrl_thread_entry
*\Description   电机控制控制线程，用来创建邮箱和两个线程
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    杨贝贝
*               创建函数。
***************************************************************************************************/
void motor_ctrl_thread_entry(void* p)
{
	rt_thread_t tid_motor1 = 0, tid_motor2 = 0;
	
	/************************************************************
	
	消息邮箱定义的说明：
	一个邮箱是4个字节，
	[0]：电机号     1表示电机1,2表示电机2
	[1]：控制指令   1表示正转，2表示反转，3表示停车
	[2~3]：步进距离 单位mm
	
	************************************************************/
	
	mailbox_motor1 = rt_mb_create ("mot1M", 4, RT_IPC_FLAG_FIFO); 
	if(mailbox_motor1 == RT_NULL)
	{
		rt_kprintf("creat motor1 mailebox failed\r\n");
	}
	
	mailbox_motor2 = rt_mb_create ("mot2M", 4, RT_IPC_FLAG_FIFO);
	if(mailbox_motor2 == RT_NULL)
	{
		rt_kprintf("creat motor2 mailebox failed\r\n");
	}
	
	
	tid_motor1 = rt_thread_create("mot1T", motor1_ctrl_thread_entry, RT_NULL, 1024, 4, 5);
    if (tid_motor1 != RT_NULL)
        rt_thread_startup(tid_motor1);
	
	tid_motor2 = rt_thread_create("mot2T", motor2_ctrl_thread_entry, RT_NULL, 1024, 4, 5);
    if (tid_motor2 != RT_NULL)
        rt_thread_startup(tid_motor2);
	
}

