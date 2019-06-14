/***************************************************************************************************
*                    (c) Copyright 2006-2016  Syncretize technologies co.,ltd.
*										All Rights Reserved
*	
*\File 			param.c
*\Description   
*\Note          
*\Log           2016.05.09    Ver 1.0   Job
*               创建文件。
***************************************************************************************************/
#include "param.h"
#include "iap_if.h"
#include "flash_if.h"
#include "common_lib.h"
#include "manage_id.h"

//ip
u8 RT_LWIP_IPADDR0;
u8 RT_LWIP_IPADDR1;
u8 RT_LWIP_IPADDR2;
u8 RT_LWIP_IPADDR3;
//gw
u8 RT_LWIP_GWADDR0;
u8 RT_LWIP_GWADDR1;
u8 RT_LWIP_GWADDR2;
u8 RT_LWIP_GWADDR3;
//msk
u8 RT_LWIP_MSKADDR0;
u8 RT_LWIP_MSKADDR1;
u8 RT_LWIP_MSKADDR2;
u8 RT_LWIP_MSKADDR3;
/* DNS 地址*/
u8 RT_LWIP_DNSADDR0;
u8 RT_LWIP_DNSADDR1;
u8 RT_LWIP_DNSADDR2;
u8 RT_LWIP_DNSADDR3;

dev_param_type dev_param;//设备参数
realtime_param_type realtime_param={0};//实时参数，不保存
/***************************************************************************************************
*\Function      stm32_param_save
*\Description   参数保存接口
*\Parameter     
*\Return        void
*\Note          
*\Log           2015.07.06    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void stm32_param_save(void)
{
    u16 wdata; 
	 u32 addr;
	uint32_t addr_appparam =  FLASH_START_ADDR + Get_FlashSize() * 1024 - FLASH_APP_PARAM_ADDR*FLASH_PAGE_SIZE;//FLASH_APP_PARAM_ADDR;
	addr = addr_appparam;
	Flash_Unlock();
    /* 擦除这个扇区 */
	if (FLASH_ErasePage(addr) != FLASH_COMPLETE)
	{
		rt_kprintf("\r page 0x%08X erase fail!\n\r", addr);
	}	

    //IP地址
    wdata=RT_LWIP_IPADDR0;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_IPADDR1;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_IPADDR2;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_IPADDR3;
    FLASH_ProgramHalfWord(addr,wdata); 

    //NETMASK
    addr++;
    addr++;
    wdata=RT_LWIP_MSKADDR0;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_MSKADDR1;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_MSKADDR2;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_MSKADDR3;
    FLASH_ProgramHalfWord(addr,wdata);

    //GATEWAY
    addr++;
    addr++;
    wdata=RT_LWIP_GWADDR0;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_GWADDR1;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_GWADDR2;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_GWADDR3;
    FLASH_ProgramHalfWord(addr,wdata);

    //DNS
    addr++;
    addr++;
    wdata=RT_LWIP_DNSADDR0;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_DNSADDR1;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_DNSADDR2;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    wdata=RT_LWIP_DNSADDR3;
    FLASH_ProgramHalfWord(addr,wdata);

    addr++;
    addr++;
    Flash_Write(addr, (u8*)&dev_param, sizeof(dev_param));
    addr += sizeof(dev_param);

    FLASH_Lock();
}
/***************************************************************************************************
*\Function      stm32_param_default
*\Description   恢复默认参数接口
*\Parameter     
*\Return        void
*\Note          
*\Log           2015.07.06    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void stm32_param_default(void)
{
	int i;
    //IP地址
    RT_LWIP_IPADDR0 = 192;
    RT_LWIP_IPADDR1 = 168;
    RT_LWIP_IPADDR2 = 1;
    RT_LWIP_IPADDR3 = 100;

    //NETMASK
    RT_LWIP_MSKADDR0 = 255;
    RT_LWIP_MSKADDR1 = 255;
    RT_LWIP_MSKADDR2 = 255;
    RT_LWIP_MSKADDR3 = 0;

    //GATEWAY
    RT_LWIP_GWADDR0 = 192;
    RT_LWIP_GWADDR1 = 168;
    RT_LWIP_GWADDR2 = 1;
    RT_LWIP_GWADDR3 = 1;

    //DNS
    RT_LWIP_DNSADDR0 = 192;
    RT_LWIP_DNSADDR1 = 168;
    RT_LWIP_DNSADDR2 = 1;
    RT_LWIP_DNSADDR3 = 1;

    rt_memset(&dev_param, 0, sizeof(dev_param));

    dev_param.Power_First_Flag = DEV_APP_FIRST_POWER_FLAG;
	
	dev_param.mswork_mode = 0;//主从工作模式 主0从1
	dev_param.version =0x01;//app版本
	dev_param.analog_dial_switch=0;//模拟拨码开关
    //GPS时间同步周期
    dev_param.GPS_Period = 300;//5分钟同步一次
    dev_param.RTC_Period = 300;//5分钟一次	
	dev_param.gps_select=0;//0 GI200 F9P都使用；1仅使用GI200;2仅使用F9P；3不使用GPS
	dev_param.gps_select=0;//0 GI200 F9P都使用；1仅使用GI200;2仅使用F9P；3不使用GPS
	dev_param.GI200_freq=0;//GI200频率  0--1hz  1--5hz
	dev_param.GI200_BDGLONS=1;//GI200 BD GLONS选择 0 GLONS 1 BD(GI200)
	dev_param.GI200_AXES=1;//GI200安装坐标系 1--8	
    stm32_param_save();
}

//实时参数初始化
void realtime_param_init(void)
{
	realtime_param.voltage=0;
	realtime_param.current=0;
	realtime_param.heartbeat_count=0;
	realtime_param.can_debug_flag=0;
	realtime_param.GI200_flag=0;//未定位0，已定位1
	realtime_param.F9P_flag=0;//未定位0，已定位1
	realtime_param.CAN_flag=1;//can通信标志位 0异常 1正常
	realtime_param.flash_flag=1;//spi flash 标志位  0擦写异常 1擦写正常
	//realtime_param.InRTC
	//double latitude;//纬度
	//double longitude;//经度
	realtime_param.satellites=0;//可见卫星数	
	realtime_param.Mileage=0;//行驶里程  ddd.mm （单位：公里），最大9999公里
	realtime_param.speed=0;//速度 0~9999 单位节 扩大10倍
	realtime_param.altitude=0;//海拔 米
	realtime_param.azimuth=0;//方位角 0~3600 扩大10倍
	realtime_param.mileage_clc=0;//里程清零  0里程不清0 1里程清0
	
}

/***************************************************************************************************
*\Function      stm32_param_load
*\Description   读取参数接口
*\Parameter     
*\Return        void
*\Note          
*\Log           2015.07.06    Ver 1.0    张波
*               创建函数。
***************************************************************************************************/
void stm32_param_load(void)
{
    u32 addr;
	uint32_t addr_appparam =  FLASH_START_ADDR + Get_FlashSize() * 1024 - FLASH_APP_PARAM_ADDR*FLASH_PAGE_SIZE;//FLASH_APP_PARAM_ADDR;
	addr=addr_appparam;
    dev_param.Power_First_Flag = *(vu16*)(addr);
    if (dev_param.Power_First_Flag != DEV_APP_FIRST_POWER_FLAG)
    {     
        rt_kprintf("Load Default Param[%04X]...\r\n", dev_param.Power_First_Flag);
        //2015年12月17日 如果第一次上电，则默认值
        stm32_param_default(); //调用默认参数,擦除FLASH之后调用
    }
    else
    {
        rt_kprintf("Load Param... \r\n");
    }

    //IP 
    RT_LWIP_IPADDR0 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_IPADDR1 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_IPADDR2 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_IPADDR3 = *(vu16*)(addr);
    addr++;
    addr++;

    //NETMASK
    RT_LWIP_MSKADDR0 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_MSKADDR1 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_MSKADDR2 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_MSKADDR3 = *(vu16*)(addr);
    addr++;
    addr++;

    //GATEWAY
    RT_LWIP_GWADDR0 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_GWADDR1 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_GWADDR2 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_GWADDR3 = *(vu16*)(addr);
    addr++;
    addr++;

    //DNS
    RT_LWIP_DNSADDR0 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_DNSADDR1 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_DNSADDR2 = *(vu16*)(addr);
    addr++;
    addr++;

    RT_LWIP_DNSADDR3 = *(vu16*)(addr);
    addr++;
    addr++;

    Flash_Read(addr, (u8*)&dev_param, sizeof(dev_param));
    addr += sizeof(dev_param);
	
	dev_param.version =0x01;//app版本 根据工程实际情况设置
	
	iap_param_load();//iap参数加载
	realtime_param_init();//实时参数初始化
	device_id_set();//设备ID设置
}

