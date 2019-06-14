/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          exti.h
*\Description   
*\Note          
*\Log           2019.04.17    Ver 1.0    baron
*               创建文件。
***************************************************************************************************/
#ifndef _EXTI_H_
#define _EXTI_H_
#include "public_type.h"


typedef struct Control_Set
{
	u32 count;
	u8  out_control_flag;
}Control_Set;

extern Control_Set out_control;
/* Private function prototypes -----------------------------------------------*/
void EXTI2_Config(void);
void outcontrol_thread_entry(void* parameter);













#endif
