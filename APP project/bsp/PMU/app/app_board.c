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
static switch_type MsgBlue_Switch = {GPIOC, GPIO_Pin_0, FALSE};//��ɫ
static switch_type MsgGreen_Switch = {GPIOC, GPIO_Pin_4, FALSE};//��ɫ
BreatheMsgLedType MsgBlue = {&MsgBlue_Switch};
BreatheMsgLedType MsgGreen  = {&MsgGreen_Switch};

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

//MAX9612 ģ��i2c for fifth 9611
static GpioType MAX9611_CLK_charge = {GPIOB, GPIO_Pin_10};
static GpioType MAX9611_SDA_charge = {GPIOB, GPIO_Pin_11};

static stm32_simu_i2c_type MAX9611_I2C_charge = {&MAX9611_CLK_charge, &MAX9611_SDA_charge};

MAX9611_type MAX9611_charge = {&MAX9611_I2C_charge};
//ADC����ֵ
//ϵͳ����״̬���
State_type state = {0,0,0,1,1,1,1};
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
	breathe_msg_led_init(&MsgBlue, "MsgBlue", 100, 100);
	breathe_msg_led_init(&MsgGreen, "MsgGreen", 100, 100);
	//����
	breathe_msg_led_on(&MsgBlue, 0, FALSE);
	breathe_msg_led_on(&MsgGreen, 0, FALSE);
	//һ���������Ƴ�ʼ��
	akey_start_init();
	
	stm32_CAN_init(&CAN);
	uart_ota_init();//
	
	stm32_uart_init(&UartTTL);         //���ڴ�ӡ
    //MCU�¶�
//    stm32_adc_with_dma_init(&ADC_InterT);
//    stm32_adc_channel_add(&ADC_InterT, 0, ADC_Channel_16, ADC_SampleTime_239Cycles5, RT_NULL);
//    stm32_adc_with_dma_start(&ADC_InterT);

	//��·MAX9611  MOS�ܿ���
	max9611_control_init();
	//MAX9611 �������
  max9611_init(&MAX9611);
	//MAX9611 ������� for fifth 9611
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
*\Description   pb14��ʼ����һ����������
*\Parameter     
*\Parameter     
*\Return        
*\Note          
*\Log           2019.02.11    Ver 1.0    baron               
				����������
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
	
  if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == Bit_RESET)//�������¼��
	{
	  GPIO_SetBits(GPIOB,GPIO_Pin_14);//��ʼ���øߣ������������¸ó������� ��ֻ�а������ܹػ�
		state.power_on = 1;
		state.charge_on = 0;
	}
	else
	{
	  GPIO_ResetBits(GPIOB,GPIO_Pin_14);//��ʼ���õͣ���絼�¸ó���������ֹͣ���ʱ�豸������
		state.power_on = 0;
		state.charge_on = 1;
	}
}
/*
 * ��������fputc
 * ����  ���ض���c�⺯��printf��USART1
 * ����  ����
 * ���  ����
 * ����  ����printf����
 */
//#ifdef USING_USART_PRINTF
//int fputc(int ch, FILE *f)
//{
//    stm32_uart_send(&UartTTL, (u8*)&ch, 1);
//    return ch;
//}
//#endif /* __GNUC__ */


