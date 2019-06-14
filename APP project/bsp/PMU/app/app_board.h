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
#include "adc.h"
#include "stdio.h"

//#define USING_USART_PRINTF
typedef struct State_type
{
  u8 power_on;
	u8 charge_on;
	u8 work_mode;//充放电模式
	u8 switch_chn1;//四路开关状态判断
	u8 switch_chn2;
	u8 switch_chn3;
	u8 switch_chn4;
}State_type;

extern State_type state;//系统运行状态判断

extern BreatheMsgLedType MsgGreen;
extern BreatheMsgLedType MsgBlue;

extern UartDevType UartTTL;

extern CANBus_type CAN;
extern ADC_DMA_Type ADC_InterT;

extern MAX9611_type MAX9611;
extern MAX9611_type MAX9611_charge;//for fifth 9611

extern GpioType CHA_V_DET_Pin;//ADC123_IN2
extern GpioType PWRIN_DET_Pin;//ADC123_IN3

extern void app_board_init(void);

void akey_start_init(void);
#endif /*_APP_BOARD_H*/

