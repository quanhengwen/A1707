/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          exti.h
*\Description   
*\Note          
*\Log           2019.04.17    Ver 1.0    baron
*               创建文件。
***************************************************************************************************/
#ifndef _GPIO_H_
#define _GPIO_H_
#include "public_type.h"

#define STATE_SW1_ON   GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define STATE_SW1_OFF  GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define STATE_SW2_ON   GPIO_SetBits(GPIOA,GPIO_Pin_6)
#define STATE_SW2_OFF  GPIO_ResetBits(GPIOA,GPIO_Pin_6)
#define PLED_EN_A_ON   GPIO_SetBits(GPIOC,GPIO_Pin_9)
#define PLED_EN_A_OFF  GPIO_ResetBits(GPIOC,GPIO_Pin_9)
#define PLED_EN_B_ON   GPIO_SetBits(GPIOA,GPIO_Pin_8)
#define PLED_EN_B_OFF  GPIO_ResetBits(GPIOA,GPIO_Pin_8)
#define SW_OFF         GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define SW_ON          GPIO_ResetBits(GPIOB,GPIO_Pin_12)

extern u8 LED_Twinkle_Mode;
extern u8 LED_LW_Mode;
extern u8 LED_change_flag;
/* Private function prototypes -----------------------------------------------*/
void led_gpio_init(void);
void LEDcontrol_thread_entry(void* parameter);

#endif
