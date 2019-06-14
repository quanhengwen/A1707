/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          app_current_detect.c
*\Description   
*\Note          
*\Log           2019.01.28    Ver 1.0    baron
*               创建文件。
***************************************************************************************************/
#include "app_current_detect.h"
#include "app_board.h"
#include "param.h"
#include "drv_max9611.h"



MAX9611_data_type max9611_data;

/***************************************************************************************************
*\Function      max9611_thread_entry
*\Description   
*\Parameter     parameter
*\Return        void
*\Note          
*\Log           2019.01.28    Ver 1.0    baron
*               创建函数。
***************************************************************************************************/
void max9611_thread_entry(void* parameter)
{
	uint16_t current;
	rt_thread_delay(100);//
	
	while(1)
	{
		max9611_data_read(&MAX9611, &max9611_data);
		current = (uint16_t)max9611_data.Current;
		memcpy(&realtime_param.current, &current, 2);
		rt_kprintf("current:%dmA\r\n", current);
//		rt_kprintf("%f℃, %fmA\r\n", max9611_data.Temp, max9611_data.Current);
		rt_thread_delay(RT_TICK_PER_SECOND);				
	}

}

