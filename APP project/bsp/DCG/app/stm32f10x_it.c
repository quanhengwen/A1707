/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include <drv_board.h>
#include <rtthread.h>
#include "app_board.h"
#include "threadinit.h"

u8 start_485T; 

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}


/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern UartDevType UartTTL;
void USART1_IRQHandler(void)
{
//	stm32_uart_isr(&UartTTL);
//#ifdef RT_USING_UART1
//    extern struct rt_device uart1_device;
//	  extern void rt_hw_serial_isr(struct rt_device *device);

//    /* enter interrupt */
//    rt_interrupt_enter();

//    rt_hw_serial_isr(&uart1_device);

//    /* leave interrupt */
//    rt_interrupt_leave();
//#endif
	
	static u8 data[100];
	static u8 reci=0;
	
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE))
	{
		 USART_ReceiveData(USART1);
	}

    if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE))	//½ÓÊÕ
	{
		data[reci] = USART_ReceiveData(USART1);
		
		if(data[reci]==0xee) // Ö¡Î²   ´®¿Ú ×ªCAN
		{
			if(data[reci-1]==0xbe)canbus_middle_send(&CAN, 0x500,data,reci);
		  else                  canbus_middle_send(&CAN, 0x100,data,reci);//reci+1
		}
		
		if(data[reci]==0xef) // Ö¡Í·
		{
		  reci=0;
		//	data[reci]=0xfe;
		}	
		else
		reci++;
		
		
		//Í¨ÓÃÖ¸Áî
//		if(data[reci]==0xBe) // Ö¡Î²   ´®¿Ú ×ªCAN
//		{
//		  canbus_middle_send(&CAN, 0x100,data,reci);//reci+1
//		}
//		
//		if(data[reci]==0xfe) // Ö¡Í·
//		{
//		  reci=0;
//			data[reci]=0xfe;
//		}	
//		reci++;
		
	}
	
	
	
}

/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USART3_IRQHandler(void)
{
	 u8 data = 0;
    /* ½ÓÊÕ */
    if(USART_GetFlagStatus(USART3, USART_FLAG_RXNE))	 
    {
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
        
			
        data = USART_ReceiveData(USART3);		
			  motor485.recvbuf[motor485.rec_i]=data;
			  motor485.rec_i++;
			  if(motor485.rec_i>19) motor485.rec_i=0;
			
			  if(motor485.rec_i>(motor485.recvbuf[2]+4)) //Êý¾Ý³¤¶È
				{
				  if(motor485.recvbuf[1]==0x03)  //¹¦ÄÜÂë¶ÁÈ¡
					{
					  rt_sem_release(&motor485_sem);	// ÊÍ·ÅÐÅºÅÁ¿ ´®¿Ú½âÎö¡
					}
				}
			

				
		  //  uart3SendByte(data);
//			  USART_SendData(USART3, data);
    }
	
//    if(USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
//    {

//        USART_ReceiveData(USART3); 
//        USART_ClearFlag(USART3, USART_FLAG_ORE);
//    }
//    
//    if (USART_GetFlagStatus(USART3, USART_FLAG_ORE))
//    {
//    	 USART_ReceiveData(USART3);
//    }
//    
//    if(USART_GetFlagStatus(USART3, USART_FLAG_NE) != RESET)
//    {
//        USART_ClearFlag(USART3, USART_FLAG_NE);
//    }

//    if(USART_GetFlagStatus(USART3, USART_FLAG_FE) != RESET)
//    {
//        USART_ClearFlag(USART3, USART_FLAG_FE);
//    }

//    if(USART_GetFlagStatus(USART3, USART_FLAG_PE) != RESET)
//    {
//        USART_ClearFlag(USART3, USART_FLAG_PE);
//    }	
    
}


/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{
#ifdef RT_USING_UART2
//    extern struct rt_device uart2_device;
//	extern void rt_hw_serial_isr(struct rt_device *device);

//    /* enter interrupt */
//    rt_interrupt_enter();

//    rt_hw_serial_isr(&uart2_device);

//    /* leave interrupt */
//    rt_interrupt_leave();
#endif
}

/**
  * @}
  */
//extern CANBus_type CAN;
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	stm32_CAN_recv_isr(&CAN, CAN_FIFO0);
}

/**
  * @}
  */


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
