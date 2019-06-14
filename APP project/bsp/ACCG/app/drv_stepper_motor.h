/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_stepper_motor.h
*\Description   
*\Note          
*\Log           2019.04.17    Ver 1.0    ���
*               �����ļ���
***************************************************************************************************/
#ifndef _DRV_STEPPER_MOTOR_H_
#define _DRV_STEPPER_MOTOR_H_
#include "public_type.h"
#include <math.h>
#include "rtdef.h"
#include "drv_timer.h"

//�������߲�������ṹ��
typedef struct Stepper_motor_device_type_st
{
	GpioType* out1;/*�������OUT1��������*/
	GpioType* out2;/*�������OUT2��������*/
	GpioType* out3;/*�������OUT3��������*/
	GpioType* out4;/*�������OUT4��������*/
	GpioType* en_pin;/*�����������оƬ��������*/
	drv_timer_type* timer;
	uint16_t  speed;/*���Ʋ���������ٶ�*/
     //������Ҫ��ʼ��
	uint8_t forward_backward_flag;
	uint8_t pulse_state;/*����״̬*/
	__IO uint32_t  pulse_num;/*��¼���������*/
	rt_sem_t   sem_one_step_complete;/*���һ�����ڵ��ź���*/
    rt_mutex_t lock;//�豸��
}Stepper_motor_device_type;


uint8_t stepper_4_beat_forward_ctrl(Stepper_motor_device_type* step_dev, uint32_t total_num);
uint8_t stepper_4_beat_backforward_ctrl(Stepper_motor_device_type* step_dev, uint16_t total_num);
void stepper_stop(Stepper_motor_device_type* step_dev);

void stepper_motor_init(Stepper_motor_device_type* step_dev);

#endif
