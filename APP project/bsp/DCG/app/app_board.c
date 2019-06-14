/***************************************************************************************************
*                    (c) Copyright 2008-2017  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          app_board.c
*\Description   
*\Note          
*\Log           2017.06.02    Ver 1.0    Job
*               ´´½¨ÎÄ¼þ¡£
***************************************************************************************************/
#include "app_board.h"
#include "param.h"
#include "uart_ota_protocol.h"

//485 Óëµç»úÇý¶¯Æ÷Í¨ÐÅ ´®¿Ú3 
#define  SET_485DE     GPIO_SetBits(GPIOB, GPIO_Pin_12);
#define  RESET_485DE   GPIO_ResetBits(GPIOB, GPIO_Pin_12);

//Ö¸Ê¾µÆ
static switch_type MsgRed_Switch = {GPIOC, GPIO_Pin_1, FALSE};//ºìÉ«
static switch_type MsgBlue_Switch = {GPIOC, GPIO_Pin_0, FALSE};//À¶É«
//static switch_type MsgGreen_Switch = {GPIOB, GPIO_Pin_13, FALSE};//ÂÌÉ«  pxu

BreatheMsgLedType MsgRed  = {&MsgRed_Switch};
BreatheMsgLedType MsgBlue = {&MsgBlue_Switch};
//BreatheMsgLedType MsgGreen = {&MsgGreen_Switch};

//´®¿Ú
static GpioType UartTx = {GPIOA, GPIO_Pin_9};
static GpioType UartRx = {GPIOA, GPIO_Pin_10};
UartDevType UartTTL = {USART1, &UartTx, &UartRx, RT_NULL, 115200};

//CAN  RX PA11 TX PA12
static GpioType CAN_Tx = {GPIOA, GPIO_Pin_12};
static GpioType CAN_Rx = {GPIOA, GPIO_Pin_11};
CANBus_type CAN = {CAN1, &CAN_Tx, &CAN_Rx, CAN_BAUDRATE_500K};

//MCUÄÚ²¿ÎÂ¶È´«¸ÐÆ÷*
//static u16 ADC_InterT_Buf[32];
//ADC_DMA_Type ADC_InterT = {DMA1_Channel1, ADC1, 1, (u16*)&ADC_InterT_Buf, countof(ADC_InterT_Buf)};


void NVIC_Configuration_uart3(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/*
	 * UART3?D??ó??è?? 485
	 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}




void uart3Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOB, ENABLE);
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

// ÅäÖÃ485ÊÕ·¢ÇÐ»»¹Ü½Å PB12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
// ³£Ì¬Îª½ÓÊÕ
  GPIO_ResetBits(GPIOB, GPIO_Pin_12);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure); 
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  GPIO_PinRemapConfig(GPIO_PartialRemap_USART3,ENABLE);
	USART_Cmd(USART3, ENABLE);
  
	USART_ClearFlag(USART3, USART_FLAG_TC);	
  NVIC_Configuration_uart3();
}

u8 uart3SendStr(u8* buf, u32 len)
{
	u8  err;
	u32 i;
   SET_485DE;	// ÉèÖÃÎª·¢ËÍ
	 USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); //È¡ÏûÖÐ¶Ï½ÓÊÕ
//	OSTimeDly(9);
	for (i = 0; i < len; i++)
	{
		USART_SendData(USART3, buf[i]);
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	//µÈ´ý·¢ËÍÍê±Ï
    //while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	
	}
//  OSTimeDly(2);
  RESET_485DE;	// ÉèÖÃÎª½ÓÊÕ
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  //·¢ËÍÍêÊý¾Ý¿ªÆô½ÓÊÕÖÐ¶Ï
    return len;
}

u8 uart3SendByte(u8 data)
{
    SET_485DE;	// ÉèÖÃÎª·¢ËÍ
//	  USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); //È¡ÏûÖÐ¶Ï½ÓÊÕ
    USART_SendData(USART3, data);
	  while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	//µÈ´ý·¢ËÍÍê±Ï
//	  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  //·¢ËÍÍêÊý¾Ý¿ªÆô½ÓÊÕÖÐ¶Ï
    RESET_485DE;	// ÉèÖÃÎª½ÓÊÕ
	return 0;
}



/***************************************************************************************************
*\Function      app_board_init
*\Description   ³õÊ¼»¯ËùÓÐ±äÁ¿
*\Parameter     
*\Return        void
*\Note          1¡¢×¢Òâ±ØÐëÔÚÏà¹ØÓ¦ÓÃÖ®Ç°³õÊ¼»¯
*\Log           2017.06.02    Ver 1.0    Job               
				´´½¨º¯Êý¡£
***************************************************************************************************/
void app_board_init(void)
{
    //RTCÊ±ÖÓ
  //stm32_rtc_init();

    //Ö¸Ê¾µÆ
	breathe_msg_led_init(&MsgRed, "MsgRed",   100, 100);
   //Æô¶¯
  breathe_msg_led_on(&MsgRed, 0, FALSE);

	stm32_CAN_init(&CAN);
//	uart_ota_init();//
    //MCUÎÂ¶È
//    stm32_adc_with_dma_init(&ADC_InterT);
//    stm32_adc_channel_add(&ADC_InterT, 0, ADC_Channel_16, ADC_SampleTime_239Cycles5, RT_NULL);
//    stm32_adc_with_dma_start(&ADC_InterT);
	TIM2_PWM_Init();  //¿ØÖÆµç»úµÄ
	uart3Init();
	//MAX9611 µçÁ÷¼ì²â
  max9611_init(&MAX9611);
}

