/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          canbus_middle.c
*\Description   
*\Note          
*\Log           2018.08.29    Ver 1.0    Job
*               �����ļ���
***************************************************************************************************/
#include "canbus_middle.h"
#include "protocol_anylise_frame.h"
#include "param.h"
#include "iap_if.h"
#include "flash_if.h"
#include "string.h"
#include "drv_watchdog.h"
#include "lidar_hall.h"


//��Ҫ���ݸ���pcb���±�д�ĺ�������
//extern u32 CAN_Paramset_Read_Construct(u8* buf , u32 len);
//extern u32 CAN_Automatic_Check_Construct(u8* buf, u32 len);
//extern void CAN_Paramset_Receive_Analyse(u8* buf);


/***************************************************************************************************
*\Function      canbus_middle_send
*\Description   CAN���ݷ���
*\Parameter     can
*\Parameter     ID
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note          
*\Log           2018.08.30    Ver 1.0    Job               
				����������
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

    //ÿ�η���8�ֽ�
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
            rt_thread_delay(1);
        }
    }

    //���Ͷ�����ֽ�
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
				����������
***************************************************************************************************/
void canbus_middle_thread_entry(void* p)
{
    CanRxMsg rx;
    u32 delay = 0;
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
            case 0x500:
                AddDataToBuf(&CAN_BUF500, rx.Data, rx.DLC);
//			{
//				u32  i = 0;
//				rt_kprintf("LEN:%d\n", rx.DLC);
//				for(i = 0; i< rx.DLC;i++)
//				{
//					rt_kprintf("%02X ", rx.Data[i]);
//				}
//				rt_kprintf("\n");
//			}
                break;
			
            default:
                rt_kprintf("StdID[0x%03X] is no support!\n", rx.StdId);
                break;
            }
        }
        else
        {
            delay = 0;
            rt_thread_delay(1);
        } 
    }
}
/***************************************************************************************************
*\Function      CAN_Heartbeat_Package_Send
*\Description   //Ĭ����0x0101ƽ̨���ư巢��������
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.03.14    Ver 1.0    �ӹ���
*               ����������
***************************************************************************************************/
void CAN_Heartbeat_Package_Send(void)
{
	u32 data_len = 8;
	u8 buf[8];
	u32 i = 0;
	
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID
	buf[i++] = realtime_param.heartbeat_count;//���
	buf[i++] = dev_param.version& 0xFF;//app�汾��
	buf[i++] = 0x00;
	buf[i++] = 0x00;
	buf[i++] = (u8)realtime_param.current;//������
	buf[i++] = (u8)(realtime_param.current >> 8);//������
	canbus_middle_send(&CAN, 0x600, buf, data_len);
}
/***************************************************************************************************
*\Function      CAN_Paramset_Read_Construct
*\Description   �豸�����Ķ�ȡ
*\Parameter     buf
*\Parameter     id_buf  Ŀ��id
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.04.22    Ver 1.0    mcc
*               ����������
***************************************************************************************************/
u32 CAN_Paramset_Read_Construct(u8* buf , u32 len)//��Ҫÿ�˸����Լ�����Ŀ���
{
	u32 data_len = 36;
	u32 i = 0;

	if (len < data_len)
	{
		return 0;
	}

	buf[i++] = CAN_COMM_PROTOCOL_HEAD;
	buf[i++] = (u8)(data_len);
	buf[i++] = (u8)((data_len) >> 8);
	buf[i++] = FUC_PARAMSET_READ;
	buf[i++] = (u8)iap_param.remoteID;//remoteID
	buf[i++] = (u8)(iap_param.remoteID >> 8);//remoteID
	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID

	buf[i++] = (u8)iap_param.ID;//ID
	buf[i++] = (u8)(iap_param.ID >> 8);//ID
	buf[i++] = dev_param.mswork_mode;// ����ģʽ
	buf[i++] = realtime_param.can_debug_flag;// ������Ϣ
	buf[i++] = dev_param.analog_dial_switch;// ģ�Ⲧ�뿪��
	buf[i++] = (u8)realtime_param.voltage;// ��ѹ��
	buf[i++] = (u8)(realtime_param.voltage >> 8);// ��ѹ��
	buf[i++] = (u8)realtime_param.current;// ������
	buf[i++] = (u8)(realtime_param.current >> 8);// ������
	buf[i++] = (u8)Sys_Run_Time;// ����ʱ��0
	buf[i++] = (u8)(Sys_Run_Time >> 8);// ����ʱ��8
	buf[i++] = (u8)(Sys_Run_Time >> 16);// ����ʱ��16
	buf[i++] = (u8)(Sys_Run_Time >> 24);// ����ʱ��24
	buf[i++] = (u8)dev_param.version;//app�汾��;
	buf[i++] = (u8)(dev_param.version >> 8);// app�汾��
	buf[i++] = (u8)iap_param.swversion;// iap�汾��
	buf[i++] = (u8)(iap_param.swversion >> 8);// iap�汾��
	buf[i++] = iap_param.hwversion;// Ӳ���汾
	buf[i++] = (u8)OAG_data.lidar_broad_dist_th;//�����ϱ���ֵ��
	buf[i++] = (u8)(OAG_data.lidar_broad_dist_th>>8);//�����ϱ���ֵ��
	buf[i++] = OAG_data.lidar_config_enable;//���ò���״̬
	buf[i++] = OAG_data.lidar_config_mode;//���ò��ģʽ
	buf[i++] = OAG_data.lidar_config_mode_gear;//���ò�൵λ
	buf[i++] = (u8)OAG_data.lidar_config_period;//���ò��ģ����������������
	buf[i++] = (u8)(OAG_data.lidar_config_period>>8);
	buf[i++] = 0x00;
	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;

	return i;
}

/***************************************************************************************************
*\Function      CAN_AUTOMATIC_CHECK_ACK_Construct
*\Description   �Զ���ⷢ�ͻظ�
*\Parameter     buf
*\Parameter     id_buf  Ŀ��id
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.03.08    Ver 1.0    baron
*               ����������
***************************************************************************************************/
u32 CAN_Automatic_Check_Construct(u8* buf, u32 len)//���ݲ�ͬ���豸�������
{
	u32 data_len = 15;
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
	buf[i++] = OAG_data.lidar_failure_state;//������״̬
	buf[i++] = OAG_data.hall_failure_state;//����Ԫ������״̬
	buf[i++] = (u8)realtime_param.current;// ������
	buf[i++] = (u8)(realtime_param.current >> 8);// ������
	buf[i++] = 0x01;//canͨ������
	buf[i++] = Get_Sum8(buf + 1, data_len - 3);
	buf[i++] = CAN_COMM_PROTOCOL_TAIL;

	return i;
}


/***************************************************************************************************
*\Function      CAN_Paramset_Receive_Analyse
*\Description   ���ݽ���
*\Parameter     buf
*\Parameter     len
*\Return        u32
*\Note
*\Log           2019.03.08    Ver 1.0    baron
*               ����������
***************************************************************************************************/
void CAN_Paramset_Receive_Analyse(u8* buf)//��Ҫ�����Լ���Э���������
{
	iap_param.ID=((buf[9] << 8) | buf[8]);//ID
	dev_param.mswork_mode=buf[10];// ����ģʽ
	realtime_param.can_debug_flag=buf[11];// ������Ϣ
	dev_param.analog_dial_switch=buf[12];// ģ�Ⲧ�뿪��
	
	OAG_data.lidar_broad_dist_th = (buf[27]<<8) | buf[26];//�����ϱ���ֵ����
	OAG_data.lidar_config_enable = buf[28];  //����ʹ��
	OAG_data.lidar_config_mode = buf[29]; //���ò��ģʽ
	OAG_data.lidar_config_mode_gear = buf[30];   //��൵λ����
	OAG_data.lidar_config_period = (buf[32]<<8) | buf[31]; //���������������
}
