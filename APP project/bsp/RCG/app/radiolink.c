/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          radiolink.c
*\Description   
*\Note          
*\Log           2019.01.31    Ver 1.0    baron
*               创建文件。
***************************************************************************************************/
#include "radiolink.h"
#include "stm32f10x.h"
#include "app_board.h"

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup TIM_Input_Capture
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//tim2
TIM_ICInitTypeDef  TIM2_Ch1_ICInitStructure;
TIM_ICInitTypeDef  TIM2_Ch2_ICInitStructure;
TIM_ICInitTypeDef  TIM2_Ch3_ICInitStructure;
TIM_ICInitTypeDef  TIM2_Ch4_ICInitStructure;
//tim3
TIM_ICInitTypeDef  TIM3_Ch1_ICInitStructure;
TIM_ICInitTypeDef  TIM3_Ch2_ICInitStructure;
TIM_ICInitTypeDef  TIM3_Ch3_ICInitStructure;
TIM_ICInitTypeDef  TIM3_Ch4_ICInitStructure;
//tim4
TIM_ICInitTypeDef  TIM4_Ch3_ICInitStructure;
TIM_ICInitTypeDef  TIM4_Ch4_ICInitStructure;

//tim2 ch1 
__IO uint16_t TIM2_Ch1_IC2ReadValue1 = 0, TIM2_Ch1_IC2ReadValue2 = 0;
__IO uint32_t TIM2_Ch1_Capture = 0;
__IO uint32_t TIM2_Ch1_Freq = 0;

uint32_t TIM2_Ch1_DutyCycle = 0;
float TIM2_Ch1_temp=0;
//tim2 ch2
__IO uint16_t TIM2_Ch2_IC2ReadValue1 = 0, TIM2_Ch2_IC2ReadValue2 = 0;
__IO uint32_t TIM2_Ch2_Capture = 0;
__IO uint32_t TIM2_Ch2_Freq = 0;

uint32_t TIM2_Ch2_DutyCycle = 0;
float TIM2_Ch2_temp=0;
//tim2 ch3 
__IO uint16_t TIM2_Ch3_IC2ReadValue1 = 0, TIM2_Ch3_IC2ReadValue2 = 0;
__IO uint32_t TIM2_Ch3_Capture = 0;
__IO uint32_t TIM2_Ch3_Freq = 0;

uint32_t TIM2_Ch3_DutyCycle = 0;
float TIM2_Ch3_temp=0;
//tim2 ch4 
__IO uint16_t TIM2_Ch4_IC2ReadValue1 = 0, TIM2_Ch4_IC2ReadValue2 = 0;
__IO uint32_t TIM2_Ch4_Capture = 0;
__IO uint32_t TIM2_Ch4_Freq = 0;

uint32_t TIM2_Ch4_DutyCycle = 0;
float TIM2_Ch4_temp=0;
//tim3 ch1 
__IO uint16_t TIM3_Ch1_IC3ReadValue1 = 0, TIM3_Ch1_IC3ReadValue2 = 0;
__IO uint32_t TIM3_Ch1_Capture = 0;
__IO uint32_t TIM3_Ch1_Freq = 0;

uint32_t TIM3_Ch1_DutyCycle = 0;
float TIM3_Ch1_temp=0;
//tim3 ch2
__IO uint16_t TIM3_Ch2_IC3ReadValue1 = 0, TIM3_Ch2_IC3ReadValue2 = 0;
__IO uint16_t CaptureNumber = 0;
__IO uint32_t TIM3_Ch2_Capture = 0;
__IO uint32_t TIM3_Ch2_Freq = 0;

uint32_t TIM3_Ch2_DutyCycle = 0;
float TIM3_Ch2_temp=0;
//tim3 ch3 
__IO uint16_t TIM3_Ch3_IC3ReadValue1 = 0, TIM3_Ch3_IC3ReadValue2 = 0;
__IO uint32_t TIM3_Ch3_Capture = 0;
__IO uint32_t TIM3_Ch3_Freq = 0;

uint32_t TIM3_Ch3_DutyCycle = 0;
float TIM3_Ch3_temp=0;
//tim3 ch4 
__IO uint16_t TIM3_Ch4_IC3ReadValue1 = 0, TIM3_Ch4_IC3ReadValue2 = 0;
__IO uint32_t TIM3_Ch4_Capture = 0;
__IO uint32_t TIM3_Ch4_Freq = 0;

uint32_t TIM3_Ch4_DutyCycle = 0;
float TIM3_Ch4_temp=0;
//tim4 ch3 
__IO uint16_t TIM4_Ch3_IC4ReadValue1 = 0, TIM4_Ch3_IC4ReadValue2 = 0;
__IO uint32_t TIM4_Ch3_Capture = 0;
__IO uint32_t TIM4_Ch3_Freq = 0;

uint32_t TIM4_Ch3_DutyCycle = 0;
float TIM4_Ch3_temp=0;
//tim4 ch4 
__IO uint16_t TIM4_Ch4_IC4ReadValue1 = 0, TIM4_Ch4_IC4ReadValue2 = 0;
__IO uint32_t TIM4_Ch4_Capture = 0;
__IO uint32_t TIM4_Ch4_Freq = 0;

uint32_t TIM4_Ch4_DutyCycle = 0;
float TIM4_Ch4_temp=0;

uint32_t CH2_DutyCycle_D = 0,CH2_DutyCycle_R = 0,CH3_DutyCycle_D = 0,CH3_DutyCycle_R = 0,CH4_DutyCycle = 0,CH5_DutyCycle = 0,CH6_DutyCycle = 0,CH7_DutyCycle = 0,CH8_DutyCycle = 0,CH9_DutyCycle = 0,CH10_DutyCycle = 0;
u8 ch2_duty_flag=0;
u8 soft_switch=0;
/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_TimeConfiguration(void)
{
  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}
/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void TIM2_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void TIM3_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void TIM4_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM4 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
/**
  * @}
  */ 
//tim2 ch1 init
void TIM2_Ch1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   /* TIM2 channel 1 pin (PA.00) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  TIM2_Ch1_ICInitStructure.TIM_Channel = TIM_Channel_1;
  TIM2_Ch1_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM2_Ch1_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM2_Ch1_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM2_Ch1_ICInitStructure.TIM_ICFilter = 0x0;

  TIM_ICInit(TIM2, &TIM2_Ch1_ICInitStructure);
}

//tim2 ch2 init
void TIM2_Ch2_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   /* TIM2 channel 2 pin (PA.01) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  TIM2_Ch2_ICInitStructure.TIM_Channel = TIM_Channel_2;
  TIM2_Ch2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM2_Ch2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM2_Ch2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM2_Ch2_ICInitStructure.TIM_ICFilter = 0x0;

  TIM_ICInit(TIM2, &TIM2_Ch2_ICInitStructure);
}

//tim2 ch3 init
void TIM2_Ch3_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   /* TIM2 channel 3 pin (PA.02) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  TIM2_Ch3_ICInitStructure.TIM_Channel = TIM_Channel_3;
  TIM2_Ch3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM2_Ch3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM2_Ch3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM2_Ch3_ICInitStructure.TIM_ICFilter = 0x0;

  TIM_ICInit(TIM2, &TIM2_Ch3_ICInitStructure);
}

//tim2 ch4 init
void TIM2_Ch4_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   /* TIM2 channel 4 pin (PA.03) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  TIM2_Ch4_ICInitStructure.TIM_Channel = TIM_Channel_4;
  TIM2_Ch4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM2_Ch4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM2_Ch4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM2_Ch4_ICInitStructure.TIM_ICFilter = 0x0;

  TIM_ICInit(TIM2, &TIM2_Ch4_ICInitStructure);
}
//tim2 configuration
void TIM2_Configuration(void)
{
  /* NVIC configuration */
  TIM2_NVIC_Configuration();
  /* TIM2 clock reset   */
  TIM2_Clock_Configuration();
	
  //TIM2_Ch1_Init();
  TIM2_Ch2_Init();
  TIM2_Ch3_Init();
  TIM2_Ch4_Init();
  /* TIM enable counter */
  TIM_Cmd(TIM2, ENABLE);

  /* Enable the CC1-4 Interrupt Request */
  TIM_ITConfig(TIM2, TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4, ENABLE);
}
//tim3 ch1 init
void TIM3_Ch1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   /* TIM3 channel 1 pin (PA.06) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  TIM3_Ch1_ICInitStructure.TIM_Channel = TIM_Channel_1;
  TIM3_Ch1_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM3_Ch1_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM3_Ch1_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM3_Ch1_ICInitStructure.TIM_ICFilter = 0x0;

  TIM_ICInit(TIM3, &TIM3_Ch1_ICInitStructure);
}

//tim3 ch2 init
void TIM3_Ch2_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   /* TIM3 channel 2 pin (PA.07) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  TIM3_Ch2_ICInitStructure.TIM_Channel = TIM_Channel_2;
  TIM3_Ch2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM3_Ch2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM3_Ch2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM3_Ch2_ICInitStructure.TIM_ICFilter = 0x0;

  TIM_ICInit(TIM3, &TIM3_Ch2_ICInitStructure);
}

//tim3 ch3 init
void TIM3_Ch3_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   /* TIM3 channel 3 pin (PB.00) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  TIM3_Ch3_ICInitStructure.TIM_Channel = TIM_Channel_3;
  TIM3_Ch3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM3_Ch3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM3_Ch3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM3_Ch3_ICInitStructure.TIM_ICFilter = 0x0;

  TIM_ICInit(TIM3, &TIM3_Ch3_ICInitStructure);
}

//tim3 ch4 init
void TIM3_Ch4_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   /* TIM3 channel 4 pin (PB.01) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  TIM3_Ch4_ICInitStructure.TIM_Channel = TIM_Channel_4;
  TIM3_Ch4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM3_Ch4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM3_Ch4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM3_Ch4_ICInitStructure.TIM_ICFilter = 0x0;

  TIM_ICInit(TIM3, &TIM3_Ch4_ICInitStructure);
}
//tim3 configuration
void TIM3_Configuration(void)
{
  /* NVIC configuration */
  TIM3_NVIC_Configuration();
  /* TIM3 clock reset   */
  TIM3_Clock_Configuration();
	
//  TIM3_Ch1_Init();
//  TIM3_Ch2_Init();
  TIM3_Ch3_Init();
//  TIM3_Ch4_Init();
  /* TIM enable counter */
  TIM_Cmd(TIM3, ENABLE);

  /* Enable the CC1-4 Interrupt Request */
//  TIM_ITConfig(TIM3, TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);
}
//tim4 ch3 init
void TIM4_Ch3_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   /* TIM4 channel 3 pin (PB.08) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  TIM4_Ch3_ICInitStructure.TIM_Channel = TIM_Channel_3;
  TIM4_Ch3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//TIM_ICPolarity_Falling;
  TIM4_Ch3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM4_Ch3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM4_Ch3_ICInitStructure.TIM_ICFilter = 0x0;

  TIM_ICInit(TIM4, &TIM4_Ch3_ICInitStructure);
}

//tim4 ch4 init
void TIM4_Ch4_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   /* TIM4 channel 4 pin (PB.09) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  TIM4_Ch4_ICInitStructure.TIM_Channel = TIM_Channel_4;
  TIM4_Ch4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//TIM_ICPolarity_Falling;
  TIM4_Ch4_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM4_Ch4_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM4_Ch4_ICInitStructure.TIM_ICFilter = 0x0;

  TIM_ICInit(TIM4, &TIM4_Ch4_ICInitStructure);
}
//tim4 configuration
void TIM4_Configuration(void)
{
  /* NVIC configuration */
  TIM4_NVIC_Configuration();
  /* TIM4 clock reset   */
  TIM4_Clock_Configuration();
	
  TIM4_Ch3_Init();
  TIM4_Ch4_Init();
  /* TIM enable counter */
  TIM_Cmd(TIM4, ENABLE);

  /* Enable the CC3-4 Interrupt Request */
  TIM_ITConfig(TIM4, TIM_IT_CC3|TIM_IT_CC4, ENABLE);
}
//TIM2_PWMHandleCallback for stm32f10x_it.c
void TIM2_PWMHandleCallback(void)
{
  //tim2 ch1 it
  if(TIM_GetITStatus(TIM2, TIM_IT_CC1) == SET) 
  {
    /* Clear TIM2 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0)
	{
	  TIM2_Ch1_IC2ReadValue1 = TIM_GetCapture1(TIM2);
	  
	  TIM2_Ch1_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM2, &TIM2_Ch1_ICInitStructure);
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 1)
	{
	  TIM2_Ch1_IC2ReadValue2 = TIM_GetCapture1(TIM2); 
	  if (TIM2_Ch1_IC2ReadValue2 > TIM2_Ch1_IC2ReadValue1)
      {
        TIM2_Ch1_Capture = (TIM2_Ch1_IC2ReadValue2 - TIM2_Ch1_IC2ReadValue1); 
      }
      else
      {
        TIM2_Ch1_Capture = ((0xFFFF - TIM2_Ch1_IC2ReadValue1) + TIM2_Ch1_IC2ReadValue2); 
      }
      /* Frequency computation */ 
      //TIM2_Ch1_Freq = (uint32_t) SystemCoreClock / TIM2_Ch1_Capture;
	  TIM2_Ch1_temp = (TIM2_Ch1_Capture*frequence_default/(uint32_t) TIM2_Clock)*1000;//占空比设为千分数
	  TIM2_Ch1_DutyCycle = 1000 - ((uint32_t)TIM2_Ch1_temp);
	  
	  TIM2_Ch1_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM2, &TIM2_Ch1_ICInitStructure);	  
	}
  }
  //tim2 ch2 it
  if(TIM_GetITStatus(TIM2, TIM_IT_CC2) == SET) 
  {
    /* Clear TIM2 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) == 0)
	{
	  TIM2_Ch2_IC2ReadValue1 = TIM_GetCapture2(TIM2);
	  
	  TIM2_Ch2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM2, &TIM2_Ch2_ICInitStructure);
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) == 1)
	{
	  TIM2_Ch2_IC2ReadValue2 = TIM_GetCapture2(TIM2); 
	  if (TIM2_Ch2_IC2ReadValue2 > TIM2_Ch2_IC2ReadValue1)
      {
        TIM2_Ch2_Capture = (TIM2_Ch2_IC2ReadValue2 - TIM2_Ch2_IC2ReadValue1); 
      }
      else
      {
        TIM2_Ch2_Capture = ((0xFFFF - TIM2_Ch2_IC2ReadValue1) + TIM2_Ch2_IC2ReadValue2); 
      }
      /* Frequency computation */ 
      //TIM2_Ch2_Freq = (uint32_t) SystemCoreClock / TIM2_Ch2_Capture;
	  TIM2_Ch2_temp = (TIM2_Ch2_Capture*frequence_default/(uint32_t) TIM2_Clock)*1000;//占空比设为千分数
	  TIM2_Ch2_DutyCycle = 1000 - ((uint32_t)TIM2_Ch2_temp);
	  CH10_DutyCycle = TIM2_Ch2_DutyCycle;
	  
	  TIM2_Ch2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM2, &TIM2_Ch2_ICInitStructure);	  
	}
  }
  //tim2 ch3 it
  if(TIM_GetITStatus(TIM2, TIM_IT_CC3) == SET) 
  {
    /* Clear TIM2 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) == 0)
	{
	  TIM2_Ch3_IC2ReadValue1 = TIM_GetCapture3(TIM2);
	  
	  TIM2_Ch3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM2, &TIM2_Ch3_ICInitStructure);
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) == 1)
	{
	  TIM2_Ch3_IC2ReadValue2 = TIM_GetCapture3(TIM2); 
	  if (TIM2_Ch3_IC2ReadValue2 > TIM2_Ch3_IC2ReadValue1)
      {
        TIM2_Ch3_Capture = (TIM2_Ch3_IC2ReadValue2 - TIM2_Ch3_IC2ReadValue1); 
      }
      else
      {
        TIM2_Ch3_Capture = ((0xFFFF - TIM2_Ch3_IC2ReadValue1) + TIM2_Ch3_IC2ReadValue2); 
      }
      /* Frequency computation */ 
      //TIM2_Ch3_Freq = (uint32_t) SystemCoreClock / TIM2_Ch3_Capture;
	  TIM2_Ch3_temp = (TIM2_Ch3_Capture*frequence_default/(uint32_t) TIM2_Clock)*1000;//占空比设为千分数
	  TIM2_Ch3_DutyCycle = 1000 - ((uint32_t)TIM2_Ch3_temp);
	  CH9_DutyCycle = TIM2_Ch3_DutyCycle;
	  
	  TIM2_Ch3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM2, &TIM2_Ch3_ICInitStructure);	  
	}
  }
  //tim2 ch4 it
  if(TIM_GetITStatus(TIM2, TIM_IT_CC4) == SET) 
  {
    /* Clear TIM2 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3) == 0)
	{
	  TIM2_Ch4_IC2ReadValue1 = TIM_GetCapture4(TIM2);
	  
	  TIM2_Ch4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM2, &TIM2_Ch4_ICInitStructure);
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3) == 1)
	{
	  TIM2_Ch4_IC2ReadValue2 = TIM_GetCapture4(TIM2); 
	  if (TIM2_Ch4_IC2ReadValue2 > TIM2_Ch4_IC2ReadValue1)
      {
        TIM2_Ch4_Capture = (TIM2_Ch4_IC2ReadValue2 - TIM2_Ch4_IC2ReadValue1); 
      }
      else
      {
        TIM2_Ch4_Capture = ((0xFFFF - TIM2_Ch4_IC2ReadValue1) + TIM2_Ch4_IC2ReadValue2); 
      }
      /* Frequency computation */ 
      //TIM2_Ch4_Freq = (uint32_t) SystemCoreClock / TIM2_Ch4_Capture;
	  TIM2_Ch4_temp = (TIM2_Ch4_Capture*frequence_default/(uint32_t) TIM2_Clock)*1000;//占空比设为千分数
	  TIM2_Ch4_DutyCycle = 1000 - ((uint32_t)TIM2_Ch4_temp);
	  CH8_DutyCycle = TIM2_Ch4_DutyCycle;
	  
	  TIM2_Ch4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM2, &TIM2_Ch4_ICInitStructure);	  
	}
  }
}
//TIM3_PWMHandleCallback for stm32f10x_it.c
void TIM3_PWMHandleCallback(void)
{
  //tim3 ch1 it
  if(TIM_GetITStatus(TIM3, TIM_IT_CC1) == SET) 
  {
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) == 0)
	{
	  TIM3_Ch1_IC3ReadValue1 = TIM_GetCapture1(TIM3);
	  
	  TIM3_Ch1_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM3, &TIM3_Ch1_ICInitStructure);
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) == 1)
	{
	  TIM3_Ch1_IC3ReadValue2 = TIM_GetCapture1(TIM3); 
	  if (TIM3_Ch1_IC3ReadValue2 > TIM3_Ch1_IC3ReadValue1)
      {
        TIM3_Ch1_Capture = (TIM3_Ch1_IC3ReadValue2 - TIM3_Ch1_IC3ReadValue1); 
      }
      else
      {
        TIM3_Ch1_Capture = ((0xFFFF - TIM3_Ch1_IC3ReadValue1) + TIM3_Ch1_IC3ReadValue2); 
      }
      /* Frequency computation */ 
      //TIM3_Ch1_Freq = (uint32_t) SystemCoreClock / TIM3_Ch1_Capture;
	  TIM3_Ch1_temp = (TIM3_Ch1_Capture*frequence_default/(uint32_t) TIM3_Clock)*1000;//占空比设为千分数
	  TIM3_Ch1_DutyCycle = 1000 - ((uint32_t)TIM3_Ch1_temp);
	  CH7_DutyCycle = TIM3_Ch1_DutyCycle;
	  
	  TIM3_Ch1_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM3, &TIM3_Ch1_ICInitStructure);	  
	}
  }
  //tim3 ch2 it
  if(TIM_GetITStatus(TIM3, TIM_IT_CC2) == SET) 
  {
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7) == 0)
	{
	  TIM3_Ch2_IC3ReadValue1 = TIM_GetCapture2(TIM3);
	  
	  TIM3_Ch2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM3, &TIM3_Ch2_ICInitStructure);
	}
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7) == 1)
	{
	  TIM3_Ch2_IC3ReadValue2 = TIM_GetCapture2(TIM3); 
	  if (TIM3_Ch2_IC3ReadValue2 > TIM3_Ch2_IC3ReadValue1)
      {
        TIM3_Ch2_Capture = (TIM3_Ch2_IC3ReadValue2 - TIM3_Ch2_IC3ReadValue1); 
      }
      else
      {
        TIM3_Ch2_Capture = ((0xFFFF - TIM3_Ch2_IC3ReadValue1) + TIM3_Ch2_IC3ReadValue2); 
      }
      /* Frequency computation */ 
      //TIM3_Ch2_Freq = (uint32_t) SystemCoreClock / TIM3_Ch2_Capture;
	  TIM3_Ch2_temp = (TIM3_Ch2_Capture*frequence_default/(uint32_t) TIM3_Clock)*1000;//占空比设为千分数
	  TIM3_Ch2_DutyCycle = 1000 - ((uint32_t)TIM3_Ch2_temp);
	  CH6_DutyCycle = TIM3_Ch2_DutyCycle;
	  
	  TIM3_Ch2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM3, &TIM3_Ch2_ICInitStructure);	  
	}
  }
  //tim3 ch3 it
  if(TIM_GetITStatus(TIM3, TIM_IT_CC3) == SET) 
  {
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) == 0)
	{
	  TIM3_Ch3_IC3ReadValue1 = TIM_GetCapture3(TIM3);
	  
	  TIM3_Ch3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM3, &TIM3_Ch3_ICInitStructure);
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) == 1)
	{
	  TIM3_Ch3_IC3ReadValue2 = TIM_GetCapture3(TIM3); 
	  if (TIM3_Ch3_IC3ReadValue2 > TIM3_Ch3_IC3ReadValue1)
      {
        TIM3_Ch3_Capture = (TIM3_Ch3_IC3ReadValue2 - TIM3_Ch3_IC3ReadValue1); 
      }
      else
      {
        TIM3_Ch3_Capture = ((0xFFFF - TIM3_Ch3_IC3ReadValue1) + TIM3_Ch3_IC3ReadValue2); 
      }
      /* Frequency computation */ 
      //TIM3_Ch3_Freq = (uint32_t) SystemCoreClock / TIM3_Ch3_Capture;
	  TIM3_Ch3_temp = (TIM3_Ch3_Capture*frequence_default/(uint32_t) TIM3_Clock)*1000;//占空比设为千分数
	  TIM3_Ch3_DutyCycle = 1000 - ((uint32_t)TIM3_Ch3_temp);
	  CH5_DutyCycle = TIM3_Ch3_DutyCycle;
		if(CH5_DutyCycle <= 100){soft_switch = 0;}//软开关关闭
    else {soft_switch = 1;};	  //软开关打开
	  TIM3_Ch3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM3, &TIM3_Ch3_ICInitStructure);	  
	}
  }
  //tim3 ch4 it
  if(TIM_GetITStatus(TIM3, TIM_IT_CC4) == SET) 
  {
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0)
	{
	  TIM3_Ch4_IC3ReadValue1 = TIM_GetCapture4(TIM3);
	  
	  TIM3_Ch4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM3, &TIM3_Ch4_ICInitStructure);
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 1)
	{
	  TIM3_Ch4_IC3ReadValue2 = TIM_GetCapture4(TIM3); 
	  if (TIM3_Ch4_IC3ReadValue2 > TIM3_Ch4_IC3ReadValue1)
      {
        TIM3_Ch4_Capture = (TIM3_Ch4_IC3ReadValue2 - TIM3_Ch4_IC3ReadValue1); 
      }
      else
      {
        TIM3_Ch4_Capture = ((0xFFFF - TIM3_Ch4_IC3ReadValue1) + TIM3_Ch4_IC3ReadValue2); 
      }
      /* Frequency computation */ 
      //TIM3_Ch4_Freq = (uint32_t) SystemCoreClock / TIM3_Ch4_Capture;
	  TIM3_Ch4_temp = (TIM3_Ch4_Capture*frequence_default/(uint32_t) TIM3_Clock)*1000;//占空比设为千分数
	  TIM3_Ch4_DutyCycle = 1000 - ((uint32_t)TIM3_Ch4_temp);
	  CH4_DutyCycle = TIM3_Ch4_DutyCycle;
	  
	  TIM3_Ch4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM3, &TIM3_Ch4_ICInitStructure);	  
	}
  }
}
//TIM4_PWMHandleCallback for stm32f10x_it.c
void TIM4_PWMHandleCallback(void)
{
  //tim2 ch3 it
  if(TIM_GetITStatus(TIM4, TIM_IT_CC3) == SET) 
  {
    /* Clear TIM4 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) == 1)
	{
	  TIM4_Ch3_IC4ReadValue1 = TIM_GetCapture3(TIM4);
	  
	  TIM4_Ch3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM4, &TIM4_Ch3_ICInitStructure);
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) == 0)
	{
	  TIM4_Ch3_IC4ReadValue2 = TIM_GetCapture3(TIM4); 
	  if (TIM4_Ch3_IC4ReadValue2 > TIM4_Ch3_IC4ReadValue1)
      {
        TIM4_Ch3_Capture = (TIM4_Ch3_IC4ReadValue2 - TIM4_Ch3_IC4ReadValue1); 
      }
      else
      {
        TIM4_Ch3_Capture = ((0xFFFF - TIM4_Ch3_IC4ReadValue1) + TIM4_Ch3_IC4ReadValue2); 
      }
      /* Frequency computation */ 
      //TIM4_Ch3_Freq = (uint32_t) SystemCoreClock / TIM4_Ch3_Capture;
	  TIM4_Ch3_temp = (TIM4_Ch3_Capture*frequence_default/(uint32_t) TIM4_Clock)*1000;//占空比设为千分数
	  TIM4_Ch3_DutyCycle = ((uint32_t)TIM4_Ch3_temp);
	  if(TIM4_Ch3_DutyCycle < 97)
	  {
		  ch2_duty_flag = 1;
	    CH2_DutyCycle_D = 97 - TIM4_Ch3_DutyCycle;
	  }
	  else if(TIM4_Ch3_DutyCycle > 106)//107
	  {
		  ch2_duty_flag = 2;
	    CH2_DutyCycle_R = TIM4_Ch3_DutyCycle - 106;
	  }
	  else
	  {
		  ch2_duty_flag = 0;
	    CH2_DutyCycle_D = 0;
		  CH2_DutyCycle_R = 0;
	  }
		  
	  TIM4_Ch3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM4, &TIM4_Ch3_ICInitStructure);	  
	}
  }
  //tim4 ch4 it
  if(TIM_GetITStatus(TIM4, TIM_IT_CC4) == SET) 
  {
    /* Clear TIM4 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM4, TIM_IT_CC4);
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9) == 1)
	{
	  TIM4_Ch4_IC4ReadValue1 = TIM_GetCapture4(TIM4);
	  
	  TIM4_Ch4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM4, &TIM4_Ch4_ICInitStructure);
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9) == 0)
	{
	  TIM4_Ch4_IC4ReadValue2 = TIM_GetCapture4(TIM4); 
	  if (TIM4_Ch4_IC4ReadValue2 > TIM4_Ch4_IC4ReadValue1)
      {
        TIM4_Ch4_Capture = (TIM4_Ch4_IC4ReadValue2 - TIM4_Ch4_IC4ReadValue1); 
      }
      else
      {
        TIM4_Ch4_Capture = ((0xFFFF - TIM4_Ch4_IC4ReadValue1) + TIM4_Ch4_IC4ReadValue2); 
      }
      /* Frequency computation */ 
      //TIM4_Ch4_Freq = (uint32_t) SystemCoreClock / TIM4_Ch4_Capture;
	  TIM4_Ch4_temp = (TIM4_Ch4_Capture*frequence_default/(uint32_t) TIM4_Clock)*1000;//占空比设为千分数
	  TIM4_Ch4_DutyCycle =  ((uint32_t)TIM4_Ch4_temp);
	  if(TIM4_Ch4_DutyCycle < 97)
	  {
	    CH3_DutyCycle_D = 97 - TIM4_Ch4_DutyCycle;
	  }
	  else if(TIM4_Ch4_DutyCycle > 107)
	  {
	    CH3_DutyCycle_R = TIM4_Ch4_DutyCycle - 107;
	  }
	  else
	  {
	    CH3_DutyCycle_D = 0;
		  CH3_DutyCycle_R = 0;
	  }
	  
	  TIM4_Ch4_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//|TIM_ICPolarity_Rising;//TIM_ICPolarity_Rising;
	  TIM_ICInit(TIM4, &TIM4_Ch4_ICInitStructure);	  
	}
  }
}
//radiolink_init
void radiolink_init(void)
{
  /* Gpio Clocks Configuration */
  RCC_TimeConfiguration();

  /* TIM2 TIM3 TIM4 configuration: Input Capture mode */
//  TIM2_Configuration();
  TIM3_Configuration();
  TIM4_Configuration();
}
//TIM2_Clock_Configuration
void TIM2_Clock_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    /* TIM2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseStructure.TIM_Prescaler = 72-1; //72M/72=1M
	TIM_TimeBaseStructure.TIM_Period = 65535;//20000;	//20ms overflow
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; //
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	
    /*load */
	//TIM_ARRPreloadConfig(TIM2, ENABLE);
    //clear IT bit
	//TIM_ClearITPendingBit(TIM2,TIM_IT_Update); 
    /* overflow interrupt*/
    //TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    /* allow*/
	//TIM_Cmd(TIM2, ENABLE);
}
//TIM3_Clock_Configuration
void TIM3_Clock_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    /* TIM3 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseStructure.TIM_Prescaler = 72-1; //72M/72=1M
	TIM_TimeBaseStructure.TIM_Period = 65535;//20000;	//20ms overflow
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; //
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);	
    /*load */
	//TIM_ARRPreloadConfig(TIM2, ENABLE);
    //clear IT bit
	//TIM_ClearITPendingBit(TIM2,TIM_IT_Update); 
    /* overflow interrupt*/
    //TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    /* allow*/
	//TIM_Cmd(TIM2, ENABLE);
}
//TIM4_Clock_Configuration
void TIM4_Clock_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    /* TIM4 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseStructure.TIM_Prescaler = 72-1; //72M/72=1M
	TIM_TimeBaseStructure.TIM_Period = 65535;//20000;	//20ms overflow
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; //
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);	
    /*load */
	//TIM_ARRPreloadConfig(TIM2, ENABLE);
    //clear IT bit
	//TIM_ClearITPendingBit(TIM2,TIM_IT_Update); 
    /* overflow interrupt*/
    //TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    /* allow*/
	//TIM_Cmd(TIM2, ENABLE);
}
/***************************************************************************************************
*\Function      sendRLDuty_thread_entry
*\Description   
*\Parameter     parameter
*\Return        void
*\Note          
*\Log           2019.01.28    Ver 1.0    baron
*               创建函数。
***************************************************************************************************/
void sendRLDuty_thread_entry(void* parameter)
{
	u32 duty_send_len = 0;
	u8 send_buf[32];
	rt_thread_delay(100);//
	
	while(1)
	{
	  if(ch2_duty_flag == 1)
	  {
	    duty_send_len = CAN_Read_CH2_DutyCycle_D(send_buf, sizeof(send_buf));
	  }
	  else if(ch2_duty_flag == 2)
	  {
	    duty_send_len = CAN_Read_CH2_DutyCycle_R(send_buf, sizeof(send_buf));
	  }
	  else
	  {
	    duty_send_len = CAN_Read_CH2_DutyCycle_N(send_buf, sizeof(send_buf));
	  }
	  //rt_thread_delay(1);
		if(soft_switch == 1)  //软开关已打开
		{
			if (duty_send_len)
			{
			 canbus_middle_send(&CAN,0x201, send_buf, duty_send_len);
//				rt_thread_delay(1);
//			 canbus_middle_send(&CAN,0x101, send_buf, duty_send_len);
			}
	  } 
	  rt_thread_delay(dev_param.time_interval/10);				
	}

}
/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
