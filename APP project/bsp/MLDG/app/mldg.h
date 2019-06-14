/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          mldg.h
*\Description   
*\Note          
*\Log           2019.02.15    Ver 1.0    baron
*               创建文件。
***************************************************************************************************/
#ifndef _MLDG_H_
#define _MLDG_H_
#include "app_board.h"
#include "public_type.h"

extern u8 mode;

void mldg_init(GpioType* mldg);
void Mldg_Mode1(u8 time);
void Mldg_Mode2(u8 time);
void Mldg_Mode3(u8 time);
void Mldg_ModeOff(u8 time);
#endif



