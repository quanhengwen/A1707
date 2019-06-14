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
static switch_type MsgGreen_Switch = {GPIOB, GPIO_Pin_13, FALSE};//��ɫ  pxu

BreatheMsgLedType MsgRed  = {&MsgRed_Switch};
BreatheMsgLedType MsgBlue = {&MsgBlue_Switch};
BreatheMsgLedType MsgGreen = {&MsgGreen_Switch};

//����
static GpioType UartTx = {GPIOA, GPIO_Pin_9};
static GpioType UartRx = {GPIOA, GPIO_Pin_10};
UartDevType UartTTL = {USART1, &UartTx, &UartRx, RT_NULL, 115200};

//CAN  RX PA11 TX PA12
static GpioType CAN_Tx = {GPIOA, GPIO_Pin_12};
static GpioType CAN_Rx = {GPIOA, GPIO_Pin_11};
CANBus_type CAN = {CAN1, &CAN_Tx, &CAN_Rx, CAN_BAUDRATE_500K};


//MAX9611 ģ��i2c
static GpioType MAX9611_CLK = {GPIOB, GPIO_Pin_6};
static GpioType MAX9611_SDA = {GPIOB, GPIO_Pin_7};
static stm32_simu_i2c_type MAX9611_I2C = {&MAX9611_CLK, &MAX9611_SDA};
MAX9611_type MAX9611 = {&MAX9611_I2C};

/*�������1*/
static drv_timer_type timer_2 = {TIM2, 800};
static GpioType moror1_out1 = {GPIOB, GPIO_Pin_10};
static GpioType moror1_out2 = {GPIOB, GPIO_Pin_2};
static GpioType moror1_out3 = {GPIOB, GPIO_Pin_1};
static GpioType moror1_out4 = {GPIOB, GPIO_Pin_0};
static GpioType moror1_en = {GPIOB, GPIO_Pin_11};
Stepper_motor_device_type stepper_motor1 = {&moror1_out1, &moror1_out2, &moror1_out3, &moror1_out4, &moror1_en, &timer_2};

/*�������2*/
static drv_timer_type timer_3 = {TIM3, 800};
static GpioType moror2_out1 = {GPIOA, GPIO_Pin_6};
static GpioType moror2_out2 = {GPIOA, GPIO_Pin_7};
static GpioType moror2_out3 = {GPIOC, GPIO_Pin_4};
static GpioType moror2_out4 = {GPIOC, GPIO_Pin_5};
static GpioType moror2_en = {GPIOA, GPIO_Pin_5};
Stepper_motor_device_type stepper_motor2 = {&moror2_out1, &moror2_out2, &moror2_out3, &moror2_out4, &moror2_en, &timer_3};



//MPU9250 ģ��i2c
static GpioType MPU9250_CLK = {GPIOB, GPIO_Pin_9};
static GpioType MPU9250_SDA = {GPIOB, GPIO_Pin_8};

static stm32_simu_i2c_type MPU9250_I2C = {&MPU9250_CLK, &MPU9250_SDA};

MPU9250_type MPU9250_dev = {&MPU9250_I2C};

/*����������*/
GpioType hall_1 = {GPIOB, GPIO_Pin_13};
GpioType hall_2 = {GPIOB, GPIO_Pin_15};
GpioType hall_3 = {GPIOB, GPIO_Pin_12};
GpioType hall_4 = {GPIOB, GPIO_Pin_14};

/*CAN���͵Ļ�����*/
rt_mutex_t mutex_can_send;


//MCU�ڲ��¶ȴ�����*
//static u16 ADC_InterT_Buf[32];
//ADC_DMA_Type ADC_InterT = {DMA1_Channel1, ADC1, 1, (u16*)&ADC_InterT_Buf, countof(ADC_InterT_Buf)};

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
	
	mutex_can_send = rt_mutex_create("can_s_Mut", RT_IPC_FLAG_FIFO);
    if (mutex_can_send == RT_NULL)
    {
        rt_kprintf("can send mutex create fail!\n");
        return;
    }
	
	stm32_CAN_init(&CAN);
	
	mpu9250_init(&MPU9250_dev);
	
	max9611_init(&MAX9611);
	
	stepper_motor_init(&stepper_motor1);
	stepper_motor_init(&stepper_motor2);
	
	config_gpio_to_input(&hall_1);
	config_gpio_to_input(&hall_2);
	config_gpio_to_input(&hall_3);
	config_gpio_to_input(&hall_4);
	
//	uart_ota_init();//
    //MCU�¶�
//    stm32_adc_with_dma_init(&ADC_InterT);
//    stm32_adc_channel_add(&ADC_InterT, 0, ADC_Channel_16, ADC_SampleTime_239Cycles5, RT_NULL);
//    stm32_adc_with_dma_start(&ADC_InterT);
	
}

