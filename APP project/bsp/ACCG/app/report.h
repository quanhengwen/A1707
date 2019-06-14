/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          report.h
*\Description   
*\Note          
*\Log           2019.03.25    Ver 1.0    ���
*               �����ļ���
***************************************************************************************************/
#ifndef _REPORT_H_
#define _REPORT_H_
#include "public_type.h"
#include "drv_simu_i2c.h"
#include "app_board.h"
#include "drv_mpu9250.h"
#include "mpu.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 




typedef struct 
{
	uint16_t ID;
	uint8_t attitude_status;//��̬��״̬
	uint8_t folding_status;//�۵���״̬
	uint8_t combination_status;//��ϵ�״̬
	uint8_t stepper_status;//���������״̬
}__attribute__((packed)) report_data_type;




void report_thread_entry(void* p);

#endif
