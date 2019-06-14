
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          gps_gi200.h
*\Description   
*\Note          
*\Log           2019.04.01    Ver 1.0    Jerry
*               �����ļ���
***************************************************************************************************/
#ifndef _GPS_GI200_H
#define _GPS_GI200_H
#include "public_type.h"
#include "common_lib.h"
#include "drv_com_uart.h"
#include "app_board.h"

//#define countof(a)   (sizeof(a) / sizeof(*(a)))

//gi200 ����
#pragma pack(1)
typedef struct gi200_param_type_st
{
	u8 ins_flag;//���Ե���ʹ�� 0�ر� ����ʹ��
	u8 att_flag;//ATTЭ��ʹ�� 0�ر� ����ʹ��
	u8 freq_flag;//���Ƶ������  0--1hz  1--5hz
	u8 gsv_flag;//GSVЭ��ʹ�� 0�ر� ����ʹ��
	u8 bd_flag;//BD/GLONSѡ�� 0 GLONS 1 BD
	u8 rmc_flag;//���GIRMC   0�ر� ����ʹ��
	u8 install_flag;//���ð�װ��ʽ 0����Ӧ��װ �����̶���װ
	u8 clc_flag;//������� 0��̲����� ���������0
	u8 axes_flag;//����ϵ���� 1--8 ����������
	u8 baudrate_flag;//���������� 0 9600 ����115200

}gi200_param_type;
#pragma pack()

extern gi200_param_type gi200_param;

	
#endif /*_GPS_GI200_H*/



/***************************************************************************************************
******************************************** END OF FILE *******************************************
***************************************************************************************************/


