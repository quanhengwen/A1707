
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          gps_gi200.c
*\Description   
*\Note          
*\Log           2019.04.01    Ver 1.0    Jerry
*               创建文件。
***************************************************************************************************/
#include "gps_gi200.h"


//惯性导航使能
static uint8_t ins_enable[] = "log gpins";
static uint8_t ins_shutdown[] = "unlog gpins";

//ATT协议使能
static uint8_t att_enable[] = "log gpatt";
static uint8_t att_shutdown[] = "unlog gpatt";

//输出频率设置
static uint8_t freq_high[] = "log ghigh";//实现5HZ输出
static uint8_t freq_low[] = "unlog ghigh";//实现1HZ输出

//GSV协议使能
static uint8_t gsv_enable[] = "log gpgsv";//打开GPGSV,GPGSA
static uint8_t gsv_shutdown[] = "unlog gpgsv";//关闭GPGSV,GPGSA

//BD/GLONS选择
static uint8_t BD_enable[] = "log gpgbd";//选择GPS+BD
static uint8_t GLONS_enable[] = "unlog gpgbd";//选择GPS+Glonass

//输出GIRMC
static uint8_t rmc_enable[] = "log girmc";//打开GIRMC
static uint8_t rmc_shutdown[] = "unlog girmc";//关闭GIRMC

//设置安装方式
static uint8_t install_fix[] = "log alock";//固定安装
static uint8_t install_auto[] = "log aatuo";//自适应安装

//里程清零
static uint8_t mileage_clc[] = "log cloli";//清0

//坐标系设置
static uint8_t axes_one[] = "log zuob1";//选择坐标系1
static uint8_t axes_two[] = "log zuob2";//选择坐标系2
static uint8_t axes_three[] = "log zuob3";//选择坐标系3
static uint8_t axes_four[] = "log zuob4";//选择坐标系4
static uint8_t axes_five[] = "log zuob5";//选择坐标系5
static uint8_t axes_six[] = "log zuob6";//选择坐标系6
static uint8_t axes_seven[] = "log zuob7";//选择坐标系7
static uint8_t axes_eight[] = "log zuob8";//选择坐标系8

//波特率设置
static uint8_t baudrate_9600[] = "log g9600";//选择9600
static uint8_t baudrate_115200[] = "log g1152";//选择115200

gi200_param_type gi200_param = { 1,1,0,0,0,1,1,1,1,1 };


//gi200参数初始化
/***************************************************************************************************
*\Function      gi200_param_init
*\Description   
*\Parameter     x  
*\Return        void  
*\Note          
*\Log           2019.04.02    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void gi200_param_init(gi200_param_type *x)
{
	//惯性导航使能 0关闭 其他使能
	if ((*x).ins_flag==0)//关闭惯导
	{
		stm32_uart_send(&GI200UartTTL, ins_shutdown, countof(ins_shutdown));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, ins_enable, countof(ins_enable));
	}
	rt_thread_delay(2);
	//ATT协议使能 0关闭 其他使能
	if ((*x).att_flag == 0)//关闭att
	{
		stm32_uart_send(&GI200UartTTL, att_shutdown, countof(att_shutdown));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, att_enable, countof(att_enable));
	}
	rt_thread_delay(2);
	//输出频率设置  0--1hz  1--5hz
	if ((*x).freq_flag == 0)//1hz
	{
		stm32_uart_send(&GI200UartTTL, freq_low, countof(freq_low));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, freq_high, countof(freq_high));
	}
	rt_thread_delay(2);
	//GSV协议使能 0关闭 其他使能
	if ((*x).gsv_flag == 0)//关闭
	{
		stm32_uart_send(&GI200UartTTL, gsv_shutdown, countof(gsv_shutdown));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, gsv_enable, countof(gsv_enable));
	}
	rt_thread_delay(50);

	//BD/GLONS选择 0 GLONS 1 BD
	if ((*x).bd_flag == 0)//GLONS
	{
		stm32_uart_send(&GI200UartTTL, GLONS_enable, countof(GLONS_enable));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, BD_enable, countof(BD_enable));
	}
	rt_thread_delay(50);
	//输出GIRMC   0关闭 其他使能
	if ((*x).rmc_flag == 0)//
	{
		stm32_uart_send(&GI200UartTTL, rmc_shutdown, countof(rmc_shutdown));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, rmc_enable, countof(rmc_enable));
	}
	rt_thread_delay(2);
	//设置安装方式 0自适应安装 其他固定安装
	if ((*x).install_flag == 0)//
	{
		stm32_uart_send(&GI200UartTTL, install_auto, countof(install_auto));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, install_fix, countof(install_fix));
	}
	rt_thread_delay(2);
	//里程清零 0里程不处理 其他里程清0
	if ((*x).clc_flag == 0)//
	{
		
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, mileage_clc, countof(mileage_clc));
	}
	rt_thread_delay(2);
	//坐标系设置 1--8 其他不处理
	if ((*x).axes_flag == 1)//
	{
		stm32_uart_send(&GI200UartTTL, axes_one, countof(axes_one));
	}
	else
	if ((*x).axes_flag == 2)//
	{
		stm32_uart_send(&GI200UartTTL, axes_two, countof(axes_two));
	}
	else
	if ((*x).axes_flag == 3)//
	{
		stm32_uart_send(&GI200UartTTL, axes_three, countof(axes_three));
	}
	else
	if ((*x).axes_flag == 4)//
	{
		stm32_uart_send(&GI200UartTTL, axes_four, countof(axes_four));
	}
	else
	if ((*x).axes_flag == 5)//
	{
		stm32_uart_send(&GI200UartTTL, axes_five, countof(axes_five));
	}
	else
	if ((*x).axes_flag == 6)//
	{
		stm32_uart_send(&GI200UartTTL, axes_six, countof(axes_six));
	}
	else
	if ((*x).axes_flag == 7)//
	{
		stm32_uart_send(&GI200UartTTL, axes_seven, countof(axes_seven));
	}
	else
	if ((*x).axes_flag == 8)//
	{
		stm32_uart_send(&GI200UartTTL, axes_eight, countof(axes_eight));
	}
	else
	{
	}
	rt_thread_delay(2);
	//波特率设置 0 9600 其他115200
	if ((*x).baudrate_flag == 0)//
	{
		stm32_uart_send(&GI200UartTTL, baudrate_9600, countof(baudrate_9600));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, baudrate_115200, countof(baudrate_115200));
	}
	rt_thread_delay(2);

}

//GI200  设置坐标系
/***************************************************************************************************
*\Function      gi200_ucs
*\Description   
*\Parameter     axes_flag  
*\Return        void  
*\Note          
*\Log           2019.05.10    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void gi200_ucs(u8 axes_flag)
{
	stm32_uart_send(&GI200UartTTL, install_fix, countof(install_fix));
	rt_thread_delay(3);
	//坐标系设置 1--8 其他不处理
	if (axes_flag == 1)//
	{
		stm32_uart_send(&GI200UartTTL, axes_one, countof(axes_one));
	}
	else
	if (axes_flag == 2)//
	{
		stm32_uart_send(&GI200UartTTL, axes_two, countof(axes_two));
	}
	else
	if (axes_flag == 3)//
	{
		stm32_uart_send(&GI200UartTTL, axes_three, countof(axes_three));
	}
	else
	if (axes_flag == 4)//
	{
		stm32_uart_send(&GI200UartTTL, axes_four, countof(axes_four));
	}
	else
	if (axes_flag == 5)//
	{
		stm32_uart_send(&GI200UartTTL, axes_five, countof(axes_five));
	}
	else
	if (axes_flag == 6)//
	{
		stm32_uart_send(&GI200UartTTL, axes_six, countof(axes_six));
	}
	else
	if (axes_flag == 7)//
	{
		stm32_uart_send(&GI200UartTTL, axes_seven, countof(axes_seven));
	}
	else
	if (axes_flag == 8)//
	{
		stm32_uart_send(&GI200UartTTL, axes_eight, countof(axes_eight));
	}
	else
	{
	}
	rt_thread_delay(2);	
}

//GI200 里程计清零
/***************************************************************************************************
*\Function      gi200_clc
*\Description   
*\Parameter     clc_flag  
*\Return        void  
*\Note          
*\Log           2019.05.10    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void gi200_clc(u8 clc_flag)
{
	if (clc_flag == 0)//
	{
		
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, mileage_clc, countof(mileage_clc));
	}
	rt_thread_delay(2);	
}


//GI200 5hz
/***************************************************************************************************
*\Function      gi200_freq
*\Description   
*\Parameter     freq_flag  
*\Return        void  
*\Note          
*\Log           2019.05.10    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void gi200_freq(u8 freq_flag)
{
	if (freq_flag == 5)//5hz
	{
		stm32_uart_send(&GI200UartTTL, freq_high, countof(freq_low));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL,freq_low , countof(freq_high));
	}
	rt_thread_delay(2);	
}

//GI200 BD/GLONS
/***************************************************************************************************
*\Function      gi200_BD
*\Description   
*\Parameter     bd_flag  
*\Return        void  
*\Note          
*\Log           2019.05.10    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void gi200_BD(u8 bd_flag)
{
	//BD/GLONS选择 0 GLONS 1 BD
	if (bd_flag == 0)//GLONS
	{
		stm32_uart_send(&GI200UartTTL, GLONS_enable, countof(GLONS_enable));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, BD_enable, countof(BD_enable));
	}
	rt_thread_delay(50);
}

#ifdef RT_USING_FINSH
#include "finsh.h"

FINSH_FUNCTION_EXPORT(gi200_ucs, GI200 zuobiao 1 3)
FINSH_FUNCTION_EXPORT(gi200_clc, GI200 cloli 1)
FINSH_FUNCTION_EXPORT(gi200_freq, GI200 freq 5)
FINSH_FUNCTION_EXPORT(gi200_BD, GI200 BD 1)
#endif




/***************************************************************************************************
******************************************** END OF FILE *******************************************
***************************************************************************************************/


