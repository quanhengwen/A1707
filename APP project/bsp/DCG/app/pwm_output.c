/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * �ļ���  ��pwm_output.c
 * ����    ��         
 * ʵ��ƽ̨��Ұ��STM32������
 * Ӳ�����ӣ�---------------------
 *          |  PA.06: (TIM2_CH1)  |
 *          |  PA.07: (TIM2_CH2)  |
 *      	|  PB.00: (TIM2_CH3)  | 
 *    		|  PB.01: (TIM2_CH4)  |
 *           ---------------------    			
 * ��汾  ��ST3.5.0
 * ����    ��wildfire team 
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
**********************************************************************************/
#include "pwm_output.h"


u16  CCR_Va0=0; 


static void MOTOR_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 

  /* GPIOA 1 (IN2)���� GPIOA 2 (IN3)ɲ�� */
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_1|GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;		    // �����
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits (GPIOA,GPIO_Pin_1);    //����  �� ��ת �� ��ת
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);    //�ϵ�ɲ��  ��ɲ�� 
///	GPIO_SetBits (GPIOA,GPIO_Pin_2);    //���� �ϵ粻ɲ��
}

/*
 * ��������TIM2_GPIO_Config
 * ����  ������TIM2�������PWMʱ�õ���I/O
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 
 
 
 */
static void TIM2_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

	/* TIM2 clock enable */
	//PCLK1����2��Ƶ����ΪTIM2��ʱ��Դ����72MHz
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 

  /*GPIOA Configuration: TIM2 channel 1 and 2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_0 ;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;		    // �����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
//	GPIO_ResetBits(GPIOA,GPIO_Pin_0);  // PWM 
	
//	GPIO_SetBits (GPIOA,GPIO_Pin_0);  // PWM 
}

/*
 * ��������TIM2_Mode_Config
 * ����  ������TIM2�����PWM�źŵ�ģʽ�������ڡ����ԡ�ռ�ձ�
 * ����  ����
 * ���  ����
 * ����  ���ڲ�����
 */
static void TIM2_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* PWM�źŵ�ƽ����ֵ */
	u16 CCR5_Val = 999; 
	u16 CCR1_Val = 500;        
	u16 CCR2_Val = 375;
	u16 CCR3_Val = 250;
	u16 CCR4_Val = 125;

/* -----------------------------------------------------------------------
    TIM2 Configuration: generate 4 PWM signals with 4 different duty cycles:
    TIM2CLK = 72 MHz, Prescaler = 0x0, TIM2 counter clock = 72 MHz
    TIM2 ARR Register = 999 => TIM2 Frequency = TIM2 counter clock/(ARR + 1)
    TIM2 Frequency = 72 KHz.
    TIM2 Channel1 duty cycle = (TIM2_CCR1/ TIM2_ARR)* 100 = 50%
    TIM2 Channel2 duty cycle = (TIM2_CCR2/ TIM2_ARR)* 100 = 37.5%
    TIM2 Channel3 duty cycle = (TIM2_CCR3/ TIM2_ARR)* 100 = 25%
    TIM2 Channel4 duty cycle = (TIM2_CCR4/ TIM2_ARR)* 100 = 12.5%
  ----------------------------------------------------------------------- */

  /* Time base configuration */		 
  TIM_TimeBaseStructure.TIM_Period = 999;       //����ʱ����0������999����Ϊ1000�Σ�Ϊһ����ʱ����
  TIM_TimeBaseStructure.TIM_Prescaler = 0;	    //����Ԥ��Ƶ����Ԥ��Ƶ����Ϊ72MHz
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//����ʱ�ӷ�Ƶϵ��������Ƶ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���ģʽ

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = 0;	   //��������ֵ�������������������ֵʱ����ƽ�������� ���ó�0 �ϵ粻��
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ

  TIM_OC1Init(TIM2, &TIM_OCInitStructure);	 //ʹ��ͨ��1

  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);


  TIM_ARRPreloadConfig(TIM2, ENABLE);			 // ʹ��TIM2���ؼĴ���ARR

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);                   //ʹ�ܶ�ʱ��3	
}


void  PWM_out( u16 CCR_Val)
{
// TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
 /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //����ΪPWMģʽ1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = CCR_Val;	   //��������ֵ�������������������ֵʱ����ƽ��������
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ

  TIM_OC1Init(TIM2, &TIM_OCInitStructure);	 //ʹ��ͨ��1

  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);


  TIM_ARRPreloadConfig(TIM2, ENABLE);			 // ʹ��TIM2���ؼĴ���ARR

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);                   //ʹ�ܶ�ʱ��3	
}





/*
 * ��������TIM2_PWM_Init
 * ����  ��TIM2 ���PWM�źų�ʼ����ֻҪ�����������
 *         TIM2���ĸ�ͨ���ͻ���PWM�ź����
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void TIM2_PWM_Init(void)
{
	MOTOR_GPIO_Config();
	TIM2_GPIO_Config();
	TIM2_Mode_Config();	
}

/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
