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
static switch_type MsgBlue_Switch = {GPIOC, GPIO_Pin_0, FALSE};//蓝色
static switch_type MsgGreen_Switch = {GPIOC, GPIO_Pin_4, FALSE};//绿色
BreatheMsgLedType MsgBlue = {&MsgBlue_Switch};
BreatheMsgLedType MsgGreen  = {&MsgGreen_Switch};

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

//MAX9612 模拟i2c
static GpioType MAX9611_CLK = {GPIOB, GPIO_Pin_6};
static GpioType MAX9611_SDA = {GPIOB, GPIO_Pin_7};

static stm32_simu_i2c_type MAX9611_I2C = {&MAX9611_CLK, &MAX9611_SDA};

MAX9611_type MAX9611 = {&MAX9611_I2C};

//MAX9612 模拟i2c for fifth 9611
static GpioType MAX9611_CLK_charge = {GPIOB, GPIO_Pin_10};
static GpioType MAX9611_SDA_charge = {GPIOB, GPIO_Pin_11};

static stm32_simu_i2c_type MAX9611_I2C_charge = {&MAX9611_CLK_charge, &MAX9611_SDA_charge};

MAX9611_type MAX9611_charge = {&MAX9611_I2C_charge};
//ADC采样值
//系统运行状态检测
State_type state = {0,0,0,1,1,1,1};
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
	breathe_msg_led_init(&MsgBlue, "MsgBlue", 100, 100);
	breathe_msg_led_init(&MsgGreen, "MsgGreen", 100, 100);
	//启动
	breathe_msg_led_on(&MsgBlue, 0, FALSE);
	breathe_msg_led_on(&MsgGreen, 0, FALSE);
	//一键启动控制初始化
	akey_start_init();
	
	stm32_CAN_init(&CAN);
	uart_ota_init();//
	
	stm32_uart_init(&UartTTL);         //串口打印
    //MCU温度
//    stm32_adc_with_dma_init(&ADC_InterT);
//    stm32_adc_channel_add(&ADC_InterT, 0, ADC_Channel_16, ADC_SampleTime_239Cycles5, RT_NULL);
//    stm32_adc_with_dma_start(&ADC_InterT);

	//四路MAX9611  MOS管控制
	max9611_control_init();
	//MAX9611 电流检测
  max9611_init(&MAX9611);
	//MAX9611 电流检测 for fifth 9611
  max9611_init_charge(&MAX9611_charge);
  //ADC init
	ADC1_Init();
	
	ON_36V01;
	ON_36V02;
	ON_36V03;
	ON_36V04;
	printf("System Init Finish...\n\r");
}
/***************************************************************************************************
*\Function      akey_tostart
*\Description   pb14初始化，一键启动开启
*\Parameter     
*\Parameter     
*\Return        
*\Note          
*\Log           2019.02.11    Ver 1.0    baron               
				创建函数。
***************************************************************************************************/
void akey_start_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
   /* start pin (PB.14) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);
  /* stop pin (PB.15) configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == Bit_RESET)//按键按下检测
	{
	  GPIO_SetBits(GPIOB,GPIO_Pin_14);//初始化置高，开机按键导致该程序启动 ，只有按键才能关机
		state.power_on = 1;
		state.charge_on = 0;
	}
	else
	{
	  GPIO_ResetBits(GPIOB,GPIO_Pin_14);//初始化置低，充电导致该程序启动，停止充电时设备不运行
		state.power_on = 0;
		state.charge_on = 1;
	}
}
/*
 * 函数名：fputc
 * 描述  ：重定向c库函数printf到USART1
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 */
//#ifdef USING_USART_PRINTF
//int fputc(int ch, FILE *f)
//{
//    stm32_uart_send(&UartTTL, (u8*)&ch, 1);
//    return ch;
//}
//#endif /* __GNUC__ */


