
#include "threadinit.h"
#include "app_board.h"

motor485_type motor485;

//���� ����ļ�����14���ͼ��ٱ�1
u8 sendbuf1[]={0x01,0x10,0x00,0x73,0x00,0x02,0x04,0x00,0x0e,0x00,0x0a,0x55,0x5a};
// ������ȡ ��ת ���� ���ת�� ת�ٱ���
u8 sendbuf2[]={0x01,0x03,0x00,0x32,0x00,0x04,0xE5,0xC6};


 void motor485_thread_entry(void* parameter)
{
	rt_err_t err;
	
	//���� ����ļ�����14���ͼ��ٱ�1
	uart3SendStr(sendbuf1,13);
	rt_thread_delay(2);
	//���� ����ļ�����14���ͼ��ٱ�1
	uart3SendStr(sendbuf1,13);

	while(1)
	{
    rt_thread_delay(100);
		/*
		485 ѯ�� ���ʵʱ��Ϣ
		*/
		motor485.rec_i=0; // ���ռ�������
		uart3SendStr(sendbuf2,8); // ������ȡ ��ת ���� ���ת�� ת�ٱ���
		 
 } 

}
