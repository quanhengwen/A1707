/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_max9611.h
*\Description   
*\Note          
*\Log           2019.01.28    Ver 1.0    baron
*               创建文件。
***************************************************************************************************/
#ifndef _DRV_MAX9611_H_
#define _DRV_MAX9611_H_
#include "public_type.h"
#include "drv_simu_i2c.h"

#define MAX9611_ADDR 0xE0//(0xE0<<1)

#define MAX9611_TEMP_DATA_MSB  0x80
#define MAX9611_TEMP_DATA_LSB  0x00

#define Gain1
//#define Gain4
//#define Gain8

#ifdef Gain1
#define Vref 440.0
#endif
#ifdef Gain4
#define Vref 110.0
#endif
#ifdef Gain8
#define Vref 55.0
#endif

//max结构体
typedef struct MAX9611_type_st
{
     stm32_simu_i2c_type* I2C;
     //以上需要初始化
     u8   SerialID[6];//Serial ID
     rt_mutex_t lock;//设备锁
}MAX9611_type;

typedef struct MAX9611_set_cfg
{
	u8 con_reg1,cfg1;
	u8 con_reg2,cfg2;
}MAX9611_set_cfg;

typedef struct
{
    u16 CSA;
	u16 RS;
	u16 OUT;
	u16 SET;
	float Temp;//温度 单位0.01℃
	float Current;//电流值 单位mA
}MAX9611_data_type;

extern MAX9611_data_type max9611_data;


void max9611_init(MAX9611_type* max);
bool max9611_data_read(MAX9611_type* max, MAX9611_data_type* data);
#endif
