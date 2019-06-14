
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          gps_gi200.c
*\Description   
*\Note          
*\Log           2019.04.01    Ver 1.0    Jerry
*               �����ļ���
***************************************************************************************************/
#include "gps_gi200.h"


//���Ե���ʹ��
static uint8_t ins_enable[] = "log gpins";
static uint8_t ins_shutdown[] = "unlog gpins";

//ATTЭ��ʹ��
static uint8_t att_enable[] = "log gpatt";
static uint8_t att_shutdown[] = "unlog gpatt";

//���Ƶ������
static uint8_t freq_high[] = "log ghigh";//ʵ��5HZ���
static uint8_t freq_low[] = "unlog ghigh";//ʵ��1HZ���

//GSVЭ��ʹ��
static uint8_t gsv_enable[] = "log gpgsv";//��GPGSV,GPGSA
static uint8_t gsv_shutdown[] = "unlog gpgsv";//�ر�GPGSV,GPGSA

//BD/GLONSѡ��
static uint8_t BD_enable[] = "log gpgbd";//ѡ��GPS+BD
static uint8_t GLONS_enable[] = "unlog gpgbd";//ѡ��GPS+Glonass

//���GIRMC
static uint8_t rmc_enable[] = "log girmc";//��GIRMC
static uint8_t rmc_shutdown[] = "unlog girmc";//�ر�GIRMC

//���ð�װ��ʽ
static uint8_t install_fix[] = "log alock";//�̶���װ
static uint8_t install_auto[] = "log aatuo";//����Ӧ��װ

//�������
static uint8_t mileage_clc[] = "log cloli";//��0

//����ϵ����
static uint8_t axes_one[] = "log zuob1";//ѡ������ϵ1
static uint8_t axes_two[] = "log zuob2";//ѡ������ϵ2
static uint8_t axes_three[] = "log zuob3";//ѡ������ϵ3
static uint8_t axes_four[] = "log zuob4";//ѡ������ϵ4
static uint8_t axes_five[] = "log zuob5";//ѡ������ϵ5
static uint8_t axes_six[] = "log zuob6";//ѡ������ϵ6
static uint8_t axes_seven[] = "log zuob7";//ѡ������ϵ7
static uint8_t axes_eight[] = "log zuob8";//ѡ������ϵ8

//����������
static uint8_t baudrate_9600[] = "log g9600";//ѡ��9600
static uint8_t baudrate_115200[] = "log g1152";//ѡ��115200

gi200_param_type gi200_param = { 1,1,0,0,0,1,1,1,1,1 };


//gi200������ʼ��
/***************************************************************************************************
*\Function      gi200_param_init
*\Description   
*\Parameter     x  
*\Return        void  
*\Note          
*\Log           2019.04.02    Ver 1.0    Jerry
*               ����������
***************************************************************************************************/
void gi200_param_init(gi200_param_type *x)
{
	//���Ե���ʹ�� 0�ر� ����ʹ��
	if ((*x).ins_flag==0)//�رչߵ�
	{
		stm32_uart_send(&GI200UartTTL, ins_shutdown, countof(ins_shutdown));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, ins_enable, countof(ins_enable));
	}
	rt_thread_delay(2);
	//ATTЭ��ʹ�� 0�ر� ����ʹ��
	if ((*x).att_flag == 0)//�ر�att
	{
		stm32_uart_send(&GI200UartTTL, att_shutdown, countof(att_shutdown));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, att_enable, countof(att_enable));
	}
	rt_thread_delay(2);
	//���Ƶ������  0--1hz  1--5hz
	if ((*x).freq_flag == 0)//1hz
	{
		stm32_uart_send(&GI200UartTTL, freq_low, countof(freq_low));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, freq_high, countof(freq_high));
	}
	rt_thread_delay(2);
	//GSVЭ��ʹ�� 0�ر� ����ʹ��
	if ((*x).gsv_flag == 0)//�ر�
	{
		stm32_uart_send(&GI200UartTTL, gsv_shutdown, countof(gsv_shutdown));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, gsv_enable, countof(gsv_enable));
	}
	rt_thread_delay(50);

	//BD/GLONSѡ�� 0 GLONS 1 BD
	if ((*x).bd_flag == 0)//GLONS
	{
		stm32_uart_send(&GI200UartTTL, GLONS_enable, countof(GLONS_enable));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, BD_enable, countof(BD_enable));
	}
	rt_thread_delay(50);
	//���GIRMC   0�ر� ����ʹ��
	if ((*x).rmc_flag == 0)//
	{
		stm32_uart_send(&GI200UartTTL, rmc_shutdown, countof(rmc_shutdown));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, rmc_enable, countof(rmc_enable));
	}
	rt_thread_delay(2);
	//���ð�װ��ʽ 0����Ӧ��װ �����̶���װ
	if ((*x).install_flag == 0)//
	{
		stm32_uart_send(&GI200UartTTL, install_auto, countof(install_auto));
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, install_fix, countof(install_fix));
	}
	rt_thread_delay(2);
	//������� 0��̲����� ���������0
	if ((*x).clc_flag == 0)//
	{
		
	}
	else
	{
		stm32_uart_send(&GI200UartTTL, mileage_clc, countof(mileage_clc));
	}
	rt_thread_delay(2);
	//����ϵ���� 1--8 ����������
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
	//���������� 0 9600 ����115200
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

//GI200  ��������ϵ
/***************************************************************************************************
*\Function      gi200_ucs
*\Description   
*\Parameter     axes_flag  
*\Return        void  
*\Note          
*\Log           2019.05.10    Ver 1.0    Jerry
*               ����������
***************************************************************************************************/
void gi200_ucs(u8 axes_flag)
{
	stm32_uart_send(&GI200UartTTL, install_fix, countof(install_fix));
	rt_thread_delay(3);
	//����ϵ���� 1--8 ����������
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

//GI200 ��̼�����
/***************************************************************************************************
*\Function      gi200_clc
*\Description   
*\Parameter     clc_flag  
*\Return        void  
*\Note          
*\Log           2019.05.10    Ver 1.0    Jerry
*               ����������
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
*               ����������
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
*               ����������
***************************************************************************************************/
void gi200_BD(u8 bd_flag)
{
	//BD/GLONSѡ�� 0 GLONS 1 BD
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


