/***************************************************************************************************
*                    (c) Copyright 2008-2017  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          app_board.c
*\Description   
*\Note          
*\Log           2017.06.02    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#include "app_board.h"
#include "param.h"
#include "uart_ota_protocol.h"
//指示灯
static switch_type MsgRed_Switch = {GPIOC, GPIO_Pin_1, FALSE};//红色
static switch_type MsgBlue_Switch = {GPIOC, GPIO_Pin_0, FALSE};//蓝色

BreatheMsgLedType MsgRed  = {&MsgRed_Switch};
BreatheMsgLedType MsgBlue = {&MsgBlue_Switch};

//串口1
static GpioType Uart1_Tx = {GPIOA, GPIO_Pin_9};
static GpioType Uart1_Rx = {GPIOA, GPIO_Pin_10};
UartDevType UartTTL = {USART1, &Uart1_Tx, &Uart1_Rx, RT_NULL, 115200};

//串口2
static GpioType Uart2_Tx = {GPIOA, GPIO_Pin_2};
static GpioType Uart2_Rx = {GPIOA, GPIO_Pin_3};
UartDevType Comm_Uart2 = {USART2, &Uart2_Tx, &Uart2_Rx, RT_NULL, 115200};

//采集霍尔输出信号
static u16 ADC_Hall_Buf[100];
ADC_DMA_Type ADC_Hall = {DMA1_Channel1, ADC1, 1, (u16*)&ADC_Hall_Buf, countof(ADC_Hall_Buf)};

//CAN  RX PA11 TX PA12
static GpioType CAN_Tx = {GPIOA, GPIO_Pin_12};
static GpioType CAN_Rx = {GPIOA, GPIO_Pin_11};
CANBus_type CAN = {CAN1, &CAN_Tx, &CAN_Rx, CAN_BAUDRATE_500K};

//MCU内部温度传感器*
//static u16 ADC_InterT_Buf[32];
//ADC_DMA_Type ADC_InterT = {DMA1_Channel1, ADC1, 1, (u16*)&ADC_InterT_Buf, countof(ADC_InterT_Buf)};

//MAX9611 模拟i2c
static GpioType MAX9611_CLK = {GPIOB, GPIO_Pin_6};
static GpioType MAX9611_SDA = {GPIOB, GPIO_Pin_7};

static stm32_simu_i2c_type MAX9611_I2C = {&MAX9611_CLK, &MAX9611_SDA};

MAX9611_type MAX9611 = {&MAX9611_I2C};

//灯带控制端口
//static GpioType MLDG_Control = {GPIOB, GPIO_Pin_0};
/***************************************************************************************************
*\Function      app_board_init
*\Description   初始化所有变量
*\Parameter     
*\Return        void
*\Note          1、注意必须在相关应用之前初始化
*\Log           2017.06.02    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void app_board_init(void)
{
    //RTC时钟
	stm32_rtc_init();

    //指示灯
	breathe_msg_led_init(&MsgRed, "MsgRed",   100, 100);
	breathe_msg_led_init(&MsgBlue, "MsgBlue", 100, 100);
	
    //启动
	breathe_msg_led_on(&MsgRed, 0, FALSE);
	breathe_msg_led_on(&MsgBlue, 0, FALSE);

	stm32_CAN_init(&CAN);
	uart_ota_init();//
	
	//MAX9611 电流检测
	max9611_init(&MAX9611);

	//串口——激光测距
	stm32_uart_init(&Comm_Uart2);
	
	//HALL检测
    stm32_adc_with_dma_init(&ADC_Hall);
    stm32_adc_channel_add(&ADC_Hall, 0, ADC_Channel_8, ADC_SampleTime_239Cycles5, RT_NULL);
    stm32_adc_with_dma_start(&ADC_Hall);
	
}

