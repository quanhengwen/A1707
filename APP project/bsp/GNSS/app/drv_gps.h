
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          drv_gps.h
*\Description   
*\Note          
*\Log           2019.03.28    Ver 1.0    Jerry
*               创建文件。
***************************************************************************************************/
#ifndef _DRV_GPS_H
#define _DRV_GPS_H
#include "public_type.h"
#include "common_lib.h"
#include "drv_com_uart.h"
#include "bit_band_macro.h"
#include "drv_switch.h"
#include "drv_InRTC.h"


#include "rtthread.h"


#define GNSS_PROTOCOL_HEAD  '$'
#define GNSS_PROTOCOL_TAIL1  0x0D
#define GNSS_PROTOCOL_TAIL2  0x0A

#define GNSS_PROTOCOL_MIN_LEN  8


/*GPS时间同步类型*/
typedef struct Gps_Sync_type_st
{
	u8 GPS_Master_Flag;//GPS主标志位 0从 1主 默认f9p为主 主GPS的数据进行存储
//    USART_TypeDef* uart;
//    GpioType*   TxGpio; //uart发送脚
//    GpioType*   RxGpio; //uart接收脚
//    GpioType*   PulseGpio;//秒脉冲信号
//    switch_type*RST;//复位
//    switch_type*PWCtl;//电源引脚
//    u32         baudrate;//波特率，发送时会根据波特率计算等待时间
//    /*上面需初始化赋值*/
//    u8          rxbuf[GPS_MAX_MSG_SIZE];//uart接收缓冲区
//    u32         rx_write;//写入指针
//    u8          crc;//异或运算
    TIME InRTC;//解析时间
    u32         rtc_tick;//解析成功RTC的tick
//    u32         tick_last;//最后同步成功的tick
//    u8          gps_state;//解析状态

//    u8          sync_flag;//时间同步标志 1表示要进行同步
//    u32         uart_nums;//串口接收数据
//    //经纬度等信息
////    u8 valid_flag;//定位状态 0x01有效
////    u8 lat_flag;//纬度区分 0 北纬，1南纬
////    double latitude;//纬度
////    u8 lon_flag;//经度区分 0 东经 1 西经
////    double longitude;//经度
//    u8 gps_cmd[3];//解析标记RMC GGA GSV，
   
	
	u16 mSecond;//gps ms
	//GPRMC GNRMC
    u8 valid_flag;//定位状态 0x01有效
    u8 lat_flag;//纬度区分 0 北纬，1南纬
    double latitude;//纬度
    u8 lon_flag;//经度区分 0 东经 1 西经
    double longitude;//经度
    u16 altitude;//海拔 米
    u16 speed;//速度 0~9999 单位节 扩大10倍
    u16 azimuth;//方位角 0~3600 扩大10倍
    u16 magnetic_dec;//磁偏角 0~1800 扩大10倍
    u8 magnetic_flag;// 0 东 1 西
    u8 mode;// 0 自动，1 差分， 2 估测，3 数据无效
    u8 satellites;//可见卫星数	
	
	//GPGGA GNGGA
	u8 QF;//解状态 0无效 1单点定位 2伪距差分 6存惯性导航解（6GI200专用）
    u8 satellite_track;//卫星总数 GGA 解析 7
	u8 satellite_buf[64];//卫星数量序列用于选择哪个gps为主
	u8 satellite_count;//卫星序列计数
	u16 Gps_Precision;//卫星定位精度 GI200专用
	u16 horizon_accuracy;//水平精确度
	u16 age;//差分延迟
	u16 InsTime;//组合导航训练时间
	double height;//天线离海平面的高度
	//大地水准面高度
	//差分GPS数据期限
	//差分参考基站标号

	//GPATT
	u16 pitch;//俯仰角
	u16 roll;//横滚角
	u16 yaw;//偏航角
	u32 softversion;//软件版本号
	u8 ins;//默认打开惯性导航  1：打开，0：关闭
	u8 State_Flag;//算法状态标志
	u8 bd_flag;//选择GPS+BD/GPS+Glonass B:GPS+BD,G:GPS+Glonass
	u8 StaticFlag;//静态标志位  1：静止，0：代表动态
	u8 Uer_Code;//用户编号 1：普通用户，X：定制用户
	u8 Angle_Select;//是否采用flash安装角标志 1：不采用存储安装角  2:采用存储安装角
	u8 Save_Gps_Flag;//车库内保存卫星最后位置的标志 1：有车库位置记录成功  0：无记录
	u8 ALocK_Channel;//安装角选择通道 
	u8 Angle_Lock_Flag;//是否固定安装标志 1：启动固定安装，0：启动自适应安装
	u8 UBI_On_Flag;//UBI 1->8
	u8 City_Type;//城市地形 0：平坦地形，1：山城（类似澳门）
	double Mileage;//行驶里程  ddd.mm （单位：公里），最大9999公里

	//GPGSV
	u8 satellite_view;//卫星总数 GSV 解析 3

    u8    ErrorFlag;//目前为1同步出错，可能秒脉冲有问题，2 串口无接收数据
}GpsSyncType;

/*解析用结构*/
typedef enum
{
    GPSanylise_waiting,
    GPSanylise_accept,
    GPSanylise_reject,   
}GPSanylise_state;

typedef struct GPSdel_info_type_st 
{
    GPSanylise_state state;
    u32 del_len;
}GPSdel_info_type;



extern GpsSyncType GPS_GI200;
extern GpsSyncType GPS_F9P;
extern void gps_init(void);

#endif /*_DRV_GPS_H*/



/***************************************************************************************************
******************************************** END OF FILE *******************************************
***************************************************************************************************/


