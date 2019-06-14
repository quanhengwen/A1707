
#include "app_board.h"

void control_Init()
{
/*
	DC12EN1  PD2
	DC12EN2  PC12
	DC12EN3  PC11
	DC12EN4  PC10
	DC12EN5  PA15
	
	
	LED_SYS  PB13
	EXP1_EN  PB15
	EXP2_EN  PB14
*/
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	 GPIO_InitTypeDef GPIO_InitStructure;

	/*开启GPIO的外设时钟*/
	 RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE); 
													   
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 |GPIO_Pin_14 |GPIO_Pin_15;	
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 |GPIO_Pin_11 |GPIO_Pin_12;	
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	
		GPIO_Init(GPIOD, &GPIO_InitStructure);

     //全部 置低
   //  DC12EN1_L;DC12EN2_L; DC12EN3_L;DC12EN4_L;DC12EN5_L;LED_SYS_L;EXP1_EN_L;EXP2_EN_L;
		//全部 置高
		 DC12EN1_H;DC12EN2_H; DC12EN3_H;DC12EN4_H;DC12EN5_H;LED_SYS_H;EXP1_EN_H;EXP2_EN_H;
		 
		 
		if(dev_param.EXP1_EN) EXP1_EN_H ; else EXP1_EN_L;
		if(dev_param.EXP2_EN) EXP2_EN_H ; else EXP2_EN_L;

		if(dev_param.DC12EN1) DC12EN1_H ; else DC12EN1_L;
		if(dev_param.DC12EN2) DC12EN2_H ; else DC12EN2_L;
		if(dev_param.DC12EN3) DC12EN3_H ; else DC12EN3_L;
		if(dev_param.DC12EN4) DC12EN4_H ; else DC12EN4_L;
		if(dev_param.DC12EN5) DC12EN5_H ; else DC12EN5_L;
		
}







