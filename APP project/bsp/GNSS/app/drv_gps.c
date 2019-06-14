
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          drv_gi200.c
*\Description   
*\Note          
*\Log           2019.03.28    Ver 1.0    Jerry
*               创建文件。
***************************************************************************************************/
#include "drv_gps.h"
#include "gps_analysis.h"
#include "app_board.h"
#include "protocol_anylise_frame.h"

//GI200
GpsSyncType GPS_GI200={0};
static comm_dev_type* UartCommDev_GI200;
static u32 GI200transport_send(u8* buf, u32 len)
{
    return stm32_uart_send(&GI200UartTTL, buf, len);
}

static u32 GI200transport_recv(u8* buf, u32 len, bool state)
{
	return stm32_uart_recv(&GI200UartTTL, buf, len, state);
}
static u32 GI200transport_del(u32 len)
{
	 return stm32_uart_del(&GI200UartTTL, len);
}
static bool GI200transport_clear(void)
{
	 return stm32_uart_clear(&GI200UartTTL);
}


//F9P
GpsSyncType GPS_F9P={0};
static comm_dev_type* UartCommDev_F9P;
static u32 F9Ptransport_send(u8* buf, u32 len)
{
    return stm32_uart_send(&F9PUartTTL, buf, len);
}

static u32 F9Ptransport_recv(u8* buf, u32 len, bool state)
{
	return stm32_uart_recv(&F9PUartTTL, buf, len, state);
}
static u32 F9Ptransport_del(u32 len)
{
	 return stm32_uart_del(&F9PUartTTL, len);
}
static bool F9Ptransport_clear(void)
{
	 return stm32_uart_clear(&F9PUartTTL);
}






/***************************************************************************************************
*\Function      GI200UART_OTA_Protocol_Anylise
*\Description   
*\Parameter     comm  
*\Parameter     buf  
*\Parameter     len  
*\Return        GPSdel_info_type  
*\Note          
*\Log           2019.04.02    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
static GPSdel_info_type GI200UART_OTA_Protocol_Anylise(comm_dev_type* comm, u8* buf, u32 len)
{
    GPSdel_info_type info = {GPSanylise_waiting, 0};
    u32 i = 0;
    u32 data_len = 0;
	s32 head = 0;//帧尾查找
	s32 tail = 0;
    DataNode result[5];
    DataNode gnss_info[40];
	u32 count = 0;
	u8 check = 0;
    u32 send_len = 0;
	
    /*若长度大于协议长度，则进行协议解析，否则等待*/
    if (len >= GNSS_PROTOCOL_MIN_LEN)
    {
        while (tail++ < len - 1)
        {
            if (buf[tail] == GNSS_PROTOCOL_TAIL1 && buf[tail + 1] == GNSS_PROTOCOL_TAIL2)
            {
                //找到帧尾 再向前查找帧头 防止之前有丢失帧尾的帧
                if (tail == 0)
                {
                    //出错删掉帧尾字节 
                    info.del_len = 2;
                    info.state = GPSanylise_reject;
                    return info;
                }
                else
                {
                    //查找帧头
                    head = tail - 1;
                    while (head >= 0)
                    {
                        if (buf[head] == GNSS_PROTOCOL_HEAD)
                        {
                            //计算帧长 不包含帧头帧尾
                            data_len = tail - head - 1;
                            break;
                        }
                        head--;
                    }
                    //如果至此未找到“帧头”或者帧长小于最小长度则删除
                    if (data_len == 0 || data_len < GNSS_PROTOCOL_MIN_LEN)
                    {
                         info.del_len = tail + 2;
                         info.state = GPSanylise_reject;
                         return info;
                    }
                    else
                    {
                        //运行到此处说明查找到帧头 跳出帧尾查找
                        break;
                    }
                }   
            }
        }
        //根据帧长 判断是否查找到 未找到返回等待
        if (!data_len)
        {
            return info;
        }   
        //预设删除值
        info.state = GPSanylise_reject;
        info.del_len = tail + 2;

        //替换0x0D 0x0A为0x00，偏于字符串处理
        buf[tail] = 0x00;
        buf[tail + 1] = 0x00;

        //至此 已经检测出了一帧数据 进行检验 数据起始为head + 1，长度为data_len
        //分离校验和 计算校验 
        count = StrSplit(buf + head + 1, '*', 0x00 ,result, countof(result));
        if (count != 2)
        { 
            return info;
        }
        //因为result[1]结束时的字符会是0x0D 0x0A 所以不用管结束符
        check = StrToUL(result[1].Data, NULL, 16);
        if (check != Get_ExclusiveOR(result[0].Data, result[0].Len))
        {
//            printf("GNSS EOR Err!\n");
            return info;
        }
        //至此校验合法
        count = StrSplit(result[0].Data, ',', 0x00, gnss_info, countof(gnss_info));
        if (count == 0)
        {
//            printf("GNSS data Err!\n");
            return info;
        }
        //DevHealth.GNSS_Count++;
		
        //根据消息类型解析对应数据
        if (StrNCmp(gnss_info[0].Data + 2, "GGA", 3) == 0)
        {
//			 rt_kprintf("GI  ");
            GPGGA_msg_anylise(gnss_info, count, &GPS_GI200);
//            rt_kprintf("GI  $GPGGA %d\n",count);
        }
        else if (StrNCmp(gnss_info[0].Data + 2, "ATT", 3) == 0)
        {
			GPATT_msg_anylise(gnss_info, count, &GPS_GI200);
//            rt_kprintf("GI  $GPATT %d\n",count);
        }
        else if (StrNCmp(gnss_info[0].Data + 2, "GSV", 3) == 0)
        {
//            GPGSV_msg_anylise(gnss_info, count, &GPS_GI200);
//            rt_kprintf("$GPGSV %d\n",count);
        }
        else if (StrNCmp(gnss_info[0].Data + 2, "RMC", 3) == 0)
        {		
//			rt_kprintf("GI \n");
            GPRMC_msg_anylise(gnss_info, count, &GPS_GI200);
			GPS_Data_Storage(&GPS_GI200);//GPS信息存储
//            rt_kprintf("GI  $GPRMC %d\n",count);
        }
        else 
        {
//            rt_kprintf("$%.*s no support!\n", result[0].Len, result[0].Data);
        }
						
				
		
        info.state = GPSanylise_accept; 
        return info;
    }
    else
    {
        return info;
    }	

    
}

/***************************************************************************************************
*\Function      F9PUART_OTA_Protocol_Anylise
*\Description   
*\Parameter     comm  
*\Parameter     buf  
*\Parameter     len  
*\Return        GPSdel_info_type  
*\Note          
*\Log           2019.04.02    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
static GPSdel_info_type F9PUART_OTA_Protocol_Anylise(comm_dev_type* comm, u8* buf, u32 len)
{
    GPSdel_info_type info = {GPSanylise_waiting, 0};
    u32 i = 0;
    u32 data_len = 0;
	s32 head = 0;//帧尾查找
	s32 tail = 0;
    DataNode result[5];
    DataNode gnss_info[40];
	u32 count = 0;
	u8 check = 0;
    u32 send_len = 0;
	
    /*若长度大于协议长度，则进行协议解析，否则等待*/
    if (len >= GNSS_PROTOCOL_MIN_LEN)
    {
        while (tail++ < len - 1)
        {
            if (buf[tail] == GNSS_PROTOCOL_TAIL1 && buf[tail + 1] == GNSS_PROTOCOL_TAIL2)
            {
                //找到帧尾 再向前查找帧头 防止之前有丢失帧尾的帧
                if (tail == 0)
                {
                    //出错删掉帧尾字节 
                    info.del_len = 2;
                    info.state = GPSanylise_reject;
                    return info;
                }
                else
                {
                    //查找帧头
                    head = tail - 1;
                    while (head >= 0)
                    {
                        if (buf[head] == GNSS_PROTOCOL_HEAD)
                        {
                            //计算帧长 不包含帧头帧尾
                            data_len = tail - head - 1;
                            break;
                        }
                        head--;
                    }
                    //如果至此未找到“帧头”或者帧长小于最小长度则删除
                    if (data_len == 0 || data_len < GNSS_PROTOCOL_MIN_LEN)
                    {
                         info.del_len = tail + 2;
                         info.state = GPSanylise_reject;
                         return info;
                    }
                    else
                    {
                        //运行到此处说明查找到帧头 跳出帧尾查找
                        break;
                    }
                }   
            }
        }
        //根据帧长 判断是否查找到 未找到返回等待
        if (!data_len)
        {
            return info;
        }   
        //预设删除值
        info.state = GPSanylise_reject;
        info.del_len = tail + 2;

        //替换0x0D 0x0A为0x00，偏于字符串处理
        buf[tail] = 0x00;
        buf[tail + 1] = 0x00;

        //至此 已经检测出了一帧数据 进行检验 数据起始为head + 1，长度为data_len
        //分离校验和 计算校验 
        count = StrSplit(buf + head + 1, '*', 0x00 ,result, countof(result));
        if (count != 2)
        { 
            return info;
        }
        //因为result[1]结束时的字符会是0x0D 0x0A 所以不用管结束符
        check = StrToUL(result[1].Data, NULL, 16);
        if (check != Get_ExclusiveOR(result[0].Data, result[0].Len))
        {
//            printf("GNSS EOR Err!\n");
            return info;
        }
        //至此校验合法
        count = StrSplit(result[0].Data, ',', 0x00, gnss_info, countof(gnss_info));
        if (count == 0)
        {
//            printf("GNSS data Err!\n");
            return info;
        }
        //DevHealth.GNSS_Count++;
		
        //根据消息类型解析对应数据
        if (StrNCmp(gnss_info[0].Data + 2, "GGA", 3) == 0)
        {
//			rt_kprintf("F9P ");
            GPGGA_msg_anylise(gnss_info, count, &GPS_F9P);
//            rt_kprintf("F9P $GPGGA %d\n",count);
        }
        else if (StrNCmp(gnss_info[0].Data + 2, "ATT", 3) == 0)
        {
//			GPATT_msg_anylise(gnss_info, count, &GPS_F9P);
//            rt_kprintf("F9P $GPATT %d\n",count);
        }
        else if (StrNCmp(gnss_info[0].Data + 2, "GSV", 3) == 0)
        {
//            GPGSV_msg_anylise(gnss_info, count, &GPS_F9P);
//            rt_kprintf("F9P $GPGSV %d\n",count);
        }
        else if (StrNCmp(gnss_info[0].Data + 2, "RMC", 3) == 0)
        {
//			 rt_kprintf("F9P \n");
            GPRMC_msg_anylise(gnss_info, count, &GPS_F9P);
			GPS_Data_Storage(&GPS_F9P);//GPS信息存储
//            rt_kprintf("F9P $GPRMC %d\n",count);
        }
        else 
        {
//            rt_kprintf("$%.*s no support!\n", result[0].Len, result[0].Data);
        }
						
				
		
        info.state = GPSanylise_accept; 
        return info;
    }
    else
    {
        return info;
    }	

    
}

//gi200 pps PA4
/***************************************************************************************************
*\Function      gi200_pps_config
*\Description   
*\Parameter       
*\Return        void  
*\Note          
*\Log           2019.04.02    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void gi200_pps_config(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	/* Enable GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/* Configure PA.00 pin as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Enable AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	/* Connect EXTI0 Line to PA.00 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);

	/* Configure EXTI0 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
  
}

/***************************************************************************************************
*\Function      EXTI4_IRQHandler
*\Description   
*\Parameter       
*\Return        void  
*\Note          
*\Log           2019.04.02    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void EXTI4_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line4) != RESET)
  {
//    printf("gi200 pps\n");

    /* Clear the  EXTI line 4 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line4);
  }
}

/***************************************************************************************************
*\Function      gps_gi200_init
*\Description   
*\Parameter       
*\Return        void  
*\Note          
*\Log           2019.04.02    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void gps_gi200_init(void)
{
	//GI200
	stm32_switch_init(&GI200GPS_PWCtl);
	stm32_switch_on(&GI200GPS_PWCtl);//低电平有效
	gi200_pps_config();
	stm32_uart_init(&GI200UartTTL);
    //串口解析函数
    UartCommDev_GI200 = comm_dev_create("UartGI200", 1024, GI200transport_recv, GI200transport_del, GI200transport_clear, RT_NULL, 1*RT_TICK_PER_SECOND);
    if (UartCommDev_GI200 == RT_NULL)
    {
        rt_kprintf("UartCommDev_GI200 creat fail\n!");
        return;
    }
    //添加具体协议
    protocol_anylise_add(UartCommDev_GI200, GI200UART_OTA_Protocol_Anylise);
    //启动协议分析 优先级12
    protocol_anylise_startup(UartCommDev_GI200, 21, 2048);
}

//f9p pps PC11
/***************************************************************************************************
*\Function      f9p_pps_config
*\Description   
*\Parameter       
*\Return        void  
*\Note          
*\Log           2019.04.02    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void f9p_pps_config(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	/* Enable GPIOA clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/* Configure PA.00 pin as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Enable AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	/* Connect EXTI0 Line to PA.00 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource6);

	/* Configure EXTI0 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
  
}

/***************************************************************************************************
*\Function      EXTI15_10_IRQHandler
*\Description   
*\Parameter       
*\Return        void  
*\Note          
*\Log           2019.04.02    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void EXTI9_5_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line6) != RESET)
  {
//    printf("F9P pps\n");

    /* Clear the  EXTI line 6 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line6);
  }
}

/***************************************************************************************************
*\Function      gps_f9p_init
*\Description   
*\Parameter       
*\Return        void  
*\Note          
*\Log           2019.04.02    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void gps_f9p_init(void)
{
	//F9P
	stm32_switch_init(&F9PGPS_PWCtl);
	stm32_switch_on(&F9PGPS_PWCtl);//低电平有效
	f9p_pps_config();
	stm32_uart_init(&F9PUartTTL);
    //串口解析函数
    UartCommDev_F9P = comm_dev_create("UartF9P", 2048, F9Ptransport_recv, F9Ptransport_del, F9Ptransport_clear, RT_NULL, 1*RT_TICK_PER_SECOND);
    if (UartCommDev_F9P == RT_NULL)
    {
        rt_kprintf("UartCommDev_F9P creat fail\n!");
        return;
    }
    //添加具体协议
    protocol_anylise_add(UartCommDev_F9P, F9PUART_OTA_Protocol_Anylise);
    //启动协议分析 优先级12
    protocol_anylise_startup(UartCommDev_F9P, 21, 2048);	
}
	


/***************************************************************************************************
*\Function      gps_init
*\Description   
*\Parameter       
*\Return        void  
*\Note          
*\Log           2019.04.02    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void gps_init(void)
{
	GPS_GI200.GPS_Master_Flag=0;
	gps_gi200_init();//GI200
	
	GPS_F9P.GPS_Master_Flag=1;
	gps_f9p_init();//F9P
}


void gps_enable(u8 enable_flag)
{
	if (enable_flag == 0)//disable
	{
		stm32_switch_off(&F9PGPS_PWCtl);//低电平有效
		stm32_switch_off(&GI200GPS_PWCtl);//低电平有效		
	}
	else//enable
	{
		stm32_switch_on(&F9PGPS_PWCtl);//低电平有效
		stm32_switch_on(&GI200GPS_PWCtl);//低电平有效
	}
	
}


#ifdef RT_USING_FINSH
#include "finsh.h"
FINSH_FUNCTION_EXPORT(gps_enable, 0 DEN 1 EN)
#endif



/***************************************************************************************************
******************************************** END OF FILE *******************************************
***************************************************************************************************/


