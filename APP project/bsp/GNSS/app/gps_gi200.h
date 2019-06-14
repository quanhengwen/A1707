
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          gps_gi200.h
*\Description   
*\Note          
*\Log           2019.04.01    Ver 1.0    Jerry
*               创建文件。
***************************************************************************************************/
#ifndef _GPS_GI200_H
#define _GPS_GI200_H
#include "public_type.h"
#include "common_lib.h"
#include "drv_com_uart.h"
#include "app_board.h"

//#define countof(a)   (sizeof(a) / sizeof(*(a)))

//gi200 参数
#pragma pack(1)
typedef struct gi200_param_type_st
{
	u8 ins_flag;//惯性导航使能 0关闭 其他使能
	u8 att_flag;//ATT协议使能 0关闭 其他使能
	u8 freq_flag;//输出频率设置  0--1hz  1--5hz
	u8 gsv_flag;//GSV协议使能 0关闭 其他使能
	u8 bd_flag;//BD/GLONS选择 0 GLONS 1 BD
	u8 rmc_flag;//输出GIRMC   0关闭 其他使能
	u8 install_flag;//设置安装方式 0自适应安装 其他固定安装
	u8 clc_flag;//里程清零 0里程不处理 其他里程清0
	u8 axes_flag;//坐标系设置 1--8 其他不处理
	u8 baudrate_flag;//波特率设置 0 9600 其他115200

}gi200_param_type;
#pragma pack()

extern gi200_param_type gi200_param;

	
#endif /*_GPS_GI200_H*/



/***************************************************************************************************
******************************************** END OF FILE *******************************************
***************************************************************************************************/


