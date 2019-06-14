/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2014-04-27     Bernard      make code cleanup. 
 */

#include <drv_board.h>
#include <rtthread.h>

#ifdef RT_USING_FINSH
#include <shell.h>
#include <finsh.h>
#endif

#ifdef RT_USING_LWIP
#include <lwip/sys.h>
#include <lwip/api.h>
#include "eprt_server.h"
#endif
#ifdef RT_USING_DFS
#include <dfs_posix.h>
#include "dfs_init.h"
#endif
#include "param.h"
#include "drv_watchdog.h"
#include "app_board.h"
#include "iap_if.h"
#include "adc.h"
#include "can_heartbeat_protocol.h"

void akey_stop_thread_entry(void* parameter);
void system_runstatemode_thread_entry(void* parameter);
void CAN_Heartbeat_Package_Send(void);
	
void rt_init_thread_entry(void* parameter)
{
    rt_thread_t tid;

    //驱动配置初始化，必须在前面
    app_board_init();

#ifdef RT_USING_FINSH
    /* init finsh */
    finsh_system_init();
    finsh_set_device(FINSH_DEVICE_NAME);
#endif
	
    /*========================================================*/
    //线程名称：一键启动关闭  
    //优先级	14 
    /*========================================================*/
	  tid = rt_thread_create("akey_start", akey_stop_thread_entry, RT_NULL, 512, 14, 5);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
		
    /*========================================================*/
    //线程名称：MAX9611电流电测 
    //优先级	13 
    /*========================================================*/
    tid = rt_thread_create("max9611", max9611_thread_entry, RT_NULL, 512, 13, 5);
    if (tid != RT_NULL)
        rt_thread_startup(tid);  

		/*========================================================*/
    //线程名称：MAX9611电流电测 
    //优先级	17 
    /*========================================================*/
    tid = rt_thread_create("max9611", max9611_charge_thread_entry, RT_NULL, 512, 17, 5);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
		
    /*========================================================*/
    //线程名称：adc电压检测，电池、充电器
    //优先级	16 
    /*========================================================*/
    tid = rt_thread_create("adc", adc_chapwr_thread_entry, RT_NULL, 512, 16, 5);
    if (tid != RT_NULL)
        rt_thread_startup(tid); 
		
		/*========================================================*/
    //线程名称：运行状态检测
    //优先级	12 
    /*========================================================*/
	  tid = rt_thread_create("work_mode", system_runstatemode_thread_entry, RT_NULL, 512, 12, 5);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
    /*========================================================*/
    //线程名称：指示灯   
    //优先级	14 
    /*========================================================*/
//    tid = rt_thread_create("msgled", msg_led_thread_entry, RT_NULL, 512, 14, 5);
//    if (tid != RT_NULL)
//        rt_thread_startup(tid); 

}

#include "canbus_middle.h"
int rt_application_init()
{
    rt_thread_t tid;

    //获取复位方式
    GetRestFlag();
    //下载参数
    stm32_param_load();

    tid = rt_thread_create("init", rt_init_thread_entry, RT_NULL, 2048, 3, 20);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

    /*========================================================*/
    //线程名称：看门狗任务   
    //优先级	15 
    /*========================================================*/
    tid = rt_thread_create("watchdog", rt_watchdog_thread_entry, RT_NULL, 512, 15, 5);
    if (tid != RT_NULL)
        rt_thread_startup(tid);    
	
    tid = rt_thread_create("can", canbus_middle_thread_entry, RT_NULL, 512, 15, 5);
    if (tid != RT_NULL)
        rt_thread_startup(tid);    
    //cpu占用率统计
#ifdef CPU_USAGE_ENABLE
    {
        void cpu_usage_init(void);
        cpu_usage_init();
    }
#endif

    return 0;
}
/*****************************************************add block***************************************************************************/
/***************************************************************************************************
*\Function      akey_stop_thread_entry
*\Description   一键启动关闭检测，关闭检测按键按下2S
*\Parameter     
*\Parameter     
*\Return        
*\Note          
*\Log           2019.02.11    Ver 1.0    baron               
				创建函数。
***************************************************************************************************/
void akey_stop_thread_entry(void* parameter)
{
	static u16 t_num = 0;
  while(1)
  {
	 rt_thread_delay(2);
	 while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15) == Bit_RESET)
	 {
		 t_num++;
		 rt_thread_delay(2);
	 }
	 if(t_num >= 1 && t_num < 50)//短按开机
	 {
		 t_num = 0;
		 state.power_on = 1;
		 GPIO_SetBits(GPIOB,GPIO_Pin_14);//开机 
	 }
	 else if(t_num >= 100)//长按关机
	 {
		 t_num = 0;
		 state.power_on = 0;
		 GPIO_ResetBits(GPIOB,GPIO_Pin_14);//置低，关机
		 rt_thread_delay(300);
	 }
  }
}
/***************************************************************************************************
*\Function      system_runstatemode_thread_entry
*\Description   设备运行状态实时检测,心跳包
*\Parameter     
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.13    Ver 1.0    baron               
				创建函数。
***************************************************************************************************/
void system_runstatemode_thread_entry(void* parameter)
{
	static u8 now_mode=0x00,last_mode=0x04;
	
  while(1)
  {
	 rt_thread_delay(10);
   if(state.power_on == 1 && state.charge_on == 0){ state.work_mode = 0x00; now_mode = state.work_mode;}
	 if(state.power_on == 1 && state.charge_on == 1){ state.work_mode = 0x01; now_mode = state.work_mode;}
	 if(state.power_on == 0 && state.charge_on == 0){ state.work_mode = 0x02; now_mode = state.work_mode;}
	 if(state.power_on == 0 && state.charge_on == 1){ state.work_mode = 0x03; now_mode = state.work_mode;}
	
	 if(last_mode != now_mode)//100ms检测间隔，状态不同时立刻上传
	 {
	   last_mode = now_mode;
		 CAN_Heartbeat_Package_Send();
	 }
	 else
	 {}
  }
}
/*@}*/
