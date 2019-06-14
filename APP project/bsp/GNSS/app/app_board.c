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
#include <stdio.h>
#include <stdlib.h>  
#include <string.h>

//指示灯
//static switch_type MsgRed_Switch = {GPIOB, GPIO_Pin_5, FALSE};//红色
//static switch_type MsgBlue_Switch = {GPIOB, GPIO_Pin_1, FALSE};//蓝色
//static switch_type MsgGreen_Switch = {GPIOB, GPIO_Pin_13, FALSE};//绿色  pxu
static switch_type MsgGNSS_Switch = {GPIOC, GPIO_Pin_0, FALSE};//GNSS

//BreatheMsgLedType MsgRed  = {&MsgRed_Switch};
//BreatheMsgLedType MsgBlue = {&MsgBlue_Switch};
//BreatheMsgLedType MsgGreen = {&MsgGreen_Switch};
BreatheMsgLedType MsgGNSS = {&MsgGNSS_Switch};

//串口
static GpioType UartTx = {GPIOA, GPIO_Pin_9};
static GpioType UartRx = {GPIOA, GPIO_Pin_10};
UartDevType UartTTL = {USART1, &UartTx, &UartRx, RT_NULL, 115200};

//CAN  RX PA11 TX PA12
static GpioType CAN_Tx = {GPIOA, GPIO_Pin_12};
static GpioType CAN_Rx = {GPIOA, GPIO_Pin_11};
CANBus_type CAN = {CAN1, &CAN_Tx, &CAN_Rx, CAN_BAUDRATE_500K};

//MCU内部温度传感器*
//static u16 ADC_InterT_Buf[32];
//ADC_DMA_Type ADC_InterT = {DMA1_Channel1, ADC1, 1, (u16*)&ADC_InterT_Buf, countof(ADC_InterT_Buf)};

//GPS--GI200
static GpioType GI200UartTx = {GPIOA, GPIO_Pin_2};
static GpioType GI200UartRx = {GPIOA, GPIO_Pin_3};
UartDevType GI200UartTTL = {USART2, &GI200UartTx, &GI200UartRx, RT_NULL, 115200};
//static GpioType GI200GPS_Pulse = {GPIOA, GPIO_Pin_4};
//static switch_type GI200GPS_RST = {GPIOA, GPIO_Pin_5, FALSE};
 switch_type GI200GPS_PWCtl = {GPIOC, GPIO_Pin_6, FALSE};

//GPS--F9P
static GpioType F9PUartTx = {GPIOC, GPIO_Pin_12};
static GpioType F9PUartRx = {GPIOD, GPIO_Pin_2};
UartDevType F9PUartTTL = {UART5, &F9PUartTx, &F9PUartRx, RT_NULL, 115200};
//static GpioType F9PGPS_Pulse = {GPIOC, GPIO_Pin_11};
//static switch_type F9PGPS_RST = {GPIOC, GPIO_Pin_12, FALSE};
 switch_type F9PGPS_PWCtl = {GPIOB, GPIO_Pin_5, FALSE};

 //spi flash
static switch_type SPI2_CS = {GPIOB, GPIO_Pin_12, FALSE};
static GpioType SPI2_CLK = {GPIOB, GPIO_Pin_13};
static GpioType SPI2_MOSI = {GPIOB, GPIO_Pin_15};
static GpioType SPI2_MISO = {GPIOB, GPIO_Pin_14};

static SpiDevType SPI2_Dev = {SPI2, &SPI2_CS, &SPI2_CLK, &SPI2_MOSI, &SPI2_MISO, \
36*1000*1000, SPI_DataSize_8b, SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_FirstBit_MSB};

w25qxx_type  W25Q64 = {&SPI2_Dev, 4096};
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
//	breathe_msg_led_init(&MsgRed, "MsgRed",   100, 100);
//	breathe_msg_led_init(&MsgBlue, "MsgBlue", 100, 100);
//	breathe_msg_led_init(&MsgGreen, "MsgGreen", 100, 100);
	breathe_msg_led_init(&MsgGNSS, "MsgGNSS", 100, 100);
    //启动
//    breathe_msg_led_on(&MsgRed, 0, FALSE);
//    breathe_msg_led_on(&MsgBlue, 0, FALSE);
	breathe_msg_led_on(&MsgGNSS, 0, FALSE);
	stm32_CAN_init(&CAN);
//	uart_ota_init();//
    //MCU温度
//    stm32_adc_with_dma_init(&ADC_InterT);
//    stm32_adc_channel_add(&ADC_InterT, 0, ADC_Channel_16, ADC_SampleTime_239Cycles5, RT_NULL);
//    stm32_adc_with_dma_start(&ADC_InterT);
    //GPS
    gps_init();



}

//GPS定位
void GPS(void)
{
//    u8 buf[20];
//    rt_kprintf("V:%d\n", GPS_Dev.valid_flag);

//    snprintf((char*)buf, 20, "%f", GPS_Dev.latitude);
//    rt_kprintf("%s\n", buf);

//    snprintf((char*)buf, 20, "%f", GPS_Dev.longitude);
//    rt_kprintf("%s\n", buf);

//    rt_kprintf("Satellite_Track:%d\n", GPS_Dev.satellite_track);
//    rt_kprintf("Satellite_View:%d\n", GPS_Dev.satellite_view);

//    rt_kprintf("GPS_Sync[%d]\r\n", GPS_Dev.ErrorFlag);
}

#ifdef RT_USING_FINSH
#include "finsh.h"


FINSH_FUNCTION_EXPORT(GPS, GPS...)

#endif


