/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          exti.c
*\Description   通过中断有无判定通信的有无
*\Note          
*\Log           2019.04.17    Ver 1.0    baron
*               创建文件。
***************************************************************************************************/
#include "exti.h"
#include "app_board.h"


/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup EXTI_Config
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

Control_Set out_control = {0,0};

EXTI_InitTypeDef   EXTI_InitStructure;
GPIO_InitTypeDef   GPIO_InitStructure;
NVIC_InitTypeDef   NVIC_InitStructure;

/**
  * @brief  Configure PA.02 in interrupt mode
  * @param  None
  * @retval None
  */
void EXTI2_Config(void)
{
  /* Enable GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  /* Configure PA.02 pin as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Enable AFIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

  /* Connect EXTI0 Line to PA.02 pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);

  /* Configure EXTI2 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line2;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI2 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/***************************************************************************************************
*\Function      outcontrol_thread_entry
*\Description   
*\Parameter     parameter
*\Return        void
*\Note          
*\Log           2019.04.17    Ver 1.0    baron
*               创建函数。
***************************************************************************************************/
void outcontrol_thread_entry(void* parameter)
{
	
	while(1)
	{
	  out_control.count++;
		rt_thread_delay(8);
		if(out_control.count >= 5)
		  out_control.out_control_flag  = 1;
		else
			out_control.out_control_flag  = 0;
	}

}
/**
  * @}
  */ 
