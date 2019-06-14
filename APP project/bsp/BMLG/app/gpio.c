/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          gpio.c
*\Description   ͨ���ж������ж�ͨ�ŵ�����
*\Note          
*\Log           2019.05.06    Ver 1.0    baron
*               �����ļ���
***************************************************************************************************/
#include "gpio.h"
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
u8 LED_Twinkle_Mode = 0xff;
u8 LED_LW_Mode = 0xff;
u8 LED_change_flag = 0;
GPIO_InitTypeDef   GPIO_InitStructure;

/**
  * @brief  Configure led control mode
  * @param  None
  * @retval None
  */
void led_gpio_init(void)
{
  /* Enable GPIOA clock */ 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* control pin (PA.08/05/06/07) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_8 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  /* Enable GPIOB clock */ 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	/* control pin (PB.12) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Enable GPIOC clock */ 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	/* control pin (PB.12) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	SW_OFF;//�������ر�
	PLED_EN_A_OFF;//ǰ�����ƹر�
	PLED_EN_B_OFF;
}
/**
  * @brief  Configure led twinkle mode17
  * @param  None �����ر�
  * @retval None
  */
void twinkle_mode17(void)
{
  SW_OFF;//�������ر�
}
/**
  * @brief  Configure led twinkle mode18
  * @param  None ��������
  * @retval None
  */
void twinkle_mode18(void)
{
  SW_ON;//����������
}
/**
  * @brief  Configure led twinkle mode19
  * @param  None ������˸
  * @retval None
  */
void twinkle_mode19(void)
{
  SW_ON;//����������
	rt_thread_delay(50);
	SW_OFF;//�������ر�
	rt_thread_delay(50);
}
/**
  * @brief  Configure led twinkle mode20
  * @param  None ǰ�����ƹر�
  * @retval None
  */
void twinkle_mode20(void)
{
  PLED_EN_A_OFF;
	PLED_EN_B_OFF;
}
/**
  * @brief  Configure led twinkle mode21
  * @param  None ǰ�����Ƴ���
  * @retval None
  */
void twinkle_mode21(void)
{
  PLED_EN_A_ON;
	PLED_EN_B_ON;
}
/**
  * @brief  Configure led twinkle mode22
  * @param  None ǰ��������˸
  * @retval None
  */
void twinkle_mode22(void)
{
  PLED_EN_A_ON;
	PLED_EN_B_ON;
	rt_thread_delay(50);
	PLED_EN_A_OFF;
	PLED_EN_B_OFF;
	rt_thread_delay(50);
}
/**
  * @brief  Configure led twinkle mode23
  * @param  None ǰ״̬�ƹر�
  * @retval None
  */
void twinkle_mode23(void)
{
  STATE_SW1_OFF;
	STATE_SW2_OFF;
}
/**
  * @brief  Configure led twinkle mode24
  * @param  None ǰ״̬�Ƴ���
  * @retval None
  */
void twinkle_mode24(void)
{
  STATE_SW1_ON;
	STATE_SW2_ON;
}
/**
  * @brief  Configure led twinkle mode25
  * @param  None ǰ״̬����˸
  * @retval None
  */
void twinkle_mode25(void)
{
  STATE_SW1_ON;
	STATE_SW2_ON;
	rt_thread_delay(50);
	STATE_SW1_OFF;
	STATE_SW2_OFF;
	rt_thread_delay(50);
}
/***************************************************************************************************
*\Function      USART_Send_Construct
*\Description   fe len data cs be 
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note
*\Log           2018.01.19    Ver 1.0    Job
*               ����������
***************************************************************************************************/
u32 USART_Send_Construct(u8* buf, u8 len)
{
	u32 data_len = 5;
	u32 i = 0;
  u8 cs = 0;
	if (len < data_len)
	{
		return 0;
	}

	buf[i++] = 0xfe;
	buf[i++] = 0x01;
	buf[i++] = LED_LW_Mode;
	buf[i++] = cs ^ LED_LW_Mode;
	buf[i++] = 0xbe;//remoteID
	return i;
}
/***************************************************************************************************
*\Function      LEDcontrol_thread_entry
*\Description   
*\Parameter     parameter
*\Return        void
*\Note          
*\Log           2019.05.06    Ver 1.0    baron
*               ����������
***************************************************************************************************/
void LEDcontrol_thread_entry(void* parameter)
{
	u8 len = 0;
	u8 USART_TXBuf[8];

	while(1)
	{
		rt_thread_delay(10);
		switch(LED_Twinkle_Mode)
		//switch(0x17)
		{
			case 0x00://��LED��˸ģʽ���ͳ�ȥ
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 0;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x01:
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 1;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x02:
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 2;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x03:
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 3;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x04:
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 4;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x05:
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 5;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x06:
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 6;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x07:
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 7;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x08:
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 8;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x09:
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 9;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x0a:
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 0x0a;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x0b:
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 0x0b;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x0c:
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 0x0c;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x0d:
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 0x0d;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x0e:
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 0x0e;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x0f:
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 0x0f;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x10:
			{
				if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 0x10;
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x11:
			{
        twinkle_mode17();// �����ر�
			}
			break;
			case 0x12:
			{
        twinkle_mode18();// ��������
			}
			break;
			case 0x13:
			{
        twinkle_mode19();// ������˸
			}
			break;
			case 0x14:
			{
        twinkle_mode20();// ǰ�����ƹر�
			}
			break;
			case 0x15:
			{
        twinkle_mode21();// ǰ�����Ƴ���
			}
			break;
			case 0x16:
			{
        twinkle_mode22();// ǰ��������˸
			}
			break;
			case 0x17:
			{
        twinkle_mode23();// ǰ״̬�ƹر�
			}
			break;
			case 0x18:
			{
        twinkle_mode24();// ǰ״̬�Ƴ���
			}
			break;
			case 0x19:
			{
        twinkle_mode25();// ǰ״̬����˸
			}
			break;
			case 0x1a:
			{
        if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 0x1a;  //�ư���ȫ��
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			case 0x1b:
			{
        if(LED_change_flag == 1)
				{
					LED_change_flag = 0;
					LED_LW_Mode = 0x1b;  //�ư�ȵ�ȫ��
					len = USART_Send_Construct(USART_TXBuf, 5);
					stm32_uart_send(&UartTTL, USART_TXBuf, 5);
				}
			}
			break;
			default: rt_thread_delay(10);break;
			}
	}

}
/**
  * @}
  */ 
