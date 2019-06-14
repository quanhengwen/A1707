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
#include "uart_ota_protocol.h"
//ָʾ��
static switch_type MsgRed_Switch = {GPIOB, GPIO_Pin_5, FALSE};//��ɫ
static switch_type MsgBlue_Switch = {GPIOB, GPIO_Pin_1, FALSE};//��ɫ

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
	/*
    stm32_rtc_init();

	*/
	 stm32_CAN_init(&CAN);
	 control_Init();
	 ADC1_Init();	
}

