/***************************************************************************************************
*                    (c) Copyright 2006-2016  Syncretize technologies co.,ltd.
*										All Rights Reserved
*	
*\File 			param.h
*\Description   
*\Note          
*\Log           2016.05.09    Ver 1.0   Job
*               �����ļ���
***************************************************************************************************/
#ifndef _PARAM_H
#define _PARAM_H

#include "public_type.h"
#include "common_lib.h"
#define VARI3101_GNSS_ID

#define DEV_APP_FIRST_POWER_FLAG 0x4321  //APP��IAP�ı�־λ����Ϊ��ͬ��

//����IP
extern u8 RT_LWIP_IPADDR0;
extern u8 RT_LWIP_IPADDR1;
extern u8 RT_LWIP_IPADDR2;
extern u8 RT_LWIP_IPADDR3;
//netmask
extern u8 RT_LWIP_MSKADDR0;	
extern u8 RT_LWIP_MSKADDR1;	
extern u8 RT_LWIP_MSKADDR2;	
extern u8 RT_LWIP_MSKADDR3;	
//gateway
extern u8 RT_LWIP_GWADDR0;
extern u8 RT_LWIP_GWADDR1;	
extern u8 RT_LWIP_GWADDR2;	
extern u8 RT_LWIP_GWADDR3;
//DNS
extern u8 RT_LWIP_DNSADDR0;
extern u8 RT_LWIP_DNSADDR1;
extern u8 RT_LWIP_DNSADDR2;
extern u8 RT_LWIP_DNSADDR3;

//ͬ����˸Ƶ��
enum
{
    Blink_Freq_0,//����
    Blink_Freq_30,
    Blink_Freq_60,
    Blink_Freq_120,
};

//ռ�ձ� ��˸ռ�ձ�
enum
{
    Duty_1_2 = 2,//1/2
    Duty_1_3,//1/3
    Duty_1_4,//1/4
};

#pragma pack(1)
typedef struct dev_param_type_st
{
    u16 Power_First_Flag;//��һ���ϵ��־ 	
	u16 mswork_mode;//���ӹ���ģʽ ��0��1
	u16 version;//�汾
	u8 analog_dial_switch;//ģ�Ⲧ�뿪�� ��δʹ�ò��뿪�ص�pcbʹ��Ĭ��0 ����1
    u16 GPS_Period;//GPSͬ������	
    u16 RTC_Period;//rtcͬ������	
	u8 gps_select;//0 GI200 F9P��ʹ�ã�1��ʹ��GI200;2��ʹ��F9P��3��ʹ��GPS
	u8 GI200_freq;//GI200Ƶ��  0--1hz  1--5hz
	u8 GI200_BDGLONS;//GI200 BD GLONSѡ�� 0 GLONS 1 BD(GI200)
	u8 GI200_AXES;//GI200��װ����ϵ 1--8
}dev_param_type;
#pragma pack()

//ʵʱ������������
#pragma pack(1)
typedef struct realtime_param_type_st
{
	u16	voltage;//��ѹ
	u16 current;//����
	u8 heartbeat_count;//���������
	u8 can_debug_flag;//can������Ϣ���� 0������ 1����
	u8 GI200_flag;//δ��λ0���Ѷ�λ1
	u8 F9P_flag;//δ��λ0���Ѷ�λ1
	u8 CAN_flag;//canͨ�ű�־λ 0�쳣 1����
	u8 flash_flag;//spi flash ��־λ  0��д�쳣 1��д����
	TIME InRTC;//����ʱ��
	double latitude;//γ��
	double longitude;//����
	u8 satellites;//�ɼ�������	
	u16 Mileage;//��ʻ���  ddd.mm ����λ����������9999����
	u16 speed;//�ٶ� 0~9999 ��λ�� ����10��
	u16 altitude;//���� ��
	u16 azimuth;//��λ�� 0~3600 ����10��
	u8 mileage_clc;//�������  0��̲���0 1�����0
	
}realtime_param_type;
#pragma pack()

extern dev_param_type dev_param;//�豸����
extern realtime_param_type realtime_param;//ʵʱ������������

void stm32_param_save(void);
void stm32_param_default(void);
void stm32_param_load(void);

#endif /*_PARAM_H*/
