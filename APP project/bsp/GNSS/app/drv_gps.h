
/***************************************************************************************************
*                    (c) Copyright 1992-2019 Syncretize technologies co.,ltd.
*                                       All Rights Reserved
*
*\File          drv_gps.h
*\Description   
*\Note          
*\Log           2019.03.28    Ver 1.0    Jerry
*               �����ļ���
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


/*GPSʱ��ͬ������*/
typedef struct Gps_Sync_type_st
{
	u8 GPS_Master_Flag;//GPS����־λ 0�� 1�� Ĭ��f9pΪ�� ��GPS�����ݽ��д洢
//    USART_TypeDef* uart;
//    GpioType*   TxGpio; //uart���ͽ�
//    GpioType*   RxGpio; //uart���ս�
//    GpioType*   PulseGpio;//�������ź�
//    switch_type*RST;//��λ
//    switch_type*PWCtl;//��Դ����
//    u32         baudrate;//�����ʣ�����ʱ����ݲ����ʼ���ȴ�ʱ��
//    /*�������ʼ����ֵ*/
//    u8          rxbuf[GPS_MAX_MSG_SIZE];//uart���ջ�����
//    u32         rx_write;//д��ָ��
//    u8          crc;//�������
    TIME InRTC;//����ʱ��
    u32         rtc_tick;//�����ɹ�RTC��tick
//    u32         tick_last;//���ͬ���ɹ���tick
//    u8          gps_state;//����״̬

//    u8          sync_flag;//ʱ��ͬ����־ 1��ʾҪ����ͬ��
//    u32         uart_nums;//���ڽ�������
//    //��γ�ȵ���Ϣ
////    u8 valid_flag;//��λ״̬ 0x01��Ч
////    u8 lat_flag;//γ������ 0 ��γ��1��γ
////    double latitude;//γ��
////    u8 lon_flag;//�������� 0 ���� 1 ����
////    double longitude;//����
//    u8 gps_cmd[3];//�������RMC GGA GSV��
   
	
	u16 mSecond;//gps ms
	//GPRMC GNRMC
    u8 valid_flag;//��λ״̬ 0x01��Ч
    u8 lat_flag;//γ������ 0 ��γ��1��γ
    double latitude;//γ��
    u8 lon_flag;//�������� 0 ���� 1 ����
    double longitude;//����
    u16 altitude;//���� ��
    u16 speed;//�ٶ� 0~9999 ��λ�� ����10��
    u16 azimuth;//��λ�� 0~3600 ����10��
    u16 magnetic_dec;//��ƫ�� 0~1800 ����10��
    u8 magnetic_flag;// 0 �� 1 ��
    u8 mode;// 0 �Զ���1 ��֣� 2 ���⣬3 ������Ч
    u8 satellites;//�ɼ�������	
	
	//GPGGA GNGGA
	u8 QF;//��״̬ 0��Ч 1���㶨λ 2α���� 6����Ե����⣨6GI200ר�ã�
    u8 satellite_track;//�������� GGA ���� 7
	u8 satellite_buf[64];//����������������ѡ���ĸ�gpsΪ��
	u8 satellite_count;//�������м���
	u16 Gps_Precision;//���Ƕ�λ���� GI200ר��
	u16 horizon_accuracy;//ˮƽ��ȷ��
	u16 age;//����ӳ�
	u16 InsTime;//��ϵ���ѵ��ʱ��
	double height;//�����뺣ƽ��ĸ߶�
	//���ˮ׼��߶�
	//���GPS��������
	//��ֲο���վ���

	//GPATT
	u16 pitch;//������
	u16 roll;//�����
	u16 yaw;//ƫ����
	u32 softversion;//����汾��
	u8 ins;//Ĭ�ϴ򿪹��Ե���  1���򿪣�0���ر�
	u8 State_Flag;//�㷨״̬��־
	u8 bd_flag;//ѡ��GPS+BD/GPS+Glonass B:GPS+BD,G:GPS+Glonass
	u8 StaticFlag;//��̬��־λ  1����ֹ��0������̬
	u8 Uer_Code;//�û���� 1����ͨ�û���X�������û�
	u8 Angle_Select;//�Ƿ����flash��װ�Ǳ�־ 1�������ô洢��װ��  2:���ô洢��װ��
	u8 Save_Gps_Flag;//�����ڱ����������λ�õı�־ 1���г���λ�ü�¼�ɹ�  0���޼�¼
	u8 ALocK_Channel;//��װ��ѡ��ͨ�� 
	u8 Angle_Lock_Flag;//�Ƿ�̶���װ��־ 1�������̶���װ��0����������Ӧ��װ
	u8 UBI_On_Flag;//UBI 1->8
	u8 City_Type;//���е��� 0��ƽ̹���Σ�1��ɽ�ǣ����ư��ţ�
	double Mileage;//��ʻ���  ddd.mm ����λ����������9999����

	//GPGSV
	u8 satellite_view;//�������� GSV ���� 3

    u8    ErrorFlag;//ĿǰΪ1ͬ���������������������⣬2 �����޽�������
}GpsSyncType;

/*�����ýṹ*/
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


