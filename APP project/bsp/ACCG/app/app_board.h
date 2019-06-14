/***************************************************************************************************
*                    (c) Copyright 2008-2017  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          app_board.h
*\Description   
*\Note          
*\Log           2017.06.02    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#ifndef _APP_BOARD_H
#define _APP_BOARD_H
#include "common_lib.h"
#include "drv_switch.h"
#include "drv_breathe_led.h"
#include "drv_MixRTC.h"
#include "drv_com_uart.h"
#include "drv_simu_i2c.h"
#include "drv_adc.h"
#include "drv_can.h"
#include "drv_max9611.h"
#include "drv_stepper_motor.h"
#include "drv_mpu9250.h"
#include "app_current_detect.h"
#include "report.h"
#include "stepper_ctrl.h"
#include "hall_detect.h"
#include "iap_if.h"

extern BreatheMsgLedType MsgRed;
extern BreatheMsgLedType MsgBlue;

extern UartDevType UartTTL;

extern CANBus_type CAN;
extern ADC_DMA_Type ADC_InterT;

extern MAX9611_type MAX9611;

extern report_data_type report_data;

extern Stepper_motor_device_type stepper_motor1;
extern Stepper_motor_device_type stepper_motor2;

/*霍尔传感器*/
extern GpioType hall_1;
extern GpioType hall_2;
extern GpioType hall_3;
extern GpioType hall_4;

/*CAN发送的互斥量*/
extern rt_mutex_t mutex_can_send;

extern void app_board_init(void);

#endif /*_APP_BOARD_H*/

