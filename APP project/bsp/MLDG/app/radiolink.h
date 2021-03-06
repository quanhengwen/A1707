/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          radiolink.h
*\Description   
*\Note          
*\Log           2019.01.31    Ver 1.0    baron
*               �����ļ���
***************************************************************************************************/
#ifndef _RADIOLINK_H_
#define _RADIOLINK_H_
#include "public_type.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* User can use this section to tailor TIMx instance used and associated
   resources */
/* Definition for TIMx clock resources */
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
/* Exported functions ------------------------------------------------------- */

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
