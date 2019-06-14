
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          gps_analysis.c
*\Description   
*\Note          
*\Log           2019.03.28    Ver 1.0    Jerry
*               �����ļ���
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
<1> UTC��Coordinated Universal Time��ʱ�䣬hhmmss��ʱ���룩��ʽ
<2> ��λ״̬��A=��Ч��λ��V=��Ч��λ
<3> Latitude��γ��ddmm.mmmm���ȷ֣���ʽ��ǰ��λ��������0��
<4> γ�Ȱ���N�������򣩻�S���ϰ���
<5> Longitude������dddmm.mmmm���ȷ֣���ʽ��ǰ��λ��������0��
<6> ���Ȱ���E����������W��������
<7> �������ʣ�000.0~999.9�ڣ�Knot��ǰ��λ��������0��
<8> ���溽�� ��λ�ǣ�000.0~359.9�ȣ����汱Ϊ�ο���׼��ǰ��λ��������0��
<9> UTC���ڣ�ddmmyy�������꣩��ʽ
<10> Magnetic Variation����ƫ�ǣ�000.0~180.0�ȣ�ǰ��λ��������0��
<11> Declination����ƫ�Ƿ���E��������W������
<12> Mode Indicator��ģʽָʾ����NMEA0183 3.00�汾�����A=������λ��D=��֣�E=���㣬N=������Ч��
<13> У���
*\Log           2018.04.24    Ver 1.0    Job
����������
***************************************************************************************************/
 bool GPRMC_msg_anylise(DataNode result[], u32 count, GpsSyncType *gps)
{
	double tmp = 0.0;
//	rt_kprintf("ct %d     ",count);
	if (count == 0 || count < 13)
	{
		//����Ϊ��
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
		//UTC + 8Сʱ ����ʱ��
		TimeDECAddHour(&gps->InRTC, 8);


		/*printf("20%02d-%02d-%02d %02d:%02d:%02d\n", gps->InRTC.T.Year, gps->InRTC.T.Month, gps->InRTC.T.Day,
		gps->InRTC.T.Hour, gps->InRTC.T.Minute, gps->InRTC.T.Second);*/
		if (TimeDECAddSecond(&gps->InRTC, 1))//�˴���1s�����������ж���������Ҫ��1s��
		{
			gps->rtc_tick = rt_tick_get();
		}
		else
		{
			gps->rtc_tick = 0;//�������
		}
	}
//	rt_kprintf(" %d-%d-%d  %d:%d:%d:%d  ",gps->InRTC.Year ,gps->InRTC.Month,gps->InRTC.Day,gps->InRTC.Hour,gps->InRTC.Minute ,gps->InRTC.Second,gps->mSecond);
	//��λ��Ч���
	if (result[2].Data)
	{
		if (result[2].Data[0] == 'A')
		{
			gps->valid_flag = 1;
		}
		else if (result[2].Data[0] == 'V')
		{
			gps->valid_flag = 0;
			return FALSE;//��λ��Чֱ���˳�
		}
		else
		{
			gps->valid_flag = 0;
			return FALSE;
		}
	}
	//γ��
	if (result[3].Data != RT_NULL)
	{
		gps->latitude = atof((const char*)result[3].Data);
		gps->latitude /= 100;
	}
//	char str[32];
//	sprintf(str,"lat=%f  ",gps->latitude);
//	rt_kprintf(str);
	 
	//γ������
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
	//����
	if (result[5].Data != RT_NULL)
	{
		gps->longitude = atof((const char*)result[5].Data);
		gps->longitude /= 100;
	}
		
//	sprintf(str,"log=%f  \n",gps->longitude);
//	rt_kprintf(str);
	//��������
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
	//�ٶ�
	if (result[7].Data)
	{
		tmp = atof((const char*)result[7].Data);
		tmp *= 100;
		gps->speed = (u16)tmp;
	}
	//���溽��
	if (result[8].Data)
	{
		tmp = atof((const char*)result[8].Data);
		tmp *= 10;
		gps->azimuth = (u16)tmp;
	}

	//��ƫ��
	if (result[10].Data)
	{
		tmp = atof((const char*)result[10].Data);
		tmp *= 10;
		gps->magnetic_dec = (u16)tmp;
	}
	//��ƫ�Ƿ���
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
	//��λģʽ
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
*\Description   GGA����������-�����õ�������
*\Parameter     gps
*\Return        bool
*\Note
$GPGGA����ʼ������������ʽ˵��(����ΪGPS��λ����)��
<1> UTC ʱ�䣬��ʽΪhhmmss.sss��
<2> γ�ȣ���ʽΪddmm.mmmm(��һλ����Ҳ������)��
<3> γ�Ȱ���N ��S(��γ����γ)
<4> ���ȣ���ʽΪdddmm.mmmm(��һλ��Ҳ������)��
<5> ���Ȱ���E ��W(����������)
<6> ��λ����ָʾ��0=��λ��Ч��1=��λ��Ч��
<7>ʹ��������������00��12(��һ����Ҳ������)
<8>ˮƽ��ȷ�ȣ�0.5��99.9
<9>�����뺣ƽ��ĸ߶ȣ�-9999.9��9999.9��Mָ��λ��
<10>���ˮ׼��߶ȣ�-9999.9��9999.9��Mָ��λ��
<11>���GPS��������(RTCMSC-104)���������RTCM���͵�������
<12>��ֲο���վ��ţ���0000��1023(��λ0Ҳ������)��
*\Log           2018.10.23    Ver 1.0    Job
����������
***************************************************************************************************/
 bool GPGGA_msg_anylise(DataNode result[], u32 count, GpsSyncType *gps)
{
	double tmp = 0.0;

	if (count == 0 || count < 14)
	{
		gps->satellite_buf[gps->satellite_count]=gps->satellite_track;//����������������ѡ���ĸ�gpsΪ��
		gps->satellite_count++;//�������м���
		if(gps->satellite_count>63)
		{
			gps->satellite_count=0;
		}
		return FALSE;
	}

	//��״̬ 0��Ч 1���㶨λ 2α���� 6����Ե����⣨6GI200ר�ã�
	if (result[6].Data)
	{
		gps->QF = atof((const char*)result[6].Data);
	}
	//������
	if (result[7].Data != RT_NULL)
	{
		gps->satellite_track = StrToUL(result[7].Data, RT_NULL, 10);
		gps->satellite_buf[gps->satellite_count]=gps->satellite_track;//����������������ѡ���ĸ�gpsΪ��
		gps->satellite_count++;//�������м���
		if(gps->satellite_count>63)
		{
			gps->satellite_count=0;
		}
//		rt_kprintf("%d\n",gps->satellite_track);
	}
	//���Ƕ�λ���� GI200ר��
	if (result[8].Data)
	{
		gps->Gps_Precision = atof((const char*)result[8].Data);
	}
	//����
	if (result[9].Data)
	{
		gps->height = atof((const char*)result[9].Data);
	}
	//ˮƽ��ȷ��
	if (result[11].Data)
	{
		tmp = atof((const char*)result[11].Data);
		tmp *= 10;
		gps->horizon_accuracy = (u16)tmp;
	}
	//����ӳ�
	if (result[13].Data)
	{
		gps->age = atof((const char*)result[13].Data);
	}
	//��ϵ���ѵ��ʱ��
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
*               ����������
***************************************************************************************************/
 bool GPATT_msg_anylise(DataNode result[], u32 count, GpsSyncType *gps)
{

	double tmp = 0.0;

	if (count == 0 || count != 28)
	{
		return FALSE;
	}


	//������
	if (result[1].Data)
	{
		tmp = atof((const char*)result[1].Data);
		tmp *= 100;
		gps->pitch = (u16)tmp;
	}
	//�����
	if (result[3].Data)
	{
		tmp = atof((const char*)result[3].Data);
		tmp *= 100;
		gps->roll = (u16)tmp;
	}
	//ƫ����
	if (result[5].Data)
	{
		tmp = atof((const char*)result[5].Data);
		tmp *= 100;
		gps->yaw = (u16)tmp;
	}

	//����汾��
	if (result[7].Data)
	{
		gps->softversion = atof((const char*)result[7].Data);
	}

	//Ĭ�ϴ򿪹��Ե���  1���򿪣�0���ر�
	if (result[11].Data)
	{
		gps->ins = atof((const char*)result[11].Data);
	}

	//�㷨״̬��־
	if (result[14].Data)
	{
		gps->State_Flag = atof((const char*)result[14].Data);
	}

	//ѡ��GPS+BD/GPS+Glonass B:GPS+BD,G:GPS+Glonass
	if (result[17].Data)
	{
		gps->bd_flag = atof((const char*)result[17].Data);
	}

	//��̬��־λ  1����ֹ��0������̬
	if (result[19].Data)
	{
		gps->StaticFlag = atof((const char*)result[19].Data);
	}
	//�û���� 1����ͨ�û���X�������û�	
	if (result[20].Data)
	{
		gps->Uer_Code = atof((const char*)result[20].Data);
	}

	//�Ƿ����flash��װ�Ǳ�־ 1�������ô洢��װ��  2:���ô洢��װ��
	if (result[21].Data)
	{
		gps->Angle_Select = atof((const char*)result[21].Data);
	}

	//�����ڱ����������λ�õı�־ 1���г���λ�ü�¼�ɹ�  0���޼�¼
	if (result[22].Data)
	{
		gps->Save_Gps_Flag = atof((const char*)result[22].Data);
	}

	//��װ��ѡ��ͨ�� 
	if (result[23].Data)
	{
		gps->ALocK_Channel = atof((const char*)result[23].Data);
	}

	//�Ƿ�̶���װ��־ 1�������̶���װ��0����������Ӧ��װ
	if (result[24].Data)
	{
		gps->Angle_Lock_Flag = atof((const char*)result[24].Data);
	}

	//UBI 1->8
	if (result[25].Data)
	{
		gps->UBI_On_Flag = atof((const char*)result[25].Data);
	}

	//���е��� 0��ƽ̹���Σ�1��ɽ�ǣ����ư��ţ�
	if (result[26].Data)
	{
		gps->City_Type = atof((const char*)result[26].Data);
	}
	//��ʻ���  ddd.mm ����λ����������9999����
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
*\Description   ��������������տɼ���������
*\Parameter     gps
*\Return        bool
*\Note
*\Log           2018.10.23    Ver 1.0    Job
����������
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

extern rt_mq_t mq_gps_data;//GPS���ݽ������͵���Ϣ����
//GPS���ݴ洢
/***************************************************************************************************
*\Function      GPS_Data_Storage
*\Description   
*\Parameter     gps  
*\Return        bool  
*\Note          
*\Log           2019.05.10    Ver 1.0    Jerry
*               ����������
***************************************************************************************************/
bool GPS_Data_Storage(GpsSyncType *gps)
{
	int result;
	float data_trans;//

	//��ǰGPS������GPS���������ݴ洢
	if(gps->GPS_Master_Flag==0)
	{
		return FALSE;
	}
	
	//gps ��λ��־λΪV ��Ч��λ��Ϣ �����д洢
	if(gps->valid_flag==0)
	{
		return FALSE;
	}
	
	//��ʼ�洢����
	//�洢 ������ ʱ���� ���� ��γ��
	gps_buf_send[0]=gps->InRTC.Year;
	gps_buf_send[1]=gps->InRTC.Month;
	gps_buf_send[2]=gps->InRTC.Day;
	gps_buf_send[3]=gps->InRTC.Hour;
	gps_buf_send[4]=gps->InRTC.Minute;
	gps_buf_send[5]=gps->InRTC.Second;
	gps_buf_send[6]=(u8)gps->mSecond;//��8λ
	gps_buf_send[7]=(u8)(gps->mSecond >> 8);//��8λ
	data_trans=(float)gps->longitude;
	memcpy(&gps_buf_send[8], &data_trans, sizeof(data_trans));//����
	data_trans=(float)gps->latitude;
	memcpy(&gps_buf_send[12], &data_trans, sizeof(data_trans));//γ��
	result=rt_mq_send(mq_gps_data,gps_buf_send,sizeof(gps_buf_send));
	
	return TRUE;
}


//��GPS ѡ�� ÿS ����һ���ж� 
/***************************************************************************************************
*\Function      GPS_Master_Select
*\Description   
*\Parameter       
*\Return        bool  
*\Note          
*\Log           2019.05.10    Ver 1.0    Jerry
*               ����������
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
	
	//�Ա�F9P GI200���µ����� 
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
*               ����������
***************************************************************************************************/
void rt_thread_gps_entry(void* parameter)
{	
	//10s�ȶ�һ��gps
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
*               ����������
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


