/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_stepper_motor.h
*\Description   
*\Note          
*\Log           2019.04.17    Ver 1.0    杨贝贝
*               创建文件。
***************************************************************************************************/
#ifndef _DRV_STEPPER_MOTOR_H_
#define _DRV_STEPPER_MOTOR_H_
#include "public_type.h"
#include <math.h>
#include "rtdef.h"
#include "drv_timer.h"

//两相四线步进电机结构体
typedef struct Stepper_motor_device_type_st
{
	GpioType* out1;/*步进电机OUT1控制引脚*/
	GpioType* out2;/*步进电机OUT2控制引脚*/
	GpioType* out3;/*步进电机OUT3控制引脚*/
	GpioType* out4;/*步进电机OUT4控制引脚*/
	GpioType* en_pin;/*步进电机驱动芯片控制引脚*/
	drv_timer_type* timer;
	uint16_t  speed;/*控制步进电机的速度*/
     //以上需要初始化
	uint8_t forward_backward_flag;
	uint8_t pulse_state;/*脉冲状态*/
	__IO uint32_t  pulse_num;/*记录脉冲的总数*/
	rt_sem_t   sem_one_step_complete;/*完成一个周期的信号量*/
    rt_mutex_t lock;//设备锁
}Stepper_motor_device_type;


uint8_t stepper_4_beat_forward_ctrl(Stepper_motor_device_type* step_dev, uint32_t total_num);
uint8_t stepper_4_beat_backforward_ctrl(Stepper_motor_device_type* step_dev, uint16_t total_num);
void stepper_stop(Stepper_motor_device_type* step_dev);

void stepper_motor_init(Stepper_motor_device_type* step_dev);

#endif
