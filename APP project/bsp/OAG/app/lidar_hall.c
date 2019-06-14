/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          lidar_hall.c
*\Description   ¼¤¹â²â¾àºÍ³å»÷¼ì²â
*\Note       	1.²â¾à¼ì²âµ½ÕÏ°­ÎïĞ¡ÓÚ¹æ¶¨¾àÀë£¬ÔòÁ¢¼´ÉÏ±¨Æ½Ì¨,×î¶àÉÏ±¨3´Î£»
*               2.¼ì²âµ½ÕÏ°­ÎïÇÒ¾àÀë´óÓÚ¹æ¶¨ÉÏ±¨¾àÀë£¬Ôò¼ì²â¼¸´Î£¬ÅĞ¶ÏÊı¾İÊÇ·ñ³ÊÏÂ½µÇ÷ÊÆ£¬ÈôÊÇÔòÉÏ±¨£¬ÉÏ±¨´ÎÊı×î¶à3´Î¡£
*               3.³å»÷¼ì²â£¬µ±Êı¾İĞ¡ÓÚÄ³Ò»Éè¶¨ãĞÖµ£¬ÔòÉÏ±¨Æ½Ì¨£¬ÉÏ±¨´ÎÊı×î¶à3´Î¡£
*
*\Problems      ²â¾à£ºÈôĞè·Ö±æÕÏ°­ÎïÔÚÉè±¸Ç°·½»òºó·½£¬ÔòĞè»ñÈ¡Éè±¸ÔË¶¯×´Ì¬
*               1.ÈçºÎÅĞ¶ÏÉè±¸ÔË¶¯·½Ïò£¬´Ó¶øÈ·¶¨ÄÄÁ½¸ö´«¸ĞÆ÷Êı¾İÓĞĞ§£
*               2.¸÷Ä£¿éÈçºÎÈ·¶¨ÊÇ·ñÉÏ±¨Êı¾İ¡
*
*\Log           2019.04.09    Ver 1.0    mcc
*               ´´½¨ÎÄ¼ş¡£
***************************************************************************************************/
#include "lidar_hall.h"

#include "app_board.h"
#include "protocol_anylise_frame.h"
#include "string.h"

#include "can_ota_protocol.h"
#include "iap_if.h"
#include "param.h"
#include "usdl_buf.h"


OAG_protocol_type  OAG_data;//±ÜÕÏÍø¹Ø²ÎÊı½á¹¹Ìå

//Lidar½âÎöĞ­ÒéÊ¹ÓÃ
u8 LidarCommBuf[LIDAR_RECV_BUFSIZE];
u32 wait_ticks;//µÈ´ı³¬Ê±¼ÆÊ±

static RingBufType liadr_dist_data;//´æ·ÅÕÏ°­Îï¾àÀëÊı¾İ»º´æÇø



u32 uart2_send(u8* buf, u32 len)
{
    return stm32_uart_send(&Comm_Uart2, buf, len);
}

static u32 uart2_recv(u8* buf, u32 len, bool state)
{
	return stm32_uart_recv(&Comm_Uart2, buf, len, state);
}
static u32 uart2_del(u32 len)
{
	 return stm32_uart_del(&Comm_Uart2, len);
}
static bool uart2_clear(void)
{
	 return stm32_uart_clear(&Comm_Uart2);
}



/***************************************************************************************************
*\Function      OAG_param_default
*\Description   OAGÄ¬ÈÏ²ÎÊı
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.22    Ver 1.0    mcc
*               ´´½¨º¯Êı¡£
***************************************************************************************************/
void OAG_param_default(void)
{
    memset(&OAG_data, 0, sizeof(OAG_data));
	
	OAG_data.hall_failure_state = 0x00;    //Ä¬ÈÏÎŞ¹ÊÕÏ
	OAG_data.hall_hit_flag = 0x00;         //Ä¬ÈÏÎŞ×²»÷
	
	OAG_data.lidar_failure_state = 0x00;   //Ä¬ÈÏÎŞ¹ÊÕÏ
	OAG_data.lidar_barrier_flag = 0x00;    //Ä¬ÈÏÎŞÕÏ°­Îï
	
	OAG_data.lidar_config_enable = 0x00;   //Ä¬ÈÏ²»ÅäÖÃ
	OAG_data.lidar_config_mode = 0x00;     //×Ô¶¯²â¾àÄ£Ê½
	OAG_data.lidar_config_mode_gear = 0x07;//²â¾àµµÎ»--¹Ì¶¨²â¾àÄ£Ê½ÏÂÊ¹ÓÃ
	OAG_data.lidar_config_period = 0x000A; //10ms(100Hz)
	
	OAG_data.lidar_broad_dist_th = LIDAR_REPORT_MIN_DIST;//¾àÀëÉÏ±¨ãĞÖµÉèÖÃ
}


/******************²â¾àÄ£×é²ÎÊıÅäÖÃ***********************/
/***************************************************************************************************
*\Function      Lidar_Config_Enable_Cmd
*\Description   ²â¾àÄ£×é½øÈëÅäÖÃÃüÁî
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.19    Ver 1.0    mcc
*               ´´½¨º¯Êı¡£
***************************************************************************************************/
void Lidar_Config_Enable_Cmd(void)
{
	u32 data_len = 8;
	u8 buf[8];
	u32 i = 0;
	
	buf[i++] = 0x42;
	buf[i++] = 0x57;
	buf[i++] = 0x02;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x01;
	buf[i++] = LIDAR_CONFIG_CMD;
	stm32_uart_send(&Comm_Uart2, buf, data_len);
}


/***************************************************************************************************
*\Function      Lidar_Config_Disable_Cmd
*\Description   ²â¾àÄ£×éÍË³öÅäÖÃÃüÁî
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.19    Ver 1.0    mcc
*               ´´½¨º¯Êı¡£
***************************************************************************************************/
void Lidar_Config_Disable_Cmd(void)
{
	u32 data_len = 8;
	u8 buf[8];
	u32 i = 0;
	
	buf[i++] = 0x42;
	buf[i++] = 0x57;
	buf[i++] = 0x02;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = LIDAR_CONFIG_CMD;
	stm32_uart_send(&Comm_Uart2, buf, data_len);
}

/***************************************************************************************************
*\Function      Lidar_Config_Period_Cmd
*\Description   ²â¾àÄ£×éÊı¾İÊä³öÖÜÆÚÉèÖÃ
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.19    Ver 1.0    mcc
*               ´´½¨º¯Êı¡£
***************************************************************************************************/
void Lidar_Config_Period_Cmd(void)
{
	u32 data_len = 8;
	u8 buf[8];
	u32 i = 0;
	
	buf[i++] = 0x42;
	buf[i++] = 0x57;
	buf[i++] = 0x02;
	buf[i++] = 0x00;
	buf[i++] = (u8)OAG_data.lidar_config_period;
	buf[i++] = (u8)(OAG_data.lidar_config_period>>8);
	buf[i++] = 0x00;
	buf[i++] = LIDAR_CONFIG_PERIOD_CMD;
	stm32_uart_send(&Comm_Uart2, buf, data_len);
}


/***************************************************************************************************
*\Function      Lidar_Config_Mode_Cmd
*\Description   ²â¾àÄ£×é²â¾àÄ£Ê½ÉèÖÃ
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.19    Ver 1.0    mcc
*               ´´½¨º¯Êı¡£
***************************************************************************************************/
void Lidar_Config_Mode_Cmd(void)
{
	u32 data_len = 8;
	u8 buf[8];
	u32 i = 0;
	
	buf[i++] = 0x42;
	buf[i++] = 0x57;
	buf[i++] = 0x02;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = OAG_data.lidar_config_mode;
	buf[i++] = LIDAR_CONFIG_MODE_CMD;
	stm32_uart_send(&Comm_Uart2, buf, data_len);
}

/***************************************************************************************************
*\Function      Lidar_Config_Mode_Gear_Cmd
*\Description   ²â¾àÄ£×é²â¾àµµÎ»ÉèÖÃ
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.19    Ver 1.0    mcc
*               ´´½¨º¯Êı¡£
***************************************************************************************************/
void Lidar_Config_Mode_Gear_Cmd(void)
{
	u32 data_len = 8;
	u8 buf[8];
	u32 i = 0;
	
	buf[i++] = 0x42;
	buf[i++] = 0x57;
	buf[i++] = 0x02;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = OAG_data.lidar_config_mode_gear;
	buf[i++] = LIDAR_CONFIG_MODE_GEAR_CMD;
	stm32_uart_send(&Comm_Uart2, buf, data_len);
}




/***************************************************************************************************
*\Function      Lidar_Data_Decline
*\Description   
*\Parameter     buf
*\Parameter     len
*\Return        bool: TRUE-Êı¾İ³ÊÏÂ½µÇ÷ÊÆ£»FALSE-Êı¾İ³ÊÉÏÉıÇ÷ÊÆ»òÆ½ÎÈ×´Ì¬
*\Note          
*\Log           2019.04.10    Ver 1.0    mcc               
				´´½¨º¯Êı¡£
***************************************************************************************************/
bool Lidar_Data_Decline(RingBufType* buf)
{
	u32 len;
	u16 compare_buf[3];
	
	len = GetLenFromBuf(buf);//»ñÈ¡Êı¾İ³¤¶È
	
	if(len >= 6)
	{
		compare_buf[0] = (buf->RxBuf[(buf->Tail-4+buf->RxBufSize)%buf->RxBufSize]<<8) | buf->RxBuf[(buf->Tail-5+buf->RxBufSize)%buf->RxBufSize];
		compare_buf[1] = (buf->RxBuf[(buf->Tail-2+buf->RxBufSize)%buf->RxBufSize]<<8) | buf->RxBuf[(buf->Tail-3+buf->RxBufSize)%buf->RxBufSize];
		compare_buf[2] = (buf->RxBuf[buf->Tail]<<8) | buf->RxBuf[(buf->Tail-1+buf->RxBufSize)%buf->RxBufSize];
		if((compare_buf[2] - compare_buf[1]<0) && (compare_buf[1] - compare_buf[0]<0))
		{
			return TRUE;
		}
	}		
	return FALSE;
}

/***************************************************************************************************
*\Function      Lidar_Data_Process
*\Description   Êı¾İ½âÎö¼°´¦Àí
*\Parameter     buf
*\Parameter     len
*\Return        del_info_type
*\Note          
*\Log           2019.04.10    Ver 1.0    mcc               
				´´½¨º¯Êı¡£
***************************************************************************************************/
void Lidar_Data_Process(UartDevType* uart_dev, OAG_protocol_type *OAG)
{
	static u8 state = START_CONFIG;//³õÊ¼»¯Éè±¸×´Ì¬
	static u8 broad_num = 0; //¼ì²âµ½ÕÏ°­ÎïºóÉÏ±¨´ÎÊı
	
	if(OAG_data.lidar_config_enable) //ÅäÖÃÉè±¸²ÎÊı
	{
		switch(state)
		{
			case START_CONFIG://½øÈëÅäÖÃÄ£Ê½
				Lidar_Config_Enable_Cmd();
				rt_thread_delay(1);
				if((OAG_data.lidar_config_ack==1) && (OAG_data.lidar_config_cmd_ack == LIDAR_CONFIG_CMD))
				{
					OAG_data.lidar_config_ack = 0;
					state = CONFIG_PERIOD;
				}
				break;
			case CONFIG_PERIOD://ÅäÖÃÊä³öÖÜÆÚ
				//·¢ËÍÅäÖÃÊı¾İÖÜÆÚÊä³ö¼ä¸ôÃüÁî
				Lidar_Config_Period_Cmd();
				rt_thread_delay(1);
				if((OAG_data.lidar_config_ack==1) && (OAG_data.lidar_config_cmd_ack == LIDAR_CONFIG_PERIOD_CMD))
				{
					OAG_data.lidar_config_ack = 0;
					state = CONFIG_MODE;
				}
				break;
			case CONFIG_MODE://ÅäÖÃ²â¾àÄ£Ê½
				//·¢ËÍÅäÖÃ²â¾àÄ£Ê½ÃüÁî
				Lidar_Config_Mode_Cmd();
				rt_thread_delay(1);
				if((OAG_data.lidar_config_ack==1) && (OAG_data.lidar_config_cmd_ack == LIDAR_CONFIG_MODE_CMD))
				{
					OAG_data.lidar_config_ack = 0;
					if(OAG_data.lidar_config_mode == 0x01)//½öÔÚ²â¾àÄ£Ê½ÅäÖÃÎª¹Ì¶¨Ä£Ê½Ê±£¬ÅäÖÃµµÎ»×Ö½Ú²ÅÓĞĞ§
					{
						state = CONFIG_GEAR;
					}
					else//Èô²â¾àÄ£Ê½Îª×Ô¶¯Ä£Ê½£¬ÔòÖ±½Ó½øÈëÍË³öÄ£Ê½
					{
						state = STOP_CONFIG;
					}
				}
				break;
			case CONFIG_GEAR://ÅäÖÃ²â¾àµµÎ»
				Lidar_Config_Mode_Gear_Cmd();
				rt_thread_delay(1);
				if((OAG_data.lidar_config_ack==1) && (OAG_data.lidar_config_cmd_ack == LIDAR_CONFIG_MODE_GEAR_CMD))
				{
					OAG_data.lidar_config_ack = 0;
					state = STOP_CONFIG;
				}
				break;
			case STOP_CONFIG://ÍË³öÅäÖÃÄ£Ê½
				Lidar_Config_Disable_Cmd();
				rt_thread_delay(1);
				if((OAG_data.lidar_config_ack==1) && (OAG_data.lidar_config_cmd_ack == LIDAR_CONFIG_CMD))
				{
					OAG_data.lidar_config_ack = 0;
					OAG_data.lidar_config_enable = 0x00;
					state = START_CONFIG;
				}
				break;
			default:
				break;
		}
	}
	else //Õı³£Êı¾İ´¦Àí
	{
		if((OAG->lidar_strength>=20) && (OAG->lidar_strength<=2000))//Êı¾İ¿ÉĞÅÇø¼ä
		{
			//´æ´¢ÓĞĞ§¾àÀëĞÅÏ¢
			AddByteToBuf(&liadr_dist_data,(u8)(OAG->lidar_dist)); //ÏÈ´æ´¢µÍÎ»£¬ÔÙ´æ´¢¸ßÎ»
			AddByteToBuf(&liadr_dist_data,(u8)(OAG->lidar_dist>>8));
			
			//¼ì²âµ½ÕÏ°­Îï
			if(OAG->lidar_dist<1200)//ÔÚ¼ì²â¾àÀë>12mÊ±£¬Êä³ö¾àÀëÎª1200
			{
				//lidar_data_buf[num] = OAG->lidar_dist;
				
				/*
				*¼ì²âµ½ÕÏ°­Îï¾àÀëÉè±¸½Ï½ü£¬Á¢¼´ÉÏ±¨
				*µ±¼ì²âµ½ÕÏ°­Îï£¬ÇÒÕÏ°­Îï¾àÀëÉè±¸¾àÀë>LIDAR_REPORT_MIN_DISTÊ±£¬Ğè¶à´Î¼ì²â£¬ÅĞ¶ÏÕÏ°­Îï¾àÀëÊÇ·ñ³ÊÏÂ½µÇ÷ÊÆ£¬ÈôÎªÏÂ½µÇ÷ÊÆ£¬ÔòÕıÔÚ¿¿½üÕÏ°­Îï£¬ÉÏ±¨ĞÅÏ¢¡£
				*¼ì²âµ½ÕÏ°­Îïºó£¬´Ë´ÎÉÏ±¨´ÎÊıÏŞ¶¨Îª EVENT_BROAD_MAX_NUM ´Î
				*/				
				//if((OAG->lidar_dist<LIDAR_REPORT_MIN_DIST) || ((num>2) && (Data_Decline(lidar_data_buf,num))))
				if((OAG->lidar_dist<OAG_data.lidar_broad_dist_th) && (Lidar_Data_Decline(&liadr_dist_data)))
				{
					//Á¢¼´ÉÏ±¨
					OAG->lidar_barrier_flag = 1;
					//num = 0;
					//×é°ü£¬Í¨ÖªÆ½Ì¨
					//ÉÏ±¨Ìõ¼ş£ºÉÏ±¨´ÎÊı<ÉÏÏŞ;Á¬ĞøÉÏ±¨EVENT_BROAD_MAX_NUM´Îºó£¬ÈôÇé¿öÎ´¸ÄÉÆ£¬ÔòÃ¿¸ô¼¸´ÎÉÏ±¨Ò»´Î
					if((broad_num < EVENT_BROAD_MAX_NUM)                                         \
						|| ((OAG->lidar_dist>800)&&(OAG->lidar_dist<1200)&&(broad_num%10 == 0))  \
						|| ((OAG->lidar_dist>500)&&(OAG->lidar_dist<=800)&&(broad_num%5 == 0))   \
						|| ((OAG->lidar_dist>100)&&(OAG->lidar_dist<500)&&(broad_num%3 == 0))    \
						|| (OAG->lidar_dist <= 100))
					{
						CAN_OAG_Package_Send();
					}
					broad_num++;
					if(broad_num == 255)
					{
						broad_num = EVENT_BROAD_MAX_NUM;
					}
				}
				//else
				//{
				//	//¼ì²âµ½ÕÏ°­Îï£¬ÇÒ¾àÀëÎ´´ïµ½Á¢¼´ÉÏ±¨Ìõ¼ş£¬ÀÛ¼Æ¼ì²âµ½´ÎÊıÔö¼Ó¼ì²â¿É¿¿ĞÔ
				//	num++;
				//	if(num > lidar_data_buf_len)
				//	{
				//		num = 0;
				//	}
				//}
			}
			else//Î´¼ì²âµ½ÕÏ°­Îï
			{
				OAG->lidar_barrier_flag = 0;
				broad_num = 0;
				//num = 0;
			}
		}
	}

}


/***************************************************************************************************
*\Function      Uart_Lidar_Protocol_Anylise
*\Description   
*\Parameter     buf
*\Parameter     len
*\Return        del_info_type
*\Note          
*\Log           2019.04.10    Ver 1.0    mcc               
				´´½¨º¯Êı¡£
***************************************************************************************************/
extern void system_reset(void);
static del_info_type Uart_Lidar_Protocol_Anylise(UartDevType* uart_dev, u8* buf, u32 len)
{
    del_info_type info = {anylise_waiting, 0};
    u32 i = 0;
	u8 data;
	
    if (len >= LIDAR_PROTOCOL_MIN_LEN)
    {
        /*¿ªÊ¼½âÎö*/
        i = 0;
        while (1)
        {
            /*Í¬²½Í·¡¢Î²²»ÕıÈ·»òÕßĞ£ÑéºÍ²»ÕıÈ·*/
            if ((buf[i] !=  LIDAR_PROTOCOL_HEAD) && ((buf[i] !=  LIDAR_CONFIG_PROTOCOL_HEAD)))
            {
                for (i += 1; i < len; i++)
                {
                    if ((buf[i] == LIDAR_PROTOCOL_HEAD) || (buf[i] == LIDAR_CONFIG_PROTOCOL_HEAD))
                    {
                        break;
                    }
                }
                if (len - i >=  LIDAR_PROTOCOL_MIN_LEN)
                {
                    continue;
                }
                else
                {
                    info.state = anylise_reject;
                    info.del_len = i;
                    return info;
                }      
            }
            else
            {
                /*²éÕÒÊı¾İ°ü³¤¶È*/
                //data_len = buf[i + 2];
                //data_len = (data_len << 8) + buf[i + 1];
                //³¤¶ÈºÏ·¨ÅĞ¶Ï Ä¿Ç°³¤¶ÈÔÊĞí8-1200
                if (((buf[i] == LIDAR_PROTOCOL_HEAD) && (buf[i + 1] != LIDAR_PROTOCOL_HEAD)) || ((buf[i] == LIDAR_CONFIG_PROTOCOL_HEAD) && (buf[i + 1] != LIDAR_CONFIG_PROTOCOL_SECOND)))//²éÕÒµÚ¶ş¸öÊı¾İÊÇ·ñ·ûºÏĞ­Òé°üÒªÇó
                {
                    i++;
                    continue;
                }
                /*Èç¹û½ÓÊÕÊı¾İ³¤¶ÈĞ¡ÓÚÖ¡¹¹³É³¤¶È ¼ÌĞøµÈ´ı*/
                if (len - i < LIDAR_PROTOCOL_MIN_LEN)
                {
                    /*i == 0±íÊ¾ÕıÈ·µÄÖ¡Í·Ç°ÃæÃ»ÓĞ´íÎóÊı¾İ ·µ»ØµÈ´ı,·ñÔòÉ¾³ı´íÎóµÄÊı¾İ*/
                    if (i == 0)
                    {
                        info.state = anylise_waiting;
                        info.del_len = 0;
                    }
                    else
                    {
                        info.state = anylise_reject;
                        info.del_len = i;
                    }
                    return info;
                }
                /*¼ì²âÖ¡Î²,Ö¡Î²·ûºÏ¹æÔò ¾ÍÈÏÎªÕâÊÇÒ»¸öÍêÕûµÄÊı¾İÖ¡*/
                if (((buf[i] == LIDAR_PROTOCOL_HEAD) && (buf[i + 8] == Get_Sum8(buf + i, 8)))        \
					|| ((buf[i] == LIDAR_CONFIG_PROTOCOL_HEAD) && (buf[i + 1] == LIDAR_CONFIG_PROTOCOL_SECOND)))
                {
                    break;
                }
                else//Ö¡Î²Ğ£Ñé²»·ûºÏ É¾³ıÒ»¸ö×Ö½Ú ¼ÌĞø²éÕÒ
                {
                    i += 1;
                    continue;
                }
            }
        }
        //ÑéÖ¤Í¨¹ı£¬½øĞĞĞ­Òé´¦Àí
        info.state = anylise_accept;//ÏÈÔ¤ÉèÎª½ÓÊÕ
		switch(buf[i])
		{
			case 0x59: //²â¾àÄ£×é×Ô¶¯ÖÜÆÚÊä³öÊı¾İ
				//¾àÀë
				data = buf[i+3];
				OAG_data.lidar_dist = (data<<8) | buf[i+2];
				if(OAG_data.lidar_dist > 0xFFF0) //¹©µçÍ»È»³öÎÊÌâÊ±£¬³öÏÖ¾àÀëÊä³ö0xFFFDµÄÇé¿ö
				{
					OAG_data.lidar_failure_state = 0x01;
				}
				else
				{
					OAG_data.lidar_failure_state = 0x00;
				}
				//ĞÅºÅÇ¿¶È
				data = buf[i+5];
				OAG_data.lidar_strength = (data<<8) | buf[i+4];
				//Ä£Ê½
				OAG_data.lidar_mode_gear = buf[i+6];
				info.del_len = 9 + i; 
				break;
			case 0x42: //²â¾àÄ£×éÅäÖÃÄ£Ê½ÏÂ»Ø¸´Êı¾İ
				OAG_data.lidar_config_ack = buf[i+3];
				OAG_data.lidar_config_cmd_ack = buf[i+7];
				info.del_len = 8 + i; 
				break;
		}
        //Êı¾İ½âÎö
		Lidar_Data_Process(uart_dev, &OAG_data);
        return info;
    }
    else
    {
        return info;
    } 
}



/***************************************************************************************************
*\Function      lidar_thread_entry
*\Description   
*\Parameter     parameter
*\Return        void
*\Note          
*\Log           2019.04.10    Ver 1.0    mcc
*               ´´½¨º¯Êı¡£
***************************************************************************************************/
void lidar_Protocol_thread_entry(void* parameter)
{	
	del_info_type info = {anylise_waiting, 0};
	
	rt_thread_OAG_init();
	
	while(1)
	{
		u32 len = 0;
		int time_diff = 0;
		
		rt_thread_delay(1);
		
		//¶ÁÈ¡»º´æÇøÊı¾İ
		len = uart2_recv(LidarCommBuf, LIDAR_RECV_BUFSIZE, FALSE);
		if(len == 0)
		{
			wait_ticks = rt_tick_get();
			if(OAG_data.lidar_config_enable == 0x00)
			{
				OAG_data.lidar_failure_state = 0x01;//ÎŞÊı¾İÊä³ö£¬Lidar¹ÊÕÏ
			}
		}
		else
		{
			//Ğ­Òé½âÎö
			info = Uart_Lidar_Protocol_Anylise(&Comm_Uart2, LidarCommBuf, len);
			
			if (info.state != anylise_waiting)
			{
				//É¾³ı
				uart2_del(info.del_len);
				wait_ticks = rt_tick_get();
			}
			else
			{
				time_diff = rt_tick_get() - wait_ticks;
				if (Abs(time_diff) > 100)//³¬¹ı5S½âÎö²»³É¹¦£¬ÔòÇå¿Õ»º³åÇøÊı¾İ£¨»òÕß±£ÊØÒ»µã£¬É¾³ıÒ»¸ö×Ö½Ú£¬µ«ÕâÑù»á½âÎö±È½ÏÂı£©
				{
					uart2_clear();
					wait_ticks = rt_tick_get();
				}
			}
			memset(LidarCommBuf,0,LIDAR_RECV_BUFSIZE);//Çå¿Õ½ÓÊÕ»º´æ
		}
		
				
	}

}


/***************************************************************************************************
*\Function      stm32_get_conv_vv
*\Description   µçÑ¹×ª»»
*\Parameter     value
*\Return        u32
*\Note          1¡¢µ¥Î»mV
              
*\Log           2017.06.27    Ver 1.0    Job               
				´´½¨º¯Êı¡£
***************************************************************************************************/
u32 stm32_get_conv_volt(u16 value)
{
    double volt;
    
    volt = value * 3.3 / 4096 * 1000;

    return (u32)volt;
}

/***************************************************************************************************
*\Function      hall_thread_entry
*\Description   
*\Parameter     parameter
*\Return        void
*\Note          
*\Log           2019.04.10    Ver 1.0    mcc
*               ´´½¨º¯Êı¡£
***************************************************************************************************/
static RingBufType hall_data;
void hall_handle_thread_entry(void* parameter)
{	
	u8 broad_num = 0; //¼ì²âµ½³å»÷ºóÉÏ±¨´ÎÊı
	
	while(1)
	{
		OAG_data.hall_vol = (stm32_adc_get_value(&ADC_Hall,0));
		
		//´æ´¢ADCĞÅÏ¢
		AddByteToBuf(&hall_data,(u8)(OAG_data.hall_vol)); //ÏÈ´æ´¢µÍÎ»£¬ÔÙ´æ´¢¸ßÎ»
		AddByteToBuf(&hall_data,(u8)(OAG_data.hall_vol>>8));
		
		//Êı¾İ´¦Àí
		if(OAG_data.hall_vol == 0)
		{
			OAG_data.hall_failure_state = 0x01;
		}
		else
		{
			OAG_data.hall_failure_state = 0x00;
			//ÉèãĞÖµ£¬Èô´ïµ½ãĞÖµÌõ¼ş£¬Á¢¼´ÉÏ±¨
			if(OAG_data.hall_vol > VTH)
			{
				//Á¢¼´ÉÏ±¨
				OAG_data.hall_hit_flag = 1;
				//ÉÏ±¨Ìõ¼ş£ºÉÏ±¨´ÎÊı<ÉÏÏŞ;Á¬ĞøÉÏ±¨EVENT_BROAD_MAX_NUM´Îºó£¬ÈôÇé¿öÎ´¸ÄÉÆ£¬ÔòÃ¿¸ôEVENT_BROAD_GAP_NUM´ÎÉÏ±¨Ò»´Î
				if((broad_num < EVENT_BROAD_MAX_NUM) || (broad_num % 5 == 0))
				{
					//×é°ü£¬Í¨ÖªÆ½Ì¨
					CAN_OAG_Package_Send();
				}
				broad_num++;
				if(broad_num == 255)
				{
					broad_num = EVENT_BROAD_MAX_NUM;
				}
			}
			else
			{
				OAG_data.hall_hit_flag = 0;
				broad_num = 0;
			}
		}
		rt_thread_delay(1);		
	}
}




/***************OAG CANÉÏ´«******************/

/***************************************************************************************************
*\Function      CAN_OAG_Package_Send
*\Description   //Ä¬ÈÏÏò0x0101Æ½Ì¨¿ØÖÆ°åÉÏ´«Êı¾İ
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.03.14    Ver 1.0    ÅÓ¹úÈğ
*               ´´½¨º¯Êı¡£
***************************************************************************************************/
void CAN_OAG_Package_Send(void)
{
	u32 data_len = 8;
	u8 buf[8];
	u32 i = 0;
	
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID
	buf[i++] = OAG_data.lidar_barrier_flag;//ÕÏ°­Îï±êÖ¾
	buf[i++] = (u8)OAG_data.lidar_dist;//¾àÀëµÍ
	buf[i++] = (u8)(OAG_data.lidar_dist >> 8);//¾àÀë¸ß
	buf[i++] = OAG_data.hall_hit_flag;//×²»÷±êÖ¾
	buf[i++] = 0x00;
	buf[i++] = Get_Sum8(buf, data_len - 1);
	canbus_middle_send(&CAN, 0x500, buf, data_len);
}

//OAGÖ÷¶¯ÉÏ´«£¬ÉÏ´«¼ä¸ô100ms
void rt_thread_OAG_upload_entry(void* parameter)
{	
	rt_thread_delay(100);//Éè±¸Æô¶¯1sºó¿ªÆôÉÏ´«Ïß³Ì

	while (1)
	{
		CAN_OAG_Package_Send();
		//¹Ì¶¨¼ä¸ôÖ÷¶¯ÉÏ´«
		rt_thread_delay(10);
	}
}

/***************************************************************************************************
*\Function      rt_thread_OAG_init
*\Description   
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.03.14    Ver 1.0    ÅÓ¹úÈğ
*               ´´½¨º¯Êı¡£
***************************************************************************************************/
void rt_thread_OAG_init(void)
{
	rt_thread_t OAG_init_thread;  
	
	//OAG¸÷²ÎÊı³õÊ¼»¯
	OAG_param_default();
	RxBufInit(&liadr_dist_data,100);
	RxBufInit(&hall_data,100);
	
	OAG_init_thread = rt_thread_create("lidar", rt_thread_OAG_upload_entry, RT_NULL, 512, 15, 5);
	if (OAG_init_thread != RT_NULL)
		rt_thread_startup(OAG_init_thread);
}




