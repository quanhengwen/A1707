/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          lidar_hall.c
*\Description   激光测距和冲击检测
*\Note       	1.测距检测到障碍物小于规定距离，则立即上报平台,最多上报3次；
*               2.检测到障碍物且距离大于规定上报距离，则检测几次，判断数据是否呈下降趋势，若是则上报，上报次数最多3次。
*               3.冲击检测，当数据小于某一设定阈值，则上报平台，上报次数最多3次。
*
*\Problems      测距：若需分辨障碍物在设备前方或后方，则需获取设备运动状态
*               1.如何判断设备运动方向，从而确定哪两个传感器数据有效�
*               2.各模块如何确定是否上报数据�
*
*\Log           2019.04.09    Ver 1.0    mcc
*               创建文件。
***************************************************************************************************/
#include "lidar_hall.h"

#include "app_board.h"
#include "protocol_anylise_frame.h"
#include "string.h"

#include "can_ota_protocol.h"
#include "iap_if.h"
#include "param.h"
#include "usdl_buf.h"


OAG_protocol_type  OAG_data;//避障网关参数结构体

//Lidar解析协议使用
u8 LidarCommBuf[LIDAR_RECV_BUFSIZE];
u32 wait_ticks;//等待超时计时

static RingBufType liadr_dist_data;//存放障碍物距离数据缓存区



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
*\Description   OAG默认参数
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.22    Ver 1.0    mcc
*               创建函数。
***************************************************************************************************/
void OAG_param_default(void)
{
    memset(&OAG_data, 0, sizeof(OAG_data));
	
	OAG_data.hall_failure_state = 0x00;    //默认无故障
	OAG_data.hall_hit_flag = 0x00;         //默认无撞击
	
	OAG_data.lidar_failure_state = 0x00;   //默认无故障
	OAG_data.lidar_barrier_flag = 0x00;    //默认无障碍物
	
	OAG_data.lidar_config_enable = 0x00;   //默认不配置
	OAG_data.lidar_config_mode = 0x00;     //自动测距模式
	OAG_data.lidar_config_mode_gear = 0x07;//测距档位--固定测距模式下使用
	OAG_data.lidar_config_period = 0x000A; //10ms(100Hz)
	
	OAG_data.lidar_broad_dist_th = LIDAR_REPORT_MIN_DIST;//距离上报阈值设置
}


/******************测距模组参数配置***********************/
/***************************************************************************************************
*\Function      Lidar_Config_Enable_Cmd
*\Description   测距模组进入配置命令
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.19    Ver 1.0    mcc
*               创建函数。
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
*\Description   测距模组退出配置命令
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.19    Ver 1.0    mcc
*               创建函数。
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
*\Description   测距模组数据输出周期设置
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.19    Ver 1.0    mcc
*               创建函数。
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
*\Description   测距模组测距模式设置
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.19    Ver 1.0    mcc
*               创建函数。
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
*\Description   测距模组测距档位设置
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.19    Ver 1.0    mcc
*               创建函数。
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
*\Return        bool: TRUE-数据呈下降趋势；FALSE-数据呈上升趋势或平稳状态
*\Note          
*\Log           2019.04.10    Ver 1.0    mcc               
				创建函数。
***************************************************************************************************/
bool Lidar_Data_Decline(RingBufType* buf)
{
	u32 len;
	u16 compare_buf[3];
	
	len = GetLenFromBuf(buf);//获取数据长度
	
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
*\Description   数据解析及处理
*\Parameter     buf
*\Parameter     len
*\Return        del_info_type
*\Note          
*\Log           2019.04.10    Ver 1.0    mcc               
				创建函数。
***************************************************************************************************/
void Lidar_Data_Process(UartDevType* uart_dev, OAG_protocol_type *OAG)
{
	static u8 state = START_CONFIG;//初始化设备状态
	static u8 broad_num = 0; //检测到障碍物后上报次数
	
	if(OAG_data.lidar_config_enable) //配置设备参数
	{
		switch(state)
		{
			case START_CONFIG://进入配置模式
				Lidar_Config_Enable_Cmd();
				rt_thread_delay(1);
				if((OAG_data.lidar_config_ack==1) && (OAG_data.lidar_config_cmd_ack == LIDAR_CONFIG_CMD))
				{
					OAG_data.lidar_config_ack = 0;
					state = CONFIG_PERIOD;
				}
				break;
			case CONFIG_PERIOD://配置输出周期
				//发送配置数据周期输出间隔命令
				Lidar_Config_Period_Cmd();
				rt_thread_delay(1);
				if((OAG_data.lidar_config_ack==1) && (OAG_data.lidar_config_cmd_ack == LIDAR_CONFIG_PERIOD_CMD))
				{
					OAG_data.lidar_config_ack = 0;
					state = CONFIG_MODE;
				}
				break;
			case CONFIG_MODE://配置测距模式
				//发送配置测距模式命令
				Lidar_Config_Mode_Cmd();
				rt_thread_delay(1);
				if((OAG_data.lidar_config_ack==1) && (OAG_data.lidar_config_cmd_ack == LIDAR_CONFIG_MODE_CMD))
				{
					OAG_data.lidar_config_ack = 0;
					if(OAG_data.lidar_config_mode == 0x01)//仅在测距模式配置为固定模式时，配置档位字节才有效
					{
						state = CONFIG_GEAR;
					}
					else//若测距模式为自动模式，则直接进入退出模式
					{
						state = STOP_CONFIG;
					}
				}
				break;
			case CONFIG_GEAR://配置测距档位
				Lidar_Config_Mode_Gear_Cmd();
				rt_thread_delay(1);
				if((OAG_data.lidar_config_ack==1) && (OAG_data.lidar_config_cmd_ack == LIDAR_CONFIG_MODE_GEAR_CMD))
				{
					OAG_data.lidar_config_ack = 0;
					state = STOP_CONFIG;
				}
				break;
			case STOP_CONFIG://退出配置模式
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
	else //正常数据处理
	{
		if((OAG->lidar_strength>=20) && (OAG->lidar_strength<=2000))//数据可信区间
		{
			//存储有效距离信息
			AddByteToBuf(&liadr_dist_data,(u8)(OAG->lidar_dist)); //先存储低位，再存储高位
			AddByteToBuf(&liadr_dist_data,(u8)(OAG->lidar_dist>>8));
			
			//检测到障碍物
			if(OAG->lidar_dist<1200)//在检测距离>12m时，输出距离为1200
			{
				//lidar_data_buf[num] = OAG->lidar_dist;
				
				/*
				*检测到障碍物距离设备较近，立即上报
				*当检测到障碍物，且障碍物距离设备距离>LIDAR_REPORT_MIN_DIST时，需多次检测，判断障碍物距离是否呈下降趋势，若为下降趋势，则正在靠近障碍物，上报信息。
				*检测到障碍物后，此次上报次数限定为 EVENT_BROAD_MAX_NUM 次
				*/				
				//if((OAG->lidar_dist<LIDAR_REPORT_MIN_DIST) || ((num>2) && (Data_Decline(lidar_data_buf,num))))
				if((OAG->lidar_dist<OAG_data.lidar_broad_dist_th) && (Lidar_Data_Decline(&liadr_dist_data)))
				{
					//立即上报
					OAG->lidar_barrier_flag = 1;
					//num = 0;
					//组包，通知平台
					//上报条件：上报次数<上限;连续上报EVENT_BROAD_MAX_NUM次后，若情况未改善，则每隔几次上报一次
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
				//	//检测到障碍物，且距离未达到立即上报条件，累计检测到次数增加检测可靠性
				//	num++;
				//	if(num > lidar_data_buf_len)
				//	{
				//		num = 0;
				//	}
				//}
			}
			else//未检测到障碍物
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
				创建函数。
***************************************************************************************************/
extern void system_reset(void);
static del_info_type Uart_Lidar_Protocol_Anylise(UartDevType* uart_dev, u8* buf, u32 len)
{
    del_info_type info = {anylise_waiting, 0};
    u32 i = 0;
	u8 data;
	
    if (len >= LIDAR_PROTOCOL_MIN_LEN)
    {
        /*开始解析*/
        i = 0;
        while (1)
        {
            /*同步头、尾不正确或者校验和不正确*/
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
                /*查找数据包长度*/
                //data_len = buf[i + 2];
                //data_len = (data_len << 8) + buf[i + 1];
                //长度合法判断 目前长度允许8-1200
                if (((buf[i] == LIDAR_PROTOCOL_HEAD) && (buf[i + 1] != LIDAR_PROTOCOL_HEAD)) || ((buf[i] == LIDAR_CONFIG_PROTOCOL_HEAD) && (buf[i + 1] != LIDAR_CONFIG_PROTOCOL_SECOND)))//查找第二个数据是否符合协议包要求
                {
                    i++;
                    continue;
                }
                /*如果接收数据长度小于帧构成长度 继续等待*/
                if (len - i < LIDAR_PROTOCOL_MIN_LEN)
                {
                    /*i == 0表示正确的帧头前面没有错误数据 返回等待,否则删除错误的数据*/
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
                /*检测帧尾,帧尾符合规则 就认为这是一个完整的数据帧*/
                if (((buf[i] == LIDAR_PROTOCOL_HEAD) && (buf[i + 8] == Get_Sum8(buf + i, 8)))        \
					|| ((buf[i] == LIDAR_CONFIG_PROTOCOL_HEAD) && (buf[i + 1] == LIDAR_CONFIG_PROTOCOL_SECOND)))
                {
                    break;
                }
                else//帧尾校验不符合 删除一个字节 继续查找
                {
                    i += 1;
                    continue;
                }
            }
        }
        //验证通过，进行协议处理
        info.state = anylise_accept;//先预设为接收
		switch(buf[i])
		{
			case 0x59: //测距模组自动周期输出数据
				//距离
				data = buf[i+3];
				OAG_data.lidar_dist = (data<<8) | buf[i+2];
				if(OAG_data.lidar_dist > 0xFFF0) //供电突然出问题时，出现距离输出0xFFFD的情况
				{
					OAG_data.lidar_failure_state = 0x01;
				}
				else
				{
					OAG_data.lidar_failure_state = 0x00;
				}
				//信号强度
				data = buf[i+5];
				OAG_data.lidar_strength = (data<<8) | buf[i+4];
				//模式
				OAG_data.lidar_mode_gear = buf[i+6];
				info.del_len = 9 + i; 
				break;
			case 0x42: //测距模组配置模式下回复数据
				OAG_data.lidar_config_ack = buf[i+3];
				OAG_data.lidar_config_cmd_ack = buf[i+7];
				info.del_len = 8 + i; 
				break;
		}
        //数据解析
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
*               创建函数。
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
		
		//读取缓存区数据
		len = uart2_recv(LidarCommBuf, LIDAR_RECV_BUFSIZE, FALSE);
		if(len == 0)
		{
			wait_ticks = rt_tick_get();
			if(OAG_data.lidar_config_enable == 0x00)
			{
				OAG_data.lidar_failure_state = 0x01;//无数据输出，Lidar故障
			}
		}
		else
		{
			//协议解析
			info = Uart_Lidar_Protocol_Anylise(&Comm_Uart2, LidarCommBuf, len);
			
			if (info.state != anylise_waiting)
			{
				//删除
				uart2_del(info.del_len);
				wait_ticks = rt_tick_get();
			}
			else
			{
				time_diff = rt_tick_get() - wait_ticks;
				if (Abs(time_diff) > 100)//超过5S解析不成功，则清空缓冲区数据（或者保守一点，删除一个字节，但这样会解析比较慢）
				{
					uart2_clear();
					wait_ticks = rt_tick_get();
				}
			}
			memset(LidarCommBuf,0,LIDAR_RECV_BUFSIZE);//清空接收缓存
		}
		
				
	}

}


/***************************************************************************************************
*\Function      stm32_get_conv_vv
*\Description   电压转换
*\Parameter     value
*\Return        u32
*\Note          1、单位mV
              
*\Log           2017.06.27    Ver 1.0    Job               
				创建函数。
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
*               创建函数。
***************************************************************************************************/
static RingBufType hall_data;
void hall_handle_thread_entry(void* parameter)
{	
	u8 broad_num = 0; //检测到冲击后上报次数
	
	while(1)
	{
		OAG_data.hall_vol = (stm32_adc_get_value(&ADC_Hall,0));
		
		//存储ADC信息
		AddByteToBuf(&hall_data,(u8)(OAG_data.hall_vol)); //先存储低位，再存储高位
		AddByteToBuf(&hall_data,(u8)(OAG_data.hall_vol>>8));
		
		//数据处理
		if(OAG_data.hall_vol == 0)
		{
			OAG_data.hall_failure_state = 0x01;
		}
		else
		{
			OAG_data.hall_failure_state = 0x00;
			//设阈值，若达到阈值条件，立即上报
			if(OAG_data.hall_vol > VTH)
			{
				//立即上报
				OAG_data.hall_hit_flag = 1;
				//上报条件：上报次数<上限;连续上报EVENT_BROAD_MAX_NUM次后，若情况未改善，则每隔EVENT_BROAD_GAP_NUM次上报一次
				if((broad_num < EVENT_BROAD_MAX_NUM) || (broad_num % 5 == 0))
				{
					//组包，通知平台
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




/***************OAG CAN上传******************/

/***************************************************************************************************
*\Function      CAN_OAG_Package_Send
*\Description   //默认向0x0101平台控制板上传数据
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.03.14    Ver 1.0    庞国瑞
*               创建函数。
***************************************************************************************************/
void CAN_OAG_Package_Send(void)
{
	u32 data_len = 8;
	u8 buf[8];
	u32 i = 0;
	
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID
	buf[i++] = OAG_data.lidar_barrier_flag;//障碍物标志
	buf[i++] = (u8)OAG_data.lidar_dist;//距离低
	buf[i++] = (u8)(OAG_data.lidar_dist >> 8);//距离高
	buf[i++] = OAG_data.hall_hit_flag;//撞击标志
	buf[i++] = 0x00;
	buf[i++] = Get_Sum8(buf, data_len - 1);
	canbus_middle_send(&CAN, 0x500, buf, data_len);
}

//OAG主动上传，上传间隔100ms
void rt_thread_OAG_upload_entry(void* parameter)
{	
	rt_thread_delay(100);//设备启动1s后开启上传线程

	while (1)
	{
		CAN_OAG_Package_Send();
		//固定间隔主动上传
		rt_thread_delay(10);
	}
}

/***************************************************************************************************
*\Function      rt_thread_OAG_init
*\Description   
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.03.14    Ver 1.0    庞国瑞
*               创建函数。
***************************************************************************************************/
void rt_thread_OAG_init(void)
{
	rt_thread_t OAG_init_thread;  
	
	//OAG各参数初始化
	OAG_param_default();
	RxBufInit(&liadr_dist_data,100);
	RxBufInit(&hall_data,100);
	
	OAG_init_thread = rt_thread_create("lidar", rt_thread_OAG_upload_entry, RT_NULL, 512, 15, 5);
	if (OAG_init_thread != RT_NULL)
		rt_thread_startup(OAG_init_thread);
}




