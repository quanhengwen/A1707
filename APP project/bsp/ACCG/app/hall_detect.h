/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          hall_detect.h
*\Description   
*\Note          
*\Log           2019.04.17    Ver 1.0    杨贝贝
*               创建文件。
***************************************************************************************************/
#ifndef _HALL_DETECT_H_
#define _HALL_DETECT_H_
#include "public_type.h"
#include <math.h>
#include "rtdef.h"





void config_gpio_to_input(GpioType* gpio_dev);


void hall_detect_thread_entry(void* p);

#endif
