/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          report.c
*\Description   
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝
*               创建文件。
***************************************************************************************************/
#include "app_board.h"
#include "report.h"
#include "attitude_solution_quaternions.h"
#include "canbus_middle.h"
#include "iap_if.h"

extern iap_param_type iap_param;

report_data_type report_data;


/***************************************************************************************************
*\Function      IMU_thread_entry
*\Description   
*\Parameter     p
*\Return        void
*\Note          
*\Log           2019.04.08    Ver 1.0    杨贝贝               
				创建函数。
***************************************************************************************************/
void report_thread_entry(void* p)
{
	memset(&report_data, 0, sizeof(report_data));

	rt_thread_delay(100);
	
	while(1)
	{	
		report_data.ID = iap_param.ID;
		canbus_middle_send(&CAN, 0x500, (uint8_t *)&report_data, 8);
		rt_thread_delay(10);
	}
}
