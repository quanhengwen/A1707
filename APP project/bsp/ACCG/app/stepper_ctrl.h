/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          stepper_ctrl.h
*\Description   
*\Note          
*\Log           2019.04.17    Ver 1.0    杨贝贝
*               创建文件。
***************************************************************************************************/
#ifndef _STEPPER_CTRL_H_
#define _STEPPER_CTRL_H_
#include "public_type.h"
#include <math.h>
#include "rtdef.h"
#include "drv_timer.h"




void motor_stop(void);
void motor1_stop(void);
void motor2_stop(void);

void motor_ctrl(uint8_t direction , uint16_t distance);
void motor1_ctrl(uint8_t direction , uint16_t distance);
void motor2_ctrl(uint8_t direction , uint16_t distance);






void motor_ctrl_thread_entry(void* p);

#endif
