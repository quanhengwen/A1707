#ifndef __PWM_OUTPUT_H
#define	__PWM_OUTPUT_H

#include "stm32f10x.h"



#define run_zheng  GPIO_SetBits (GPIOA,GPIO_Pin_1);    //����  �� ��ת �� ��ת
#define run_fan    GPIO_ResetBits (GPIOA,GPIO_Pin_1);    //����  �� ��ת �� ��ת

#define Stop_motor    GPIO_ResetBits(GPIOA,GPIO_Pin_2);//����  �� ��ת �� ��ת
#define run_motor 	GPIO_SetBits (GPIOA,GPIO_Pin_2);    //���� �ϵ粻ɲ��

extern  u16  CCR_Va0; 
void TIM2_PWM_Init(void);

void  PWM_out( u16 CCR_Val);

#endif /* __PWM_OUTPUT_H */

