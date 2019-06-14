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
#include "max9611.h"
#include "radiolink.h"
#include "usart.h"
#include "stdio.h"
#include "canbus_middle.h"
#include "param.h"
#include "exti.h"

extern BreatheMsgLedType MsgRed;
extern BreatheMsgLedType MsgBlue;

extern UartDevType UartTTL;

extern CANBus_type CAN;
extern ADC_DMA_Type ADC_InterT;

extern MAX9611_type MAX9611;

extern void app_board_init(void);

#endif /*_APP_BOARD_H*/

