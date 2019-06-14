
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          gps_analysis.c
*\Description   
*\Note          
*\Log           2019.03.28    Ver 1.0    Jerry
*               创建文件。
***************************************************************************************************/
#include "gps_analysis.h"
#include <string.h>

u8 gps_buf_send[16];
/***************************************************************************************************
*\Function      GPRMC_msg_anylise
*\Description
*\Parameter     gps
*\Return        bool
*\Note
$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*<13><CR><LF>
<1> UTC（Coordinated Universal Time）时间，hhmmss（时分秒）格式
<2> 定位状态，A=有效定位，V=无效定位
<3> Latitude，纬度ddmm.mmmm（度分）格式（前导位数不足则补0）
<4> 纬度半球N（北半球）或S（南半球）
<5> Longitude，经度dddmm.mmmm（度分）格式（前导位数不足则补0）
<6> 经度半球E（东经）或W（西经）
<7> 地面速率（000.0~999.9节，Knot，前导位数不足则补0）
<8> 地面航向 方位角（000.0~359.9度，以真北为参考基准，前导位数不足则补0）
<9> UTC日期，ddmmyy（日月年）格式
<10> Magnetic Variation，磁偏角（000.0~180.0度，前导位数不足则补0）
<11> Declination，磁偏角方向，E（东）或W（西）
<12> Mode Indicator，模式指示（仅NMEA0183 3.00版本输出，A=自主定位，D=差分，E=估算，N=数据无效）
<13> 校验和
*\Log           2018.04.24    Ver 1.0    Job
创建函数。
***************************************************************************************************/
 bool GPRMC_msg_anylise(DataNode result[], u32 count, GpsSyncType *gps)
{
	double tmp = 0.0;
//	rt_kprintf("ct %d     ",count);
	if (count == 0 || count < 13)
	{
		//数据为空
		return FALSE;
	}
	//hhmmss.sss
	if (result[1].Data && result[9].Data)
	{
		gps->InRTC.Hour = 10 * (result[1].Data[0] - '0') + (result[1].Data[1] - '0');
		gps->InRTC.Minute = 10 * (result[1].Data[2] - '0') + (result[1].Data[3] - '0');
		gps->InRTC.Second = 10 * (result[1].Data[4] - '0') + (result[1].Data[5] - '0');
		gps->mSecond= 100 * (result[1].Data[7] - '0') + 10 * (result[1].Data[8] - '0') ;
		
		gps->InRTC.Day = 10 * (result[9].Data[0] - '0') + (result[9].Data[1] - '0');
		gps->InRTC.Month = 10 * (result[9].Data[2] - '0') + (result[9].Data[3] - '0');
		gps->InRTC.Year = 10 * (result[9].Data[4] - '0') + (result[9].Data[5] - '0');
		
		//printf("%d,", gps->InRTC.T.Second);
		//UTC + 8小时 北京时间
		TimeDECAddHour(&gps->InRTC, 8);


		/*printf("20%02d-%02d-%02d %02d:%02d:%02d\n", gps->InRTC.T.Year, gps->InRTC.T.Month, gps->InRTC.T.Day,
		gps->InRTC.T.Hour, gps->InRTC.T.Minute, gps->InRTC.T.Second);*/
		if (TimeDECAddSecond(&gps->InRTC, 1))//此处加1s，在秒脉冲中断中则不再需要加1s了
		{
			gps->rtc_tick = rt_tick_get();
		}
		else
		{
			gps->rtc_tick = 0;//代表出错
		}
	}
//	rt_kprintf(" %d-%d-%d  %d:%d:%d:%d  ",gps->InRTC.Year ,gps->InRTC.Month,gps->InRTC.Day,gps->InRTC.Hour,gps->InRTC.Minute ,gps->InRTC.Second,gps->mSecond);
	//定位有效标记
	if (result[2].Data)
	{
		if (result[2].Data[0] == 'A')
		{
			gps->valid_flag = 1;
		}
		else if (result[2].Data[0] == 'V')
		{
			gps->valid_flag = 0;
			return FALSE;//定位无效直接退出
		}
		else
		{
			gps->valid_flag = 0;
			return FALSE;
		}
	}
	//纬度
	if (result[3].Data != RT_NULL)
	{
		gps->latitude = atof((const char*)result[3].Data);
		gps->latitude /= 100;
	}
//	char str[32];
//	sprintf(str,"lat=%f  ",gps->latitude);
//	rt_kprintf(str);
	 
	//纬度区分
	if (result[4].Data)
	{
		if (result[4].Data[0] == 'N')
		{
			gps->lat_flag = 0x00;
		}
		else if (result[4].Data[0] == 'S')
		{
			gps->lat_flag = 0x01;
		}
	}
	//经度
	if (result[5].Data != RT_NULL)
	{
		gps->longitude = atof((const char*)result[5].Data);
		gps->longitude /= 100;
	}
		
//	sprintf(str,"log=%f  \n",gps->longitude);
//	rt_kprintf(str);
	//经度区分
	if (result[6].Data)
	{
		if (result[6].Data[0] == 'E')
		{
			gps->lat_flag = 0x00;
		}
		else if (result[6].Data[0] == 'W')
		{
			gps->lat_flag = 0x01;
		}
	}
	//速度
	if (result[7].Data)
	{
		tmp = atof((const char*)result[7].Data);
		tmp *= 100;
		gps->speed = (u16)tmp;
	}
	//地面航向
	if (result[8].Data)
	{
		tmp = atof((const char*)result[8].Data);
		tmp *= 10;
		gps->azimuth = (u16)tmp;
	}

	//磁偏角
	if (result[10].Data)
	{
		tmp = atof((const char*)result[10].Data);
		tmp *= 10;
		gps->magnetic_dec = (u16)tmp;
	}
	//磁偏角方向
	if (result[11].Data)
	{
		if (result[11].Data[0] == 'E')
		{
			gps->magnetic_flag = 0x00;
		}
		else if (result[11].Data[0] == 'W')
		{
			gps->magnetic_flag = 0x01;
		}
	}
	//定位模式
	if (result[12].Data)
	{
		if (result[12].Data[0] == 'A')
		{
			gps->mode = 0x00;
		}
		else if (result[12].Data[0] == 'D')
		{
			gps->mode = 0x01;
		}
		else if (result[12].Data[0] == 'E')
		{
			gps->mode = 0x02;
		}
		else if (result[12].Data[0] == 'N')
		{
			gps->mode = 0x03;
		}
	}


//	printf("%f\r\n", gps->longitude);
//	printf("%f\r\n", gps->latitude);
	return TRUE;
}

/***************************************************************************************************
*\Function      GPGGA_msg_anylise
*\Description   GGA解析卫星数-计算用的卫星数
*\Parameter     gps
*\Return        bool
*\Note
$GPGGA：起始引导符及语句格式说明(本句为GPS定位数据)；
<1> UTC 时间，格式为hhmmss.sss；
<2> 纬度，格式为ddmm.mmmm(第一位是零也将传送)；
<3> 纬度半球，N 或S(北纬或南纬)
<4> 经度，格式为dddmm.mmmm(第一位零也将传送)；
<5> 经度半球，E 或W(东经或西经)
<6> 定位质量指示，0=定位无效，1=定位有效；
<7>使用卫星数量，从00到12(第一个零也将传送)
<8>水平精确度，0.5到99.9
<9>天线离海平面的高度，-9999.9到9999.9米M指单位米
<10>大地水准面高度，-9999.9到9999.9米M指单位米
<11>差分GPS数据期限(RTCMSC-104)，最后设立RTCM传送的秒数量
<12>差分参考基站标号，从0000到1023(首位0也将传送)。
*\Log           2018.10.23    Ver 1.0    Job
创建函数。
***************************************************************************************************/
 bool GPGGA_msg_anylise(DataNode result[], u32 count, GpsSyncType *gps)
{
	double tmp = 0.0;

	if (count == 0 || count < 14)
	{
		gps->satellite_buf[gps->satellite_count]=gps->satellite_track;//卫星数量序列用于选择哪个gps为主
		gps->satellite_count++;//卫星序列计数
		if(gps->satellite_count>63)
		{
			gps->satellite_count=0;
		}
		return FALSE;
	}

	//解状态 0无效 1单点定位 2伪距差分 6存惯性导航解（6GI200专用）
	if (result[6].Data)
	{
		gps->QF = atof((const char*)result[6].Data);
	}
	//卫星数
	if (result[7].Data != RT_NULL)
	{
		gps->satellite_track = StrToUL(result[7].Data, RT_NULL, 10);
		gps->satellite_buf[gps->satellite_count]=gps->satellite_track;//卫星数量序列用于选择哪个gps为主
		gps->satellite_count++;//卫星序列计数
		if(gps->satellite_count>63)
		{
			gps->satellite_count=0;
		}
//		rt_kprintf("%d\n",gps->satellite_track);
	}
	//卫星定位精度 GI200专用
	if (result[8].Data)
	{
		gps->Gps_Precision = atof((const char*)result[8].Data);
	}
	//海拔
	if (result[9].Data)
	{
		gps->height = atof((const char*)result[9].Data);
	}
	//水平精确度
	if (result[11].Data)
	{
		tmp = atof((const char*)result[11].Data);
		tmp *= 10;
		gps->horizon_accuracy = (u16)tmp;
	}
	//差分延迟
	if (result[13].Data)
	{
		gps->age = atof((const char*)result[13].Data);
	}
	//组合导航训练时间
	if(count==15)
	{
		if (result[14].Data)
		{
			gps->InsTime = atof((const char*)result[14].Data);
		}		
	}

	return TRUE;
}

/***************************************************************************************************
*\Function      GPATT_msg_anylise
*\Description
*\Parameter     gps
*\Return        bool
*\Note
*\Log           2019.03.27    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
 bool GPATT_msg_anylise(DataNode result[], u32 count, GpsSyncType *gps)
{

	double tmp = 0.0;

	if (count == 0 || count != 28)
	{
		return FALSE;
	}


	//俯仰角
	if (result[1].Data)
	{
		tmp = atof((const char*)result[1].Data);
		tmp *= 100;
		gps->pitch = (u16)tmp;
	}
	//横滚角
	if (result[3].Data)
	{
		tmp = atof((const char*)result[3].Data);
		tmp *= 100;
		gps->roll = (u16)tmp;
	}
	//偏航角
	if (result[5].Data)
	{
		tmp = atof((const char*)result[5].Data);
		tmp *= 100;
		gps->yaw = (u16)tmp;
	}

	//软件版本号
	if (result[7].Data)
	{
		gps->softversion = atof((const char*)result[7].Data);
	}

	//默认打开惯性导航  1：打开，0：关闭
	if (result[11].Data)
	{
		gps->ins = atof((const char*)result[11].Data);
	}

	//算法状态标志
	if (result[14].Data)
	{
		gps->State_Flag = atof((const char*)result[14].Data);
	}

	//选择GPS+BD/GPS+Glonass B:GPS+BD,G:GPS+Glonass
	if (result[17].Data)
	{
		gps->bd_flag = atof((const char*)result[17].Data);
	}

	//静态标志位  1：静止，0：代表动态
	if (result[19].Data)
	{
		gps->StaticFlag = atof((const char*)result[19].Data);
	}
	//用户编号 1：普通用户，X：定制用户	
	if (result[20].Data)
	{
		gps->Uer_Code = atof((const char*)result[20].Data);
	}

	//是否采用flash安装角标志 1：不采用存储安装角  2:采用存储安装角
	if (result[21].Data)
	{
		gps->Angle_Select = atof((const char*)result[21].Data);
	}

	//车库内保存卫星最后位置的标志 1：有车库位置记录成功  0：无记录
	if (result[22].Data)
	{
		gps->Save_Gps_Flag = atof((const char*)result[22].Data);
	}

	//安装角选择通道 
	if (result[23].Data)
	{
		gps->ALocK_Channel = atof((const char*)result[23].Data);
	}

	//是否固定安装标志 1：启动固定安装，0：启动自适应安装
	if (result[24].Data)
	{
		gps->Angle_Lock_Flag = atof((const char*)result[24].Data);
	}

	//UBI 1->8
	if (result[25].Data)
	{
		gps->UBI_On_Flag = atof((const char*)result[25].Data);
	}

	//城市地形 0：平坦地形，1：山城（类似澳门）
	if (result[26].Data)
	{
		gps->City_Type = atof((const char*)result[26].Data);
	}
	//行驶里程  ddd.mm （单位：公里），最大9999公里
	if (result[27].Data)
	{
		tmp = atof((const char*)result[27].Data);
		tmp *= 100;
		gps->Mileage = (u16)tmp;
	}

	return TRUE;
}

/***************************************************************************************************
*\Function      GPGSV_msg_anylise
*\Description   解析卫星数，天空可见的卫星数
*\Parameter     gps
*\Return        bool
*\Note
*\Log           2018.10.23    Ver 1.0    Job
创建函数。
***************************************************************************************************/
 bool GPGSV_msg_anylise(DataNode result[], u32 count, GpsSyncType *gps)
{

	if (count == 0 || count < 3)
	{
		return FALSE;
	}
	if (result[3].Data != RT_NULL)
	{
		gps->satellite_view = StrToUL(result[3].Data, RT_NULL, 10);
	}
	return TRUE;
}

extern rt_mq_t mq_gps_data;//GPS数据解析后发送的消息队列
//GPS数据存储
/***************************************************************************************************
*\Function      GPS_Data_Storage
*\Description   
*\Parameter     gps  
*\Return        bool  
*\Note          
*\Log           2019.05.10    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
bool GPS_Data_Storage(GpsSyncType *gps)
{
	int result;
	float data_trans;//

	//当前GPS不是主GPS不进行数据存储
	if(gps->GPS_Master_Flag==0)
	{
		return FALSE;
	}
	
	//gps 定位标志位为V 无效定位信息 不进行存储
	if(gps->valid_flag==0)
	{
		return FALSE;
	}
	
	//开始存储数据
	//存储 年月日 时分秒 毫秒 经纬度
	gps_buf_send[0]=gps->InRTC.Year;
	gps_buf_send[1]=gps->InRTC.Month;
	gps_buf_send[2]=gps->InRTC.Day;
	gps_buf_send[3]=gps->InRTC.Hour;
	gps_buf_send[4]=gps->InRTC.Minute;
	gps_buf_send[5]=gps->InRTC.Second;
	gps_buf_send[6]=(u8)gps->mSecond;//低8位
	gps_buf_send[7]=(u8)(gps->mSecond >> 8);//高8位
	data_trans=(float)gps->longitude;
	memcpy(&gps_buf_send[8], &data_trans, sizeof(data_trans));//经度
	data_trans=(float)gps->latitude;
	memcpy(&gps_buf_send[12], &data_trans, sizeof(data_trans));//纬度
	result=rt_mq_send(mq_gps_data,gps_buf_send,sizeof(gps_buf_send));
	
	return TRUE;
}


//主GPS 选择 每S 进行一次判断 
/***************************************************************************************************
*\Function      GPS_Master_Select
*\Description   
*\Parameter       
*\Return        bool  
*\Note          
*\Log           2019.05.10    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
bool GPS_Master_Select(void)
{  
	u8 gi200_position,f9p_position;
	if(GPS_GI200.satellite_count==0)
	{
		gi200_position=63;
	}
	else
	{
		gi200_position=GPS_GI200.satellite_count-1;
	}
	
	if(GPS_F9P.satellite_count==0)
	{
		f9p_position=63;
	}
	else
	{
		f9p_position=GPS_F9P.satellite_count-1;
	}
	
	//对比F9P GI200最新的星数 
	if(GPS_GI200.satellite_buf[gi200_position]>GPS_F9P.satellite_buf[f9p_position])
	{
		GPS_F9P.GPS_Master_Flag=0;
		GPS_GI200.GPS_Master_Flag=1;
	}
	else
	{
		GPS_GI200.GPS_Master_Flag=0;
		GPS_F9P.GPS_Master_Flag=1;				
	}

	return TRUE;
}


/***************************************************************************************************
*\Function      rt_thread_gps_entry
*\Description   
*\Parameter     parameter  
*\Return        void  
*\Note          
*\Log           2019.05.10    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void rt_thread_gps_entry(void* parameter)
{	
	//10s比对一次gps
	while (1)
	{
		GPS_Master_Select();
		rt_thread_delay(1000);
	}
}


/***************************************************************************************************
*\Function      rt_thread_gps_init
*\Description   
*\Parameter       
*\Return        void  
*\Note          
*\Log           2019.05.10    Ver 1.0    Jerry
*               创建函数。
***************************************************************************************************/
void rt_thread_gps_init(void)
{
	rt_thread_t gps_init_thread;                    
	gps_init_thread = rt_thread_create("gps",
		rt_thread_gps_entry,RT_NULL,512,12,5);
	if (gps_init_thread != RT_NULL)
		rt_thread_startup(gps_init_thread);
}




/***************************************************************************************************
******************************************** END OF FILE *******************************************
***************************************************************************************************/


