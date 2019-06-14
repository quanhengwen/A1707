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
#include "pwm_output.h"
#include "drv_watchdog.h"

#include "iap_if.h"
#include "flash_if.h"
#include "can_ota_protocol.h"
#include "param.h"
#include "max9611.h"

extern BreatheMsgLedType MsgRed;
extern BreatheMsgLedType MsgBlue;

extern UartDevType UartTTL;

extern CANBus_type CAN;
extern ADC_DMA_Type ADC_InterT;

extern void app_board_init(void);
extern u8 uart3SendStr(u8* buf, u32 len);
extern u8 uart3SendByte(u8 data);



typedef struct motor_infor_type_st
{
  u16 Motor_speed;
	u8  Motor_direction;
	u16 Motor_current;
	u8  Motor_error;
}motor_infor_type;

extern motor_infor_type motor_infor;

typedef struct motor485_type_st
{
	u8  recvbuf[20];
	u8  rec_i;
}motor485_type;

extern motor485_type motor485;

#endif /*_APP_BOARD_H*/

