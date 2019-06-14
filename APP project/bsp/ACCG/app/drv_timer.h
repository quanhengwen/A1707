/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_timer.h
*\Description   
*\Note          
*\Log           2019.01.03    Ver 1.0    杨贝贝
*               创建文件。
***************************************************************************************************/
#ifndef _DRV_TIMER_H
#define _DRV_TIMER_H
#include "public_type.h"


typedef struct drv_timer_type_st
{
    TIM_TypeDef* TIM;
	uint32_t  Freq; 
	void (*TIMCallback)(struct drv_timer_type_st *dev);
	uint16_t period;
	uint16_t prescaler;
    //以上需要配置
	rt_mutex_t lock;//设备锁
	void  *user_data;//用户私有数据
}drv_timer_type;


void init_timer(drv_timer_type* timer);
void set_timer_freq(drv_timer_type* timer, uint32_t _ulFreq);
void turn_on_timer(drv_timer_type* timer);
void turn_off_timer(drv_timer_type* timer);
void set_timer_callback(drv_timer_type* timer,void (*func)(struct drv_timer_type_st *dev));
void stm32_timer_NVIC_config(drv_timer_type* timer);
#endif /*_DRV_PWM_H*/
