/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          imu.c
*\Description   
*\Note          
*\Log           2019.03.25    Ver 1.0    ���
*               �����ļ���
***************************************************************************************************/

#include "imu.h"
#include "canbus_middle.h"
#include "param.h"
#include "mpu.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "drv_simu_i2c.h"
#include "app_board.h"
#include "drv_mpu9250.h"

attitude_data_type attitude_data;
attitude_data_type lasy_attitude_data;

/***************************************************************************************************
*\Function      IMU_thread_entry
*\Description   
*\Parameter     p
*\Return        void
*\Note          
*\Log           2019.04.08    Ver 1.0    ���               
				����������
***************************************************************************************************/
void IMU_thread_entry(void* p)
{
	float pitch,roll,yaw; 	        		//ŷ����
	short aacx,aacy,aacz;	        		//���ٶȴ�����ԭʼ����
	short gyrox,gyroy,gyroz;        		//������ԭʼ���� 
	short temp;		                		//�¶� 
	uint16_t cnt = 0,pitch_err_cnt = 0,roll_err_cnt = 0,yaw_err_cnt = 0;
	uint8_t pitch_buf[20],roll_buf[20],yaw_buf[20];
	uint8_t res;
	mems_original_data_type mems_data;
	
//	test1();
	
	cnt = 10;
	/*���������ȶ�һ��ʱ��*/
	while(cnt--)
	{
		while(mpu_mpl_get_data(&attitude_data.pitch,&attitude_data.roll,&attitude_data.yaw,&mems_data)==0)
		{
			;
		}
		rt_thread_delay(1);
	}
	
	lasy_attitude_data = attitude_data;
	
	while (1)
	{	
		while(mpu_mpl_get_data(&attitude_data.pitch,&attitude_data.roll,&attitude_data.yaw, &mems_data)==0)
		{
			/*�����⵽�����ǹ���*/
			if(abs(lasy_attitude_data.pitch - attitude_data.pitch) > (dev_param.pitch_threshold_value / 100))
			{
				pitch_err_cnt++;
				/*�������5�Σ��ͱ��������ǹ���*/
				if(pitch_err_cnt > 5)
				{
					report_data.attitude_status |= 0x01;
					lasy_attitude_data.pitch = attitude_data.pitch;
					rt_kprintf("pitch ����\r\n");
				}
			}
			else
			{
				lasy_attitude_data.pitch = attitude_data.pitch;
				pitch_err_cnt = 0;
			}
			
			/*�����⵽����ǹ���*/
			if(abs(lasy_attitude_data.roll - attitude_data.roll) > (dev_param.roll_threshold_value / 100))
			{
				roll_err_cnt++;
				/*�������5�Σ��ͱ�������ǹ���*/
				if(roll_err_cnt > 5)
				{
					report_data.attitude_status |= 0x02;
					lasy_attitude_data.roll = attitude_data.roll;
					rt_kprintf("roll ����\r\n");
				}
			}
			else
			{
				lasy_attitude_data.roll = attitude_data.roll;
				roll_err_cnt = 0;
			}
			
			/*�����⵽ƫ���ǹ���*/
			if(abs(lasy_attitude_data.yaw - attitude_data.yaw) > (dev_param.yaw_threshold_value / 100))
			{
				yaw_err_cnt++;
				/*�������5�Σ��ͱ���ƫ���ǹ���*/
				if(yaw_err_cnt > 5)
				{
					report_data.attitude_status |= 0x04;
					lasy_attitude_data.yaw = attitude_data.yaw;
					rt_kprintf("yaw ����\r\n");
				}
			}
			else
			{
				lasy_attitude_data.yaw = attitude_data.yaw;
				yaw_err_cnt = 0;
			}
			
			cnt++;
		}


		
		realtime_param.acc_x = (uint16_t)(mems_data.acc_data.x_axis_data * 0.061 * 100);
		realtime_param.acc_y = (uint16_t)(mems_data.acc_data.y_axis_data * 0.061 * 100);
		realtime_param.acc_z = (uint16_t)(mems_data.acc_data.z_axis_data * 0.061 * 100);
		
		realtime_param.pitch = (uint16_t)(attitude_data.pitch * 100);
		realtime_param.roll = (uint16_t)(attitude_data.roll * 100);
		realtime_param.yaw = (uint16_t)(attitude_data.yaw * 100);
		
		if(report_data.attitude_status)
		{
			/*�����⵽�쳣��������*/
			canbus_middle_send(&CAN, 0x500, (uint8_t *)&report_data, 8);
			rt_thread_delay(1);
			canbus_middle_send(&CAN, 0x500, (uint8_t *)&report_data, 8);
			report_data.attitude_status = 0;
		}		
		
		snprintf((char*)pitch_buf, 20, "%f", attitude_data.pitch);
		snprintf((char*)roll_buf, 20, "%f", attitude_data.roll);
		snprintf((char*)yaw_buf, 20, "%f", attitude_data.yaw);
		rt_kprintf("%s, %s, %s\n", pitch_buf, roll_buf, yaw_buf); 
		
		rt_thread_delay(5);
	}
}