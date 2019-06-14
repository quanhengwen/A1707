/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          max9611.h
*\Description   
*\Note          
*\Log           2019.01.28    Ver 1.0    baron
*               创建文件。
***************************************************************************************************/
#ifndef _MAX9611_H_
#define _MAX9611_H_
#include "public_type.h"
#include "drv_simu_i2c.h"

#define MAX9611_Num  4
#define MAX9611_1_ADDR 0xE0//(0xE0<<1)
#define MAX9611_2_ADDR 0xE6
#define MAX9611_3_ADDR 0xF8
#define MAX9611_4_ADDR 0xFE
#define MAX9611_ADDR   0xE0//for fifth 9611

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

#define ON_36V01  GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define OFF_36V01  GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define ON_36V02  GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define OFF_36V02  GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define ON_36V03  GPIO_SetBits(GPIOA,GPIO_Pin_6)
#define OFF_36V03  GPIO_ResetBits(GPIOA,GPIO_Pin_6)
#define ON_36V04  GPIO_SetBits(GPIOA,GPIO_Pin_7)
#define OFF_36V04  GPIO_ResetBits(GPIOA,GPIO_Pin_7)


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
typedef struct MAX9611_data_type
{
  u16 CSA;
	u16 RS;
	u16 OUT;
	u16 SET;
	float Temp;//温度 单位0.01℃
	float Current;//电流值 单位mA
}MAX9611_data_type;

extern MAX9611_data_type max9611_data[4];
extern MAX9611_data_type max9611_data_charge;//for fifth 9611

void max9611_control_init(void);
void max9611_thread_entry(void* parameter);
void max9611_init(MAX9611_type* max);
bool max9611_data_read(MAX9611_type* max, MAX9611_data_type* data, u8 num);
void max9611_init_charge(MAX9611_type* max);//for fifth 9611
bool max9611_data_read_charge(MAX9611_type* max, MAX9611_data_type* data);//for fifth 9611
void max9611_charge_thread_entry(void* parameter);//for fifth 9611
#endif
