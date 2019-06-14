/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          lidar_hall.h
*\Description   
*\Note          
*\Log           2019.04.09    Ver 1.0    mcc
*               创建文件。
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


#define LIDAR_REPORT_MIN_DIST   800   //<8m立即上报平台 --- 需根据实测修改此值
#define VTH                     2700  //mV，阈值，大于此值说明已受到撞击 --- 需根据实测修改此值


#define EVENT_BROAD_MAX_NUM     3     //检测到障碍物或受到冲击后，最大上传通知平台次数
#define EVENT_BROAD_GAP_NUM     10    //检测到障碍物或受到冲击后，连续上报EVENT_BROAD_MAX_NUM次后，间隔EVENT_BROAD_GAP_NUM次上报一次


/***************配置命令****************/
#define LIDAR_CONFIG_CMD            0x02 
#define LIDAR_CONFIG_PERIOD_CMD     0x07 
#define LIDAR_CONFIG_MODE_CMD       0x14 
#define LIDAR_CONFIG_MODE_GEAR_CMD  0x11 


typedef struct OAG_protocol_type_st
{	
	u8      lidar_config_enable;   //测距模组配置模式-0x01 进入；0x00 退出
	u16     lidar_config_period;   //测距模组数据输出周期设置
	u8      lidar_config_mode;     //测距模组测距模式设置-0x00 自动模式；0x01 固定模式
	u8      lidar_config_mode_gear;//配置档位：0x00 近距离档位；0x03 中距离档位；0x07 远距离档位
	u16     lidar_broad_dist_th;   //距离上报阈值
	
	u8      lidar_config_ack;      //配置信息回复
	u8      lidar_config_cmd_ack;  //配置信息回复命令
	
	u8      lidar_barrier_flag;  //是否有障碍物标记
	u16 	lidar_dist;     	 //距离信息
	u16 	lidar_strength; 	 //信号强度
	u8      lidar_mode_gear;     //测距档位-0x00 近距离档；0x03 中距离档；0x07 远距离档
	u8      lidar_failure_state; //故障状态
	
	u8      hall_hit_flag;       //是否受到撞击标记
	u16     hall_vol;            //采集电压
	u8      hall_failure_state;  //故障状态
}OAG_protocol_type;


enum{
	START_CONFIG  = 0x00,     //进入配置模式
	CONFIG_PERIOD = 0x01,     //配置输出周期
	CONFIG_MODE   = 0x02,     //配置测距模式
	CONFIG_GEAR   = 0x03,	  //配置测距档位
	STOP_CONFIG   = 0x04,     //退出配置模式
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



