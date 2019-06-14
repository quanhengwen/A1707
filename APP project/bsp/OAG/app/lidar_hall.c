/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          lidar_hall.c
*\Description   ������ͳ�����
*\Note       	1.����⵽�ϰ���С�ڹ涨���룬�������ϱ�ƽ̨,����ϱ�3�Σ�
*               2.��⵽�ϰ����Ҿ�����ڹ涨�ϱ����룬���⼸�Σ��ж������Ƿ���½����ƣ��������ϱ����ϱ��������3�Ρ�
*               3.�����⣬������С��ĳһ�趨��ֵ�����ϱ�ƽ̨���ϱ��������3�Ρ�
*
*\Problems      ��ࣺ����ֱ��ϰ������豸ǰ����󷽣������ȡ�豸�˶�״̬
*               1.����ж��豸�˶����򣬴Ӷ�ȷ��������������������Ч�
*               2.��ģ�����ȷ���Ƿ��ϱ����ݡ
*
*\Log           2019.04.09    Ver 1.0    mcc
*               �����ļ���
***************************************************************************************************/
#include "lidar_hall.h"

#include "app_board.h"
#include "protocol_anylise_frame.h"
#include "string.h"

#include "can_ota_protocol.h"
#include "iap_if.h"
#include "param.h"
#include "usdl_buf.h"


OAG_protocol_type  OAG_data;//�������ز����ṹ��

//Lidar����Э��ʹ��
u8 LidarCommBuf[LIDAR_RECV_BUFSIZE];
u32 wait_ticks;//�ȴ���ʱ��ʱ

static RingBufType liadr_dist_data;//����ϰ���������ݻ�����



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
*\Description   OAGĬ�ϲ���
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.22    Ver 1.0    mcc
*               ����������
***************************************************************************************************/
void OAG_param_default(void)
{
    memset(&OAG_data, 0, sizeof(OAG_data));
	
	OAG_data.hall_failure_state = 0x00;    //Ĭ���޹���
	OAG_data.hall_hit_flag = 0x00;         //Ĭ����ײ��
	
	OAG_data.lidar_failure_state = 0x00;   //Ĭ���޹���
	OAG_data.lidar_barrier_flag = 0x00;    //Ĭ�����ϰ���
	
	OAG_data.lidar_config_enable = 0x00;   //Ĭ�ϲ�����
	OAG_data.lidar_config_mode = 0x00;     //�Զ����ģʽ
	OAG_data.lidar_config_mode_gear = 0x07;//��൵λ--�̶����ģʽ��ʹ��
	OAG_data.lidar_config_period = 0x000A; //10ms(100Hz)
	
	OAG_data.lidar_broad_dist_th = LIDAR_REPORT_MIN_DIST;//�����ϱ���ֵ����
}


/******************���ģ���������***********************/
/***************************************************************************************************
*\Function      Lidar_Config_Enable_Cmd
*\Description   ���ģ�������������
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.19    Ver 1.0    mcc
*               ����������
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
*\Description   ���ģ���˳���������
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.19    Ver 1.0    mcc
*               ����������
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
*\Description   ���ģ�����������������
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.19    Ver 1.0    mcc
*               ����������
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
*\Description   ���ģ����ģʽ����
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.19    Ver 1.0    mcc
*               ����������
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
*\Description   ���ģ���൵λ����
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.19    Ver 1.0    mcc
*               ����������
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
*\Return        bool: TRUE-���ݳ��½����ƣ�FALSE-���ݳ��������ƻ�ƽ��״̬
*\Note          
*\Log           2019.04.10    Ver 1.0    mcc               
				����������
***************************************************************************************************/
bool Lidar_Data_Decline(RingBufType* buf)
{
	u32 len;
	u16 compare_buf[3];
	
	len = GetLenFromBuf(buf);//��ȡ���ݳ���
	
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
*\Description   ���ݽ���������
*\Parameter     buf
*\Parameter     len
*\Return        del_info_type
*\Note          
*\Log           2019.04.10    Ver 1.0    mcc               
				����������
***************************************************************************************************/
void Lidar_Data_Process(UartDevType* uart_dev, OAG_protocol_type *OAG)
{
	static u8 state = START_CONFIG;//��ʼ���豸״̬
	static u8 broad_num = 0; //��⵽�ϰ�����ϱ�����
	
	if(OAG_data.lidar_config_enable) //�����豸����
	{
		switch(state)
		{
			case START_CONFIG://��������ģʽ
				Lidar_Config_Enable_Cmd();
				rt_thread_delay(1);
				if((OAG_data.lidar_config_ack==1) && (OAG_data.lidar_config_cmd_ack == LIDAR_CONFIG_CMD))
				{
					OAG_data.lidar_config_ack = 0;
					state = CONFIG_PERIOD;
				}
				break;
			case CONFIG_PERIOD://�����������
				//��������������������������
				Lidar_Config_Period_Cmd();
				rt_thread_delay(1);
				if((OAG_data.lidar_config_ack==1) && (OAG_data.lidar_config_cmd_ack == LIDAR_CONFIG_PERIOD_CMD))
				{
					OAG_data.lidar_config_ack = 0;
					state = CONFIG_MODE;
				}
				break;
			case CONFIG_MODE://���ò��ģʽ
				//�������ò��ģʽ����
				Lidar_Config_Mode_Cmd();
				rt_thread_delay(1);
				if((OAG_data.lidar_config_ack==1) && (OAG_data.lidar_config_cmd_ack == LIDAR_CONFIG_MODE_CMD))
				{
					OAG_data.lidar_config_ack = 0;
					if(OAG_data.lidar_config_mode == 0x01)//���ڲ��ģʽ����Ϊ�̶�ģʽʱ�����õ�λ�ֽڲ���Ч
					{
						state = CONFIG_GEAR;
					}
					else//�����ģʽΪ�Զ�ģʽ����ֱ�ӽ����˳�ģʽ
					{
						state = STOP_CONFIG;
					}
				}
				break;
			case CONFIG_GEAR://���ò�൵λ
				Lidar_Config_Mode_Gear_Cmd();
				rt_thread_delay(1);
				if((OAG_data.lidar_config_ack==1) && (OAG_data.lidar_config_cmd_ack == LIDAR_CONFIG_MODE_GEAR_CMD))
				{
					OAG_data.lidar_config_ack = 0;
					state = STOP_CONFIG;
				}
				break;
			case STOP_CONFIG://�˳�����ģʽ
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
	else //�������ݴ���
	{
		if((OAG->lidar_strength>=20) && (OAG->lidar_strength<=2000))//���ݿ�������
		{
			//�洢��Ч������Ϣ
			AddByteToBuf(&liadr_dist_data,(u8)(OAG->lidar_dist)); //�ȴ洢��λ���ٴ洢��λ
			AddByteToBuf(&liadr_dist_data,(u8)(OAG->lidar_dist>>8));
			
			//��⵽�ϰ���
			if(OAG->lidar_dist<1200)//�ڼ�����>12mʱ���������Ϊ1200
			{
				//lidar_data_buf[num] = OAG->lidar_dist;
				
				/*
				*��⵽�ϰ�������豸�Ͻ��������ϱ�
				*����⵽�ϰ�����ϰ�������豸����>LIDAR_REPORT_MIN_DISTʱ�����μ�⣬�ж��ϰ�������Ƿ���½����ƣ���Ϊ�½����ƣ������ڿ����ϰ���ϱ���Ϣ��
				*��⵽�ϰ���󣬴˴��ϱ������޶�Ϊ EVENT_BROAD_MAX_NUM ��
				*/				
				//if((OAG->lidar_dist<LIDAR_REPORT_MIN_DIST) || ((num>2) && (Data_Decline(lidar_data_buf,num))))
				if((OAG->lidar_dist<OAG_data.lidar_broad_dist_th) && (Lidar_Data_Decline(&liadr_dist_data)))
				{
					//�����ϱ�
					OAG->lidar_barrier_flag = 1;
					//num = 0;
					//�����֪ͨƽ̨
					//�ϱ��������ϱ�����<����;�����ϱ�EVENT_BROAD_MAX_NUM�κ������δ���ƣ���ÿ�������ϱ�һ��
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
				//	//��⵽�ϰ���Ҿ���δ�ﵽ�����ϱ��������ۼƼ�⵽�������Ӽ��ɿ���
				//	num++;
				//	if(num > lidar_data_buf_len)
				//	{
				//		num = 0;
				//	}
				//}
			}
			else//δ��⵽�ϰ���
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
				����������
***************************************************************************************************/
extern void system_reset(void);
static del_info_type Uart_Lidar_Protocol_Anylise(UartDevType* uart_dev, u8* buf, u32 len)
{
    del_info_type info = {anylise_waiting, 0};
    u32 i = 0;
	u8 data;
	
    if (len >= LIDAR_PROTOCOL_MIN_LEN)
    {
        /*��ʼ����*/
        i = 0;
        while (1)
        {
            /*ͬ��ͷ��β����ȷ����У��Ͳ���ȷ*/
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
                /*�������ݰ�����*/
                //data_len = buf[i + 2];
                //data_len = (data_len << 8) + buf[i + 1];
                //���ȺϷ��ж� Ŀǰ��������8-1200
                if (((buf[i] == LIDAR_PROTOCOL_HEAD) && (buf[i + 1] != LIDAR_PROTOCOL_HEAD)) || ((buf[i] == LIDAR_CONFIG_PROTOCOL_HEAD) && (buf[i + 1] != LIDAR_CONFIG_PROTOCOL_SECOND)))//���ҵڶ��������Ƿ����Э���Ҫ��
                {
                    i++;
                    continue;
                }
                /*����������ݳ���С��֡���ɳ��� �����ȴ�*/
                if (len - i < LIDAR_PROTOCOL_MIN_LEN)
                {
                    /*i == 0��ʾ��ȷ��֡ͷǰ��û�д������� ���صȴ�,����ɾ�����������*/
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
                /*���֡β,֡β���Ϲ��� ����Ϊ����һ������������֡*/
                if (((buf[i] == LIDAR_PROTOCOL_HEAD) && (buf[i + 8] == Get_Sum8(buf + i, 8)))        \
					|| ((buf[i] == LIDAR_CONFIG_PROTOCOL_HEAD) && (buf[i + 1] == LIDAR_CONFIG_PROTOCOL_SECOND)))
                {
                    break;
                }
                else//֡βУ�鲻���� ɾ��һ���ֽ� ��������
                {
                    i += 1;
                    continue;
                }
            }
        }
        //��֤ͨ��������Э�鴦��
        info.state = anylise_accept;//��Ԥ��Ϊ����
		switch(buf[i])
		{
			case 0x59: //���ģ���Զ������������
				//����
				data = buf[i+3];
				OAG_data.lidar_dist = (data<<8) | buf[i+2];
				if(OAG_data.lidar_dist > 0xFFF0) //����ͻȻ������ʱ�����־������0xFFFD�����
				{
					OAG_data.lidar_failure_state = 0x01;
				}
				else
				{
					OAG_data.lidar_failure_state = 0x00;
				}
				//�ź�ǿ��
				data = buf[i+5];
				OAG_data.lidar_strength = (data<<8) | buf[i+4];
				//ģʽ
				OAG_data.lidar_mode_gear = buf[i+6];
				info.del_len = 9 + i; 
				break;
			case 0x42: //���ģ������ģʽ�»ظ�����
				OAG_data.lidar_config_ack = buf[i+3];
				OAG_data.lidar_config_cmd_ack = buf[i+7];
				info.del_len = 8 + i; 
				break;
		}
        //���ݽ���
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
*               ����������
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
		
		//��ȡ����������
		len = uart2_recv(LidarCommBuf, LIDAR_RECV_BUFSIZE, FALSE);
		if(len == 0)
		{
			wait_ticks = rt_tick_get();
			if(OAG_data.lidar_config_enable == 0x00)
			{
				OAG_data.lidar_failure_state = 0x01;//�����������Lidar����
			}
		}
		else
		{
			//Э�����
			info = Uart_Lidar_Protocol_Anylise(&Comm_Uart2, LidarCommBuf, len);
			
			if (info.state != anylise_waiting)
			{
				//ɾ��
				uart2_del(info.del_len);
				wait_ticks = rt_tick_get();
			}
			else
			{
				time_diff = rt_tick_get() - wait_ticks;
				if (Abs(time_diff) > 100)//����5S�������ɹ�������ջ��������ݣ����߱���һ�㣬ɾ��һ���ֽڣ�������������Ƚ�����
				{
					uart2_clear();
					wait_ticks = rt_tick_get();
				}
			}
			memset(LidarCommBuf,0,LIDAR_RECV_BUFSIZE);//��ս��ջ���
		}
		
				
	}

}


/***************************************************************************************************
*\Function      stm32_get_conv_vv
*\Description   ��ѹת��
*\Parameter     value
*\Return        u32
*\Note          1����λmV
              
*\Log           2017.06.27    Ver 1.0    Job               
				����������
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
*               ����������
***************************************************************************************************/
static RingBufType hall_data;
void hall_handle_thread_entry(void* parameter)
{	
	u8 broad_num = 0; //��⵽������ϱ�����
	
	while(1)
	{
		OAG_data.hall_vol = (stm32_adc_get_value(&ADC_Hall,0));
		
		//�洢ADC��Ϣ
		AddByteToBuf(&hall_data,(u8)(OAG_data.hall_vol)); //�ȴ洢��λ���ٴ洢��λ
		AddByteToBuf(&hall_data,(u8)(OAG_data.hall_vol>>8));
		
		//���ݴ���
		if(OAG_data.hall_vol == 0)
		{
			OAG_data.hall_failure_state = 0x01;
		}
		else
		{
			OAG_data.hall_failure_state = 0x00;
			//����ֵ�����ﵽ��ֵ�����������ϱ�
			if(OAG_data.hall_vol > VTH)
			{
				//�����ϱ�
				OAG_data.hall_hit_flag = 1;
				//�ϱ��������ϱ�����<����;�����ϱ�EVENT_BROAD_MAX_NUM�κ������δ���ƣ���ÿ��EVENT_BROAD_GAP_NUM���ϱ�һ��
				if((broad_num < EVENT_BROAD_MAX_NUM) || (broad_num % 5 == 0))
				{
					//�����֪ͨƽ̨
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




/***************OAG CAN�ϴ�******************/

/***************************************************************************************************
*\Function      CAN_OAG_Package_Send
*\Description   //Ĭ����0x0101ƽ̨���ư��ϴ�����
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.03.14    Ver 1.0    �ӹ���
*               ����������
***************************************************************************************************/
void CAN_OAG_Package_Send(void)
{
	u32 data_len = 8;
	u8 buf[8];
	u32 i = 0;
	
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID
	buf[i++] = OAG_data.lidar_barrier_flag;//�ϰ����־
	buf[i++] = (u8)OAG_data.lidar_dist;//�����
	buf[i++] = (u8)(OAG_data.lidar_dist >> 8);//�����
	buf[i++] = OAG_data.hall_hit_flag;//ײ����־
	buf[i++] = 0x00;
	buf[i++] = Get_Sum8(buf, data_len - 1);
	canbus_middle_send(&CAN, 0x500, buf, data_len);
}

//OAG�����ϴ����ϴ����100ms
void rt_thread_OAG_upload_entry(void* parameter)
{	
	rt_thread_delay(100);//�豸����1s�����ϴ��߳�

	while (1)
	{
		CAN_OAG_Package_Send();
		//�̶���������ϴ�
		rt_thread_delay(10);
	}
}

/***************************************************************************************************
*\Function      rt_thread_OAG_init
*\Description   
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.03.14    Ver 1.0    �ӹ���
*               ����������
***************************************************************************************************/
void rt_thread_OAG_init(void)
{
	rt_thread_t OAG_init_thread;  
	
	//OAG��������ʼ��
	OAG_param_default();
	RxBufInit(&liadr_dist_data,100);
	RxBufInit(&hall_data,100);
	
	OAG_init_thread = rt_thread_create("lidar", rt_thread_OAG_upload_entry, RT_NULL, 512, 15, 5);
	if (OAG_init_thread != RT_NULL)
		rt_thread_startup(OAG_init_thread);
}




