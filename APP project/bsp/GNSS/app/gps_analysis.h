
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          gps_analysis.h
*\Description   
*\Note          
*\Log           2019.03.28    Ver 1.0    Jerry
*               创建文件。
***************************************************************************************************/
#ifndef _GPS_ANALYSIS_H
#define _GPS_ANALYSIS_H

#include "public_type.h"
#include "common_lib.h"
#include "drv_gps.h"
#include <stdio.h>
#include <stdlib.h>

extern bool GPRMC_msg_anylise(DataNode result[], u32 count, GpsSyncType *gps);
extern bool GPGGA_msg_anylise(DataNode result[], u32 count, GpsSyncType *gps);
extern bool GPATT_msg_anylise(DataNode result[], u32 count, GpsSyncType *gps);
extern bool GPGSV_msg_anylise(DataNode result[], u32 count, GpsSyncType *gps);
extern bool GPS_Data_Storage(GpsSyncType *gps);
#endif /*_GPS_ANALYSIS_H*/



/***************************************************************************************************
******************************************** END OF FILE *******************************************
***************************************************************************************************/


