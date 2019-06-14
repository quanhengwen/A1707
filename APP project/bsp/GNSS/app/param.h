/***************************************************************************************************
*                    (c) Copyright 2006-2016  Syncretize technologies co.,ltd.
*										All Rights Reserved
*	
*\File 			param.h
*\Description   
*\Note          
*\Log           2016.05.09    Ver 1.0   Job
*               创建文件。
***************************************************************************************************/
#ifndef _PARAM_H
#define _PARAM_H

#include "public_type.h"
#include "common_lib.h"
#define VARI3101_GNSS_ID

#define DEV_APP_FIRST_POWER_FLAG 0x4321  //APP与IAP的标志位设置为不同的

//本机IP
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

//同步闪烁频率
enum
{
    Blink_Freq_0,//常亮
    Blink_Freq_30,
    Blink_Freq_60,
    Blink_Freq_120,
};

//占空比 闪烁占空比
enum
{
    Duty_1_2 = 2,//1/2
    Duty_1_3,//1/3
    Duty_1_4,//1/4
};

#pragma pack(1)
typedef struct dev_param_type_st
{
    u16 Power_First_Flag;//第一次上电标志 	
	u16 mswork_mode;//主从工作模式 主0从1
	u16 version;//版本
	u8 analog_dial_switch;//模拟拨码开关 在未使用拨码开关的pcb使用默认0 其他1
    u16 GPS_Period;//GPS同步周期	
    u16 RTC_Period;//rtc同步周期	
	u8 gps_select;//0 GI200 F9P都使用；1仅使用GI200;2仅使用F9P；3不使用GPS
	u8 GI200_freq;//GI200频率  0--1hz  1--5hz
	u8 GI200_BDGLONS;//GI200 BD GLONS选择 0 GLONS 1 BD(GI200)
	u8 GI200_AXES;//GI200安装坐标系 1--8
}dev_param_type;
#pragma pack()

//实时参数，不保存
#pragma pack(1)
typedef struct realtime_param_type_st
{
	u16	voltage;//电压
	u16 current;//电流
	u8 heartbeat_count;//心跳包序号
	u8 can_debug_flag;//can调试信息发送 0不发送 1发送
	u8 GI200_flag;//未定位0，已定位1
	u8 F9P_flag;//未定位0，已定位1
	u8 CAN_flag;//can通信标志位 0异常 1正常
	u8 flash_flag;//spi flash 标志位  0擦写异常 1擦写正常
	TIME InRTC;//解析时间
	double latitude;//纬度
	double longitude;//经度
	u8 satellites;//可见卫星数	
	u16 Mileage;//行驶里程  ddd.mm （单位：公里），最大9999公里
	u16 speed;//速度 0~9999 单位节 扩大10倍
	u16 altitude;//海拔 米
	u16 azimuth;//方位角 0~3600 扩大10倍
	u8 mileage_clc;//里程清零  0里程不清0 1里程清0
	
}realtime_param_type;
#pragma pack()

extern dev_param_type dev_param;//设备参数
extern realtime_param_type realtime_param;//实时参数，不保存

void stm32_param_save(void);
void stm32_param_default(void);
void stm32_param_load(void);

#endif /*_PARAM_H*/
