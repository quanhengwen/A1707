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
#include "drv_watchdog.h"

#include "iap_if.h"
#include "flash_if.h"
#include "can_ota_protocol.h"
#include "param.h"
#include "adc.h"

extern BreatheMsgLedType MsgRed;
extern BreatheMsgLedType MsgBlue;

extern UartDevType UartTTL;

extern CANBus_type CAN;
extern ADC_DMA_Type ADC_InterT;



/*
	DC12EN1  PD2
	DC12EN2  PC12
	DC12EN3  PC11
	DC12EN4  PC10
	DC12EN5  PA15
	
	
	LED_SYS  PB13
	EXP1_EN  PB15
	EXP2_EN  PB14
*/

#define  DC12EN1_H  GPIO_SetBits(GPIOD, GPIO_Pin_2)
#define  DC12EN1_L  GPIO_ResetBits(GPIOD, GPIO_Pin_2)

#define  DC12EN2_H  GPIO_SetBits(GPIOC, GPIO_Pin_12)
#define  DC12EN2_L  GPIO_ResetBits(GPIOC, GPIO_Pin_12)

#define  DC12EN3_H  GPIO_SetBits(GPIOC, GPIO_Pin_11)
#define  DC12EN3_L  GPIO_ResetBits(GPIOC, GPIO_Pin_11)

#define  DC12EN4_H  GPIO_SetBits(GPIOC, GPIO_Pin_10)
#define  DC12EN4_L  GPIO_ResetBits(GPIOC, GPIO_Pin_10)

#define  DC12EN5_H  GPIO_SetBits(GPIOA, GPIO_Pin_15)
#define  DC12EN5_L  GPIO_ResetBits(GPIOA, GPIO_Pin_15)

#define  LED_SYS_H  GPIO_SetBits(GPIOB, GPIO_Pin_13)
#define  LED_SYS_L  GPIO_ResetBits(GPIOB, GPIO_Pin_13)

#define  EXP1_EN_H  GPIO_SetBits(GPIOB, GPIO_Pin_15)
#define  EXP1_EN_L  GPIO_ResetBits(GPIOB, GPIO_Pin_15)


#define  EXP2_EN_H  GPIO_SetBits(GPIOB, GPIO_Pin_14)
#define  EXP2_EN_L  GPIO_ResetBits(GPIOB, GPIO_Pin_14)
void control_Init();

void app_board_init(void);

#endif /*_APP_BOARD_H*/

