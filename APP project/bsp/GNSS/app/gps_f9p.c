
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          gps_f9p.c
*\Description   
*\Note          
*\Log           2019.05.10    Ver 1.0    Jerry
*               创建文件。
***************************************************************************************************/
#include "gps_f9p.h"

static uint8_t baudrate_38400[28] = {0xB5,0x62 ,0x06 ,0x00 ,0x14 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0xD0 ,0x08 ,0x00 ,0x00 ,0x00 ,0x96 ,0x00 ,0x00 ,0x23 ,0x00 ,0x23 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xCF ,0x30};//38400
static uint8_t baudrate_115200[28] = {0xB5,0x62 ,0x06 ,0x00 ,0x14 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0xD0 ,0x08 ,0x00 ,0x00 ,0x00 ,0xC2 ,0x01 ,0x00 ,0x02 ,0x00 ,0x02 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xBA ,0x50};//115200
static uint8_t f9psave[21] = {0xB5 ,0x62 ,0x06 ,0x09 ,0x0D ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xFF ,0xFF ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x07 ,0x21 ,0xAF};//save
static uint8_t freq_5hz[14] = {0xB5 ,0x62 ,0x06 ,0x08 ,0x06 ,0x00 ,0xC8 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0xDE ,0x6A};//freq 5hz
static uint8_t freq_1hz[14] = {0xB5 ,0x62 ,0x06 ,0x08 ,0x06 ,0x00 ,0xE8 ,0x03 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x39};//freq 1hz



//F9P baudrate 1 115200
/***************************************************************************************************
*\Function      f9p_bt
*\Description   
*\Parameter     baudrate_flag  
*\Return        void  
*\Note          
*\Log           2019.05.10    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void f9p_bt(u8 baudrate_flag)
{
	//baudrate
	if (baudrate_flag == 0)//38400
	{
		stm32_uart_send(&F9PUartTTL, baudrate_38400, countof(baudrate_38400));
		rt_thread_delay(100);	
		stm32_uart_set_baudrate(&F9PUartTTL,38400);
		rt_thread_delay(100);
		stm32_uart_send(&F9PUartTTL, f9psave, countof(f9psave));
	}
	else
	{
		stm32_uart_send(&F9PUartTTL, baudrate_115200, countof(baudrate_115200));	
		rt_thread_delay(100);	
		stm32_uart_set_baudrate(&F9PUartTTL,115200);
		rt_thread_delay(100);
		stm32_uart_send(&F9PUartTTL, f9psave, countof(f9psave));
	}

}

/***************************************************************************************************
*\Function      f9p_freq
*\Description   
*\Parameter     freq_flag  
*\Return        void  
*\Note          
*\Log           2019.05.10    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void f9p_freq(u8 freq_flag)
{
	//freq
	if (freq_flag == 1)//1hz
	{
		stm32_uart_send(&F9PUartTTL, freq_1hz, countof(freq_1hz));
	}
	else
	{//5hz
		stm32_uart_send(&F9PUartTTL, freq_5hz, countof(freq_5hz));
	}
	rt_thread_delay(100);
	stm32_uart_send(&F9PUartTTL, f9psave, countof(f9psave));
}


#ifdef RT_USING_FINSH
#include "finsh.h"
FINSH_FUNCTION_EXPORT(f9p_bt, F9P baudrate 1)
FINSH_FUNCTION_EXPORT(f9p_freq, F9P freq 5)
#endif


/***************************************************************************************************
******************************************** END OF FILE *******************************************
***************************************************************************************************/


