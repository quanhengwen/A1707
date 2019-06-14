/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          radiolink.h
*\Description   
*\Note          
*\Log           2019.01.31    Ver 1.0    baron
*               创建文件。
***************************************************************************************************/
#ifndef _RADIOLINK_H_
#define _RADIOLINK_H_
#include "public_type.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* User can use this section to tailor TIMx instance used and associated
   resources */
#define frequence_default  67.6
#define TIM2_Clock 1000000
#define TIM3_Clock 1000000
#define TIM4_Clock 1000000
/* Definition for TIMx clock resources */
extern uint32_t CH2_DutyCycle_D,CH2_DutyCycle_R,CH3_DutyCycle_D,CH3_DutyCycle_R,CH4_DutyCycle,CH5_DutyCycle,CH6_DutyCycle,CH7_DutyCycle,CH8_DutyCycle,CH9_DutyCycle,CH10_DutyCycle;
extern u8 soft_switch;
//tim2
extern TIM_ICInitTypeDef  TIM2_Ch1_ICInitStructure;
extern TIM_ICInitTypeDef  TIM2_Ch2_ICInitStructure;
extern TIM_ICInitTypeDef  TIM2_Ch3_ICInitStructure;
extern TIM_ICInitTypeDef  TIM2_Ch4_ICInitStructure;
//tim3
extern TIM_ICInitTypeDef  TIM3_Ch1_ICInitStructure;
extern TIM_ICInitTypeDef  TIM3_Ch2_ICInitStructure;
extern TIM_ICInitTypeDef  TIM3_Ch3_ICInitStructure;
extern TIM_ICInitTypeDef  TIM3_Ch4_ICInitStructure;
//tim4
extern TIM_ICInitTypeDef  TIM4_Ch3_ICInitStructure;
extern TIM_ICInitTypeDef  TIM4_Ch4_ICInitStructure;

/* Private function prototypes -----------------------------------------------*/
void RCC_TimeConfiguration(void);
/* Private functions ---------------------------------------------------------*/
//tim2
void TIM2_Ch1_Init(void);
void TIM2_Ch2_Init(void);
void TIM2_Ch3_Init(void);
void TIM2_Ch4_Init(void);
void TIM2_NVIC_Configuration(void);
void TIM2_Configuration(void);
//tim3
void TIM3_Ch1_Init(void);
void TIM3_Ch2_Init(void);
void TIM3_Ch3_Init(void);
void TIM3_Ch4_Init(void);
void TIM3_NVIC_Configuration(void);
void TIM3_Configuration(void);
//tim4
void TIM4_Ch3_Init(void);
void TIM4_Ch4_Init(void);
void TIM4_NVIC_Configuration(void);
void TIM4_Configuration(void);
//TIM2/3/4_PWMHandleCallback for stm32f10x_it.c
void TIM2_PWMHandleCallback(void);
void TIM3_PWMHandleCallback(void);
void TIM4_PWMHandleCallback(void);

void radiolink_init(void);
void TIM2_Clock_Configuration(void);
void TIM3_Clock_Configuration(void);
void TIM4_Clock_Configuration(void);
void sendRLDuty_thread_entry(void* parameter);
/* Exported functions ------------------------------------------------------- */

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
