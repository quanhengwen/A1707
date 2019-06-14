/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          adc.h
*\Description   
*\Note          
*\Log           2019.02.12    Ver 1.0    baron
*               创建文件。
***************************************************************************************************/
#ifndef __ADC_H
#define	__ADC_H
#include "stm32f10x.h"

#define ADC1_BUF_SIZE       2
#define TOTAL_CONVERT_CH_NUM  2

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)

extern vu16 ADC_Buf[2];
extern float CHA_V_DET_Value;
extern u16 PWRIN_DET_Value;

void ADC1_Init(void);
void adc_chapwr_thread_entry(void* parameter);
#endif /* __ADC_H */

