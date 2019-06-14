/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          canbus_middle.c
*\Description   
*\Note          
*\Log           2018.08.29    Ver 1.0    Job
*               创建文件。
***************************************************************************************************/
#include "canbus_middle.h"
#include "protocol_anylise_frame.h"
#include "param.h"
#include "iap_if.h"
#include "flash_if.h"
#include "string.h"
#include "pwm_output.h"

#include "threadinit.h"

/***************************************************************************************************
*\Function      canbus_middle_send
*\Description   CAN数据发送
*\Parameter     can
*\Parameter     ID
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note          
*\Log           2018.08.30    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
u32 canbus_middle_send(CANBus_type* can, u32 stdID, u8* buf, u32 len)
{
    CanTxMsg tx;
    u32 i = 0;
    u32 num = 0;
    u32 len_left = 0;

    if (!can || len == 0 || !buf)
    {
        return 0;
    }

    num = len / 8;
    len_left = len % 8;

    tx.StdId = stdID;
    tx.IDE = CAN_Id_Standard;
    tx.RTR = CAN_RTR_Data;
		
    //每次发送8字节
    for (i = 0; i < num; i++)
    {
        tx.DLC = 8;
        memcpy(tx.Data, buf + i * 8, 8);
        if (!stm32_CAN_send(can, &tx))
        {
            return i * 8;
        }
        if (i % 10 == 0)
        {
         //   rt_thread_delay(1);
        }
    }

    //发送多余的字节
    if (len_left)
    {
        tx.DLC = len_left;
        memcpy(tx.Data, buf + num * 8, len_left);
        if (!stm32_CAN_send(can, &tx))
        {
            return num * 8;
        }
    }

    return len;
}

/***************************************************************************************************
*\Function      canbus_middle_thread_entry
*\Description   
*\Parameter     p
*\Return        void
*\Note          
*\Log           2018.08.30    Ver 1.0    Job               
				创建函数。
***************************************************************************************************/
void canbus_middle_thread_entry(void* p)
{	  u32  i = 0;
		CanRxMsg rx;
		u32 delay = 0;
	  static u16 time_err=0;
		can_ota_init();
		rt_thread_delay(200);
		stm32_CAN_clear(&CAN);

    while (1)
    {
        if (stm32_CAN_recv(&CAN, &rx))
        {
            delay++;
            if (delay % 10 == 0)
            {
                rt_thread_delay(1);
                delay = 0;
							  
            }

            if (rx.IDE != CAN_Id_Standard || rx.RTR != CAN_RTR_Data)
            {
                continue;
            }
            switch (rx.StdId)
            {
							case 0x100:  // 电机速速设置  //信息读取
							{
								
								//解析电机控制 协议 
								
								if(rx.Data[0]==(u8)iap_param.ID&&rx.Data[1]==(iap_param.ID >> 8)) //ID判断 是发给自己的
								{
									time_err=0;  //清零
									if(rx.Data[2]==0x01) //前进    速度是 0~35
									{
										if( Motor_state==1||Motor_state==0)
										{
											Motor_cotorl=1;
											run_motor; run_zheng ; 	
											//if(rx.Data[3]>20) rx.Data[3]=20;
											CCR_Va0=(u16) (rx.Data[3]/35.0*999.0);
											PWM_out(CCR_Va0);
											Motor_state=1;
											motor_infor.Motor_direction=0;
										}
								  }
									
								if(rx.Data[2]==0x02) //后退
								{  
									if( Motor_state==2||Motor_state==0)
									{
										Motor_cotorl=2;	
										run_motor; run_fan ; 
										//if(rx.Data[3]>20) rx.Data[3]=20;
										CCR_Va0=(u16) (rx.Data[3]/35.0*999.0);
										PWM_out(CCR_Va0);
										Motor_state=2;
										motor_infor.Motor_direction=1;
									}
								}

								if(rx.Data[2]==0x00) //缓慢停车
								{	
										Motor_cotorl=0;	
										rt_mb_send(do_motor_sem,Stop_low);  // 发送停车信号 
								}
								
								if(rx.Data[2]==0x03) //紧急刹车
								{	
										Motor_cotorl=3;	
										rt_mb_send(do_motor_sem,Stop_fast);  // 发送停车信号 
								}
									
								}
								
								rt_kprintf("LEN:%d\n", rx.DLC);
								for(i = 0; i<rx.DLC;i++)
								{
									rt_kprintf("%02X ", rx.Data[i]);
								}
								rt_kprintf("\n");
								
								rx.Data[0]=0;

							}break;
            case 0x500:
								{
                AddDataToBuf(&CAN_BUF500, rx.Data, rx.DLC);
								rt_kprintf("LEN:%d\n", rx.DLC);
								for(i = 0; i<rx.DLC;i++)
								{
									rt_kprintf("%02X ", rx.Data[i]);
								}
								rt_kprintf("\n");
						  	}
                break;
			  case 0x702:
  //        AddDataToBuf(&CAN_BUF500, rx.Data, rx.DLC);
							{
//								 if(rx.Data[0]==0xFE&&rx.Data[3]==0x01) //前进
//								 {
//										if( Motor_state==1||Motor_state==0)
//										{
//										 Motor_cotorl=1;
//										 run_motor; run_zheng ; 	
//										 if(rx.Data[3]>20) rx.Data[3]=20;
//										 CCR_Va0=(u16) (rx.Data[3]/40.0*999.0);
//										 PWM_out(CCR_Va0);
//										 Motor_state=1;
//										}
//								 }
//								 if(rx.Data[0]==0xFE&&rx.Data[3]==0x02) //后退
//								 {  
//										if( Motor_state==2||Motor_state==0)
//										{
//											Motor_cotorl=2;	
//											run_motor; run_fan ; 
//											if(rx.Data[3]>20) rx.Data[3]=20;
//											 CCR_Va0=(u16) (rx.Data[3]/40.0*999.0);
//											 PWM_out(CCR_Va0);
//											Motor_state=2;
//										}
//								 }
//								
//								 if(rx.Data[0]==0xFE&&rx.Data[3]==0x00) //停止
//								 {	
//										Motor_cotorl=0;	
//										rt_mb_send(do_motor_sem,Stop_low);  // 发送停车信号 
//								 }
							}
          break;
						
			
            default:
						{
               // rt_kprintf("StdID[0x%03X] is no support!\n", rx.StdId);
//								rt_kprintf("LEN:%d\n", rx.DLC);
//								for(i = 0; i<rx.DLC;i++)
//								{
//									rt_kprintf("%02X ", rx.Data[i]);
//								}
//								rt_kprintf("\n");
							} break;
            }
        }
        else
        {
            delay = 0;
            rt_thread_delay(1);
						time_err++;
						if(time_err>400) //5S多 没有 收到油门信号
						{
							    time_err=0;
								  //缓慢停车
							    if(Motor_state>0)
									{
										Motor_cotorl=0;	
										//rt_mb_send(do_motor_sem,Stop_low);  // 发送停车信号 
									}
						}
        } 
    }
}

//心跳包 
void CAN_Heartbeat_Package_Send(void)
{
	u32 data_len = 8;
	u8 buf[8];
	u32 i = 0;
	
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID
	buf[i++] = realtime_param.heartbeat_count;//序号
	buf[i++] = dev_param.version& 0xFF;//app版本低
	buf[i++] = 0;//保留
	buf[i++] = 0;//保留
	buf[i++] = (u8)realtime_param.current;       //电流低
	buf[i++] = (u8)(realtime_param.current >> 8);//电流高
	canbus_middle_send(&CAN, 0x600, buf, data_len);
}



/***************************************************************************************************
*\Function      CAN_AUTOMATIC_CHECK_ACK_Construct
*\Description   自动检测发送回复
*\Parameter     buf
*\Parameter     id_buf  目标id
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.03.08    Ver 1.0    baron
*               创建函数。
***************************************************************************************************/
u32 CAN_Automatic_Check_Construct(u8* buf, u32 len)//根据不同的设备进行组包
{
	u32 data_len = 24;
	u32 i = 0;

	if (len < data_len)
	{
		return 0;
	}

	buf[i++] = CAN_COMM_PROTOCOL_HEAD;
	buf[i++] = (u8)(data_len);
	buf[i++] = (u8)((data_len) >> 8);
	buf[i++] = FUC_DEVICE_SEARCH;
	buf[i++] = (u8)iap_param.remoteID;//remoteID
	buf[i++] = (u8)(iap_param.remoteID >> 8);//remoteID
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID

	buf[i++] =  motor_infor.Motor_error;     //故障状态  
	buf[i++] = (u8)realtime_param.current;    
	buf[i++] = (u8)(realtime_param.current >> 8);//电流高
	buf[i++] =  1; //CAN通信 标志
	buf[i++] =  Get_Sum8(buf + 1, data_len - 3);
	buf[i++] =  CAN_COMM_PROTOCOL_TAIL;
	
	return i;
}


