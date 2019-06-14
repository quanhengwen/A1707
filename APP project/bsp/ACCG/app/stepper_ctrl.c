/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          stepper_ctrl.c
*\Description   �������߲����������
*\Note          
*\Log           2019.04.17    Ver 1.0    ���
*               �����ļ���
***************************************************************************************************/
#include "stepper_ctrl.h"
#include "drv_stepper_motor.h"
#include "app_board.h"


rt_mailbox_t mailbox_motor1;
rt_mailbox_t mailbox_motor2;


/***************************************************************************************************
*\Function      motor1_stop
*\Description   ���1ֹͣ
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    ���
*               ����������
***************************************************************************************************/
void motor1_stop(void)
{
	stepper_stop(&stepper_motor1);
	report_data.stepper_status &= 0xF0;
}

/***************************************************************************************************
*\Function      motor2_stop
*\Description   ���2ֹͣ
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    ���
*               ����������
***************************************************************************************************/
void motor2_stop(void)
{
	stepper_stop(&stepper_motor2);
	report_data.stepper_status &= 0x0F;
}

/***************************************************************************************************
*\Function      motor_stop
*\Description   ���ֹͣ
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    ���
*               ����������
***************************************************************************************************/
void motor_stop(void)
{
	stepper_stop(&stepper_motor1);
	stepper_stop(&stepper_motor2);
}

/***************************************************************************************************
*\Function      motor1_ctrl
*\Description   ���1����
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    ���
*               ����������
***************************************************************************************************/
void motor1_ctrl(uint8_t direction , uint16_t distance)
{
	uint8_t send_data[4];
	/*����1����ת��2�Ƿ�ת��3��ͣ��*/
	send_data[0] = 1;
	send_data[1] = direction;
	memcpy(send_data + 2, &distance, 2);
	rt_mb_send(mailbox_motor1, (rt_uint32_t)send_data);
}

/***************************************************************************************************
*\Function      motor1_ctr2
*\Description   ���2����
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    ���
*               ����������
***************************************************************************************************/
void motor2_ctrl(uint8_t direction , uint16_t distance)
{
	uint8_t send_data[4];
	/*����1����ת��2�Ƿ�ת��3��ͣ��*/
	send_data[0] = 2;
	send_data[1] = direction;
	memcpy(send_data + 2, &distance, 2);
	rt_mb_send(mailbox_motor2, (rt_uint32_t)send_data);
}

/***************************************************************************************************
*\Function      motor_ctrl
*\Description   �������
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    ���
*               ����������
***************************************************************************************************/
void motor_ctrl(uint8_t direction , uint16_t distance)
{
	motor1_ctrl(direction, distance);
	motor2_ctrl(direction, distance);
}

/***************************************************************************************************
*\Function      motor1_ctrl_thread_entry
*\Description   ���1���ƿ����߳�
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    ���
*               ����������
***************************************************************************************************/
void motor1_ctrl_thread_entry(void* p)
{
	rt_err_t rt_err;
	uint8_t rev_data[4];
	uint16_t distance;

	/*ִ��ֹͣ�������ĳ��ԭ��û���ִ�еĲ��������Ὣ״̬����Ϊ0δ֪״̬*/
	report_data.stepper_status =0x22;
	
	while(1)
	{
		rt_err = rt_mb_recv (mailbox_motor1, (rt_uint32_t *)rev_data, RT_WAITING_FOREVER);
		if(rt_err == RT_EOK)
		{
			/*���1*/
			if(rev_data[0] == 1)
			{
				if(rev_data[1] == 1)
				{
					/*��ת*/
					memcpy(&distance, rev_data + 2, 2);
					/* distance ��λ��mm,����תһȦ�����������½� 0.7mm��ʵ�ʲ��� 750����������תһȦ*/
					if(stepper_4_beat_forward_ctrl(&stepper_motor1, (uint32_t)(distance / 0.7 * 750)) == 0)
					{
						/*����0��ʾ�����ɹ�*/
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
					/*��ת*/
					memcpy(&distance, rev_data + 2, 2);
					/* distance ��λ��mm,����תһȦ�����������½� 0.7mm��ʵ�ʲ��� 750����������תһȦ*/
					if(stepper_4_beat_backforward_ctrl(&stepper_motor1, (uint32_t)(distance / 0.7 * 750)) == 0)
					{
						/*����0��ʾ�����ɹ�*/
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
					/*ֹͣ*/
					stepper_stop(&stepper_motor1);
					report_data.stepper_status &= 0xF0;
				}
			}
		}
	}
	
}

/***************************************************************************************************
*\Function      motor2_ctrl_thread_entry
*\Description   ���2���ƿ����߳�
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    ���
*               ����������
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
			/*���2*/
			if(rev_data[0] == 2)
			{
				if(rev_data[1] == 1)
				{
					/*��ת*/
					memcpy(&distance, rev_data + 2, 2);
					/* distance ��λ��mm,����תһȦ�����������½� 0.7mm��ʵ�ʲ��� 750����������תһȦ*/
					if(stepper_4_beat_forward_ctrl(&stepper_motor2, (uint32_t)(distance / 0.7 * 750)) == 0)
					{
						/*����0��ʾ�����ɹ�*/
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
					/*��ת*/
					memcpy(&distance, rev_data + 2, 2);
					/* distance ��λ��mm,����תһȦ�����������½� 0.7mm��ʵ�ʲ��� 750����������תһȦ*/
					if(stepper_4_beat_backforward_ctrl(&stepper_motor2, (uint32_t)(distance / 0.7 * 750)) == 0)	
					{
						/*����0��ʾ�����ɹ�*/
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
					/*ֹͣ*/
					stepper_stop(&stepper_motor2);
					report_data.stepper_status &= 0x0F;
				}
			}
		}
	}
	
}

/***************************************************************************************************
*\Function      motor_ctrl_thread_entry
*\Description   ������ƿ����̣߳�������������������߳�
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    ���
*               ����������
***************************************************************************************************/
void motor_ctrl_thread_entry(void* p)
{
	rt_thread_t tid_motor1 = 0, tid_motor2 = 0;
	
	/************************************************************
	
	��Ϣ���䶨���˵����
	һ��������4���ֽڣ�
	[0]�������     1��ʾ���1,2��ʾ���2
	[1]������ָ��   1��ʾ��ת��2��ʾ��ת��3��ʾͣ��
	[2~3]���������� ��λmm
	
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

