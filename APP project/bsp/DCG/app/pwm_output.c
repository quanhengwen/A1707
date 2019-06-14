/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  ：pwm_output.c
 * 描述    ：         
 * 实验平台：野火STM32开发板
 * 硬件连接：---------------------
 *          |  PA.06: (TIM2_CH1)  |
 *          |  PA.07: (TIM2_CH2)  |
 *      	|  PB.00: (TIM2_CH3)  | 
 *    		|  PB.01: (TIM2_CH4)  |
 *           ---------------------    			
 * 库版本  ：ST3.5.0
 * 作者    ：wildfire team 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/
#include "pwm_output.h"


u16  CCR_Va0=0; 


static void MOTOR_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 

  /* GPIOA 1 (IN2)方向 GPIOA 2 (IN3)刹车 */
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_1|GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;		    // 挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits (GPIOA,GPIO_Pin_1);    //方向  高 正转 低 反转
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);    //上电刹车  低刹车 
///	GPIO_SetBits (GPIOA,GPIO_Pin_2);    //方向 上电不刹车
}

/*
 * 函数名：TIM2_GPIO_Config
 * 描述  ：配置TIM2复用输出PWM时用到的I/O
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 
 
 
 */
static void TIM2_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

	/* TIM2 clock enable */
	//PCLK1经过2倍频后作为TIM2的时钟源等于72MHz
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 

  /*GPIOA Configuration: TIM2 channel 1 and 2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_0 ;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;		    // 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
//	GPIO_ResetBits(GPIOA,GPIO_Pin_0);  // PWM 
	
//	GPIO_SetBits (GPIOA,GPIO_Pin_0);  // PWM 
}

/*
 * 函数名：TIM2_Mode_Config
 * 描述  ：配置TIM2输出的PWM信号的模式，如周期、极性、占空比
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void TIM2_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* PWM信号电平跳变值 */
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
  TIM_TimeBaseStructure.TIM_Period = 999;       //当定时器从0计数到999，即为1000次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Prescaler = 0;	    //设置预分频：不预分频，即为72MHz
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//设置时钟分频系数：不分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = 0;	   //设置跳变值，当计数器计数到这个值时，电平发生跳变 设置成0 上电不跑
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平

  TIM_OC1Init(TIM2, &TIM_OCInitStructure);	 //使能通道1

  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);


  TIM_ARRPreloadConfig(TIM2, ENABLE);			 // 使能TIM2重载寄存器ARR

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);                   //使能定时器3	
}


void  PWM_out( u16 CCR_Val)
{
// TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
 /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  TIM_OCInitStructure.TIM_Pulse = CCR_Val;	   //设置跳变值，当计数器计数到这个值时，电平发生跳变
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平

  TIM_OC1Init(TIM2, &TIM_OCInitStructure);	 //使能通道1

  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);


  TIM_ARRPreloadConfig(TIM2, ENABLE);			 // 使能TIM2重载寄存器ARR

  /* TIM2 enable counter */
  TIM_Cmd(TIM2, ENABLE);                   //使能定时器3	
}





/*
 * 函数名：TIM2_PWM_Init
 * 描述  ：TIM2 输出PWM信号初始化，只要调用这个函数
 *         TIM2的四个通道就会有PWM信号输出
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void TIM2_PWM_Init(void)
{
	MOTOR_GPIO_Config();
	TIM2_GPIO_Config();
	TIM2_Mode_Config();	
}

/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
