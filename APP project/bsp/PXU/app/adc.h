#ifndef __ADC_H
#define	__ADC_H

#include "stm32f10x.h"
#define  N   20      //ÿͨ����20�� 
#define  M  3      //Ϊ3��ͨ��   
extern vu16  AD_Value[N][M];   //�������ADCת�������Ҳ��DMA��Ŀ���ַ 
extern vu16  After_filter[M];    //���������ƽ��ֵ֮��Ľ��
void filter(void);
void ADC1_Init(void);

#endif /* __ADC_H */

