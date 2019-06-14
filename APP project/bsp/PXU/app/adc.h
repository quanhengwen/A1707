#ifndef __ADC_H
#define	__ADC_H

#include "stm32f10x.h"
#define  N   20      //每通道采20次 
#define  M  3      //为3个通道   
extern vu16  AD_Value[N][M];   //用来存放ADC转换结果，也是DMA的目标地址 
extern vu16  After_filter[M];    //用来存放求平均值之后的结果
void filter(void);
void ADC1_Init(void);

#endif /* __ADC_H */

