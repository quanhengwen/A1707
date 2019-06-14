

#include "threadinit.h"


/********�߳̿��ƿ�*******/
//STM32F429 һ�� 256KB �� SRAM

ALIGN(RT_ALIGN_SIZE)                // ���� Ϊ DWORD 

//  �߳� ������⣻
static struct rt_thread key_thread;//�߳̿��ƿ�
static rt_uint8_t rt_key_thread_stack[256];//�߳�ջ  ��СΪ 1024*4=4k  DWORD 
#define  rt_key_prio 10


//  �߳� ������⣻
static struct rt_thread adc_thread;//�߳̿��ƿ�
static rt_uint8_t rt_adc_thread_stack[1024];//�߳�ջ  ��СΪ 1024*4=4k  DWORD 
#define  rt_adc_prio 11



/********�ź������ƿ�********************/
//struct rt_semaphore do_key_sem;
//struct rt_semaphore do_camera_sem;


/*******������*************/
//rt_mutex_t  file_spimutex;


/*********����*************/
//rt_mailbox_t   lora_send_MB;       //�洢��������
//rt_mailbox_t   lora_TARsend_MB;       //�洢��������

void Thread_Init()
{
//	rt_thread_t tid;
	//�ź�����ʼ��
//	rt_thread_t tid;
  Sem_Init();	
		 								 						 
								 
// �����߳�
    rt_thread_init(&key_thread,                		//�߳̿��ƿ�
                   "key",                       		//�߳����֣���shell������Կ���
                   key_thread_entry,           	 	//�߳���ں���
                   RT_NULL,                     		//�߳���ں�������
                   &rt_key_thread_stack[0],         	//�߳�ջ��ʼ��ַ
                   sizeof(rt_key_thread_stack),     	//�߳�ջ��С
                   rt_key_prio,    								//�̵߳����ȼ�
                   20);                         		//�߳�ʱ��Ƭ
     rt_thread_startup(&key_thread);             		//�����߳� 
									 
									 
									 // �����߳�
    rt_thread_init(&adc_thread,                		//�߳̿��ƿ�
                   "adc",                       		//�߳����֣���shell������Կ���
                   adc_thread_entry,           	 	//�߳���ں���
                   RT_NULL,                     		//�߳���ں�������
                   &rt_adc_thread_stack[0],         	//�߳�ջ��ʼ��ַ
                   sizeof(rt_adc_thread_stack),     	//�߳�ջ��С
                   rt_adc_prio,    								//�̵߳����ȼ�
                   20);                         		//�߳�ʱ��Ƭ
     rt_thread_startup(&adc_thread);             		//�����߳� 
					 					 								 
}

void Sem_Init()
{
rt_err_t err;

/********�ź�����ʼ��*****************/

	/* 
	//��ʼ���ź�������ʼֵ��0
err = rt_sem_init(&do_key_sem, "do_key_sem", 0, RT_IPC_FLAG_FIFO);
	if(err != RT_EOK)
{
		rt_kprintf("led thread: sem take no ok!\r\n");
}


//	��ʼ���ź�������ʼֵ��0 
err = rt_sem_init(&do_camera_sem, "do_camera_sem", 0, RT_IPC_FLAG_FIFO);
	if(err != RT_EOK)
{
		rt_kprintf("camera thread: sem take no ok!\r\n");
}

//������ ��ʼ��
file_spimutex = rt_mutex_create("file_spimutex", RT_IPC_FLAG_FIFO);


// �����ʼ�� 
lora_send_MB = rt_mb_create("lora_send_MB", 20, RT_IPC_FLAG_FIFO);
if (lora_send_MB == RT_NULL)
{
		rt_kprintf("lora_send_MB Mailbox init fail!\n");
		rt_free(lora_send_MB);
}

// �����ʼ�� 
lora_TARsend_MB = rt_mb_create("lora_TARsend_MB", 20, RT_IPC_FLAG_FIFO);
if (lora_TARsend_MB == RT_NULL)
{
		rt_kprintf("lora_TARsend_MB Mailbox init fail!\n");
		rt_free(lora_TARsend_MB);
}

*/
}










