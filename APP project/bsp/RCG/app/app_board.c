/***************************************************************************************************
*                    (c) Copyright 2008-2017  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          app_board.c
*\Description   
*\Note          
*\Log           2017.06.02    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#include "app_board.h"
#include "param.h"
#include "uart_ota_protocol.h"
//ָʾ��
static switch_type MsgRed_Switch = {GPIOC, GPIO_Pin_1, FALSE};//��ɫ
static switch_type MsgBlue_Switch = {GPIOC, GPIO_Pin_0, FALSE};//��ɫ

BreatheMsgLedType MsgRed  = {&MsgRed_Switch};
BreatheMsgLedType MsgBlue = {&MsgBlue_Switch};

//����
static GpioType UartTx = {GPIOA, GPIO_Pin_9};
static GpioType UartRx = {GPIOA, GPIO_Pin_10};
UartDevType UartTTL = {USART1, &UartTx, &UartRx, RT_NULL, 115200};

//CAN  RX PA11 TX PA12
static GpioType CAN_Tx = {GPIOA, GPIO_Pin_12};
static GpioType CAN_Rx = {GPIOA, GPIO_Pin_11};
CANBus_type CAN = {CAN1, &CAN_Tx, &CAN_Rx, CAN_BAUDRATE_500K};

//MCU�ڲ��¶ȴ�����*
//static u16 ADC_InterT_Buf[32];
//ADC_DMA_Type ADC_InterT = {DMA1_Channel1, ADC1, 1, (u16*)&ADC_InterT_Buf, countof(ADC_InterT_Buf)};

//MAX9612 ģ��i2c
static GpioType MAX9611_CLK = {GPIOB, GPIO_Pin_6};
static GpioType MAX9611_SDA = {GPIOB, GPIO_Pin_7};

static stm32_simu_i2c_type MAX9611_I2C = {&MAX9611_CLK, &MAX9611_SDA};

MAX9611_type MAX9611 = {&MAX9611_I2C};

/***************************************************************************************************
*\Function      app_board_init
*\Description   ��ʼ�����б���
*\Parameter     
*\Return        void
*\Note          1��ע����������Ӧ��֮ǰ��ʼ��
*\Log           2017.06.02    Ver 1.0    Job               
				����������
***************************************************************************************************/
void app_board_init(void)
{
		//RTCʱ��
		stm32_rtc_init();

		//ָʾ��
		breathe_msg_led_init(&MsgRed, "MsgRed",   100, 100);
		breathe_msg_led_init(&MsgBlue, "MsgBlue", 100, 100);

		//����
		breathe_msg_led_on(&MsgRed, 0, FALSE);
		breathe_msg_led_on(&MsgBlue, 0, FALSE);

		stm32_CAN_init(&CAN);
		uart_ota_init();//
		//MAX9611 �������
		max9611_init(&MAX9611);
		//radiolink 10·pwmռ�ձȲɼ�
		radiolink_init();
	  /* Configure PA.02 in interrupt mode */
    EXTI2_Config();
    //MCU�¶�
//    stm32_adc_with_dma_init(&ADC_InterT);
//    stm32_adc_channel_add(&ADC_InterT, 0, ADC_Channel_16, ADC_SampleTime_239Cycles5, RT_NULL);
//    stm32_adc_with_dma_start(&ADC_InterT);
	
}
//int fputc(int ch, FILE *f)
//{
//    stm32_uart_send(&USART1, (u8*)&ch, 1);
//    return ch;
//}
