/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          imu.h
*\Description   
*\Note          
*\Log           2019.03.25    Ver 1.0    ���
*               �����ļ���
***************************************************************************************************/
#ifndef _IMU_H_
#define _IMU_H_
#include "public_type.h"





//typedef union 
//{
//	struct 
//	{
//		float x;
//		float y;
//		float z;
//	};
//	float axis[3];
//} Axis3f;




//��̬���ݽṹ
typedef struct  
{
	float pitch;
	float roll;	
	float yaw;
}__attribute__((packed)) attitude_data_type;


typedef struct  
{
	uint16_t x_axis_data;
	uint16_t y_axis_data;
	uint16_t z_axis_data;
}__attribute__((packed)) axis_3_data_type;

//ԭʼ���ݽṹ
typedef struct  
{
	axis_3_data_type gyro_data;
	axis_3_data_type acc_data;
	axis_3_data_type mag_data;
}__attribute__((packed)) mems_original_data_type;



void IMU_thread_entry(void* p);

#endif
