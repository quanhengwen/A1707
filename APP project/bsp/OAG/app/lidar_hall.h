/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          lidar_hall.h
*\Description   
*\Note          
*\Log           2019.04.09    Ver 1.0    mcc
*               �����ļ���
***************************************************************************************************/
#ifndef _LIDAR_HALL_H_
#define _LIDAR_HALL_H_
#include "app_board.h"
#include "public_type.h"

#define LIDAR_RECV_BUFSIZE     1024

#define LIDAR_PROTOCOL_HEAD    0x59
#define LIDAR_CONFIG_PROTOCOL_HEAD      0x42
#define LIDAR_CONFIG_PROTOCOL_SECOND    0x57

#define LIDAR_PROTOCOL_MIN_LEN  9
#define LIDAR_PROTOCOL_MAX_LEN  100


#define LIDAR_REPORT_MIN_DIST   800   //<8m�����ϱ�ƽ̨ --- �����ʵ���޸Ĵ�ֵ
#define VTH                     2700  //mV����ֵ�����ڴ�ֵ˵�����ܵ�ײ�� --- �����ʵ���޸Ĵ�ֵ


#define EVENT_BROAD_MAX_NUM     3     //��⵽�ϰ�����ܵ����������ϴ�֪ͨƽ̨����
#define EVENT_BROAD_GAP_NUM     10    //��⵽�ϰ�����ܵ�����������ϱ�EVENT_BROAD_MAX_NUM�κ󣬼��EVENT_BROAD_GAP_NUM���ϱ�һ��


/***************��������****************/
#define LIDAR_CONFIG_CMD            0x02 
#define LIDAR_CONFIG_PERIOD_CMD     0x07 
#define LIDAR_CONFIG_MODE_CMD       0x14 
#define LIDAR_CONFIG_MODE_GEAR_CMD  0x11 


typedef struct OAG_protocol_type_st
{	
	u8      lidar_config_enable;   //���ģ������ģʽ-0x01 ���룻0x00 �˳�
	u16     lidar_config_period;   //���ģ�����������������
	u8      lidar_config_mode;     //���ģ����ģʽ����-0x00 �Զ�ģʽ��0x01 �̶�ģʽ
	u8      lidar_config_mode_gear;//���õ�λ��0x00 �����뵵λ��0x03 �о��뵵λ��0x07 Զ���뵵λ
	u16     lidar_broad_dist_th;   //�����ϱ���ֵ
	
	u8      lidar_config_ack;      //������Ϣ�ظ�
	u8      lidar_config_cmd_ack;  //������Ϣ�ظ�����
	
	u8      lidar_barrier_flag;  //�Ƿ����ϰ�����
	u16 	lidar_dist;     	 //������Ϣ
	u16 	lidar_strength; 	 //�ź�ǿ��
	u8      lidar_mode_gear;     //��൵λ-0x00 �����뵵��0x03 �о��뵵��0x07 Զ���뵵
	u8      lidar_failure_state; //����״̬
	
	u8      hall_hit_flag;       //�Ƿ��ܵ�ײ�����
	u16     hall_vol;            //�ɼ���ѹ
	u8      hall_failure_state;  //����״̬
}OAG_protocol_type;


enum{
	START_CONFIG  = 0x00,     //��������ģʽ
	CONFIG_PERIOD = 0x01,     //�����������
	CONFIG_MODE   = 0x02,     //���ò��ģʽ
	CONFIG_GEAR   = 0x03,	  //���ò�൵λ
	STOP_CONFIG   = 0x04,     //�˳�����ģʽ
};


extern OAG_protocol_type  OAG_data;


void Lidar_Config_Enable_Cmd(void);
void Lidar_Config_Disable_Cmd(void);
void Lidar_Config_Period_Cmd(void);
void Lidar_Config_Mode_Cmd(void);
void Lidar_Config_Mode_Gear_Cmd(void);

void lidar_Protocol_thread_entry(void* parameter);
void hall_handle_thread_entry(void* parameter);
void CAN_OAG_Package_Send(void);
void rt_thread_OAG_init(void);

#endif



