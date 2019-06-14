

#include "threadinit.h"


/********�߳̿��ƿ�*******/
//STM32F429 һ�� 256KB �� SRAM

ALIGN(RT_ALIGN_SIZE)                // ���� Ϊ DWORD 

//  �߳� ѭ�� ѯ�ʵ�������� 
static struct rt_thread motor485_thread;//�߳̿��ƿ�
static rt_uint8_t rt_motor485_thread_stack[256];//�߳�ջ  ��СΪ 1024*4=4k  DWORD 
#define  rt_motor485_prio 10
//motor_thread_entry(void* parameter);

static struct rt_thread motor_thread;//�߳̿��ƿ�
static rt_uint8_t rt_motor_thread_stack[256];//�߳�ջ  ��СΪ 1024*4=4k  DWORD 
#define  rt_motor_prio  2


static struct rt_thread motor_upinfor_thread;//�߳̿��ƿ�
static rt_uint8_t rt_motor_upinfor_thread_stack[256];//�߳�ջ  ��СΪ 1024*4=4k  DWORD 
#define  rt_motor_upinfor_prio  5



static struct rt_thread max9611_thread;//�߳̿��ƿ�
static rt_uint8_t rt_max9611_thread_stack[256];//�߳�ջ  ��СΪ 1024*4=4k  DWORD 
#define  rt_max9611_prio  24




/********�ź������ƿ�********************/
struct rt_semaphore motor485_sem;



/*******������*************/
//rt_mutex_t  file_spimutex;


/*********����*************/
rt_mailbox_t   do_motor_sem;       //�洢��������
//rt_mailbox_t   lora_TARsend_MB;       //�洢��������

void Thread_Init()
{
	//�ź�����ʼ��
  Sem_Init();	
		 								 						 								 
//  �߳� ѭ�� ѯ�ʵ�������� 
    rt_thread_init(&motor485_thread,                		//�߳̿��ƿ�
                   "motor485",                       		//�߳����֣���shell������Կ���
                   motor485_thread_entry,           	 	//�߳���ں���
                   RT_NULL,                     		//�߳���ں�������
                   &rt_motor485_thread_stack[0],         	//�߳�ջ��ʼ��ַ
                   sizeof(rt_motor485_thread_stack),     	//�߳�ջ��С
                   rt_motor485_prio,    								//�̵߳����ȼ�
                   20);                         		//�߳�ʱ��Ƭ
     rt_thread_startup(&motor485_thread);             		//�����߳� 
									 
	    rt_thread_init(&motor_thread,                		//�߳̿��ƿ�
                   "motor",                       		//�߳����֣���shell������Կ���
                   motor_thread_entry,           	 	//�߳���ں���
                   RT_NULL,                     		//�߳���ں�������
                   &rt_motor_thread_stack[0],         	//�߳�ջ��ʼ��ַ
                   sizeof(rt_motor_thread_stack),     	//�߳�ջ��С
                   rt_motor_prio,    								//�̵߳����ȼ�
                   20);                         		//�߳�ʱ��Ƭ
     rt_thread_startup(&motor_thread);             		//�����߳� 								 
									 
		 rt_thread_init(&motor_upinfor_thread,                		//�߳̿��ƿ�
                   "motor_upinfor",                       		//�߳����֣���shell������Կ���
                   motor_upinfor_thread_entry,           	 	//�߳���ں���
                   RT_NULL,                     		//�߳���ں�������
                   &rt_motor_upinfor_thread_stack[0],         	//�߳�ջ��ʼ��ַ
                   sizeof(rt_motor_upinfor_thread_stack),     	//�߳�ջ��С
                   rt_motor_upinfor_prio,    								//�̵߳����ȼ�
                   20);                         		//�߳�ʱ��Ƭ
     rt_thread_startup(&motor_upinfor_thread);             		//�����߳� 		

		 rt_thread_init(&max9611_thread,                		//�߳̿��ƿ�
                   "max9611",                       		//�߳����֣���shell������Կ���
                   max9611_thread_entry,           	 	//�߳���ں���
                   RT_NULL,                     		//�߳���ں�������
                   &rt_max9611_thread_stack[0],         	//�߳�ջ��ʼ��ַ
                   sizeof(rt_max9611_thread_stack),     	//�߳�ջ��С
                   rt_max9611_prio,    								//�̵߳����ȼ�
                   20);                         		//�߳�ʱ��Ƭ
     rt_thread_startup(&max9611_thread);             		//�����߳� 											 
					 					 								 
}

void Sem_Init()
{
rt_err_t err;

/********�ź�����ʼ��*****************/

// �����ʼ�� 
do_motor_sem = rt_mb_create("do_motor_sem", 20, RT_IPC_FLAG_FIFO);
if (do_motor_sem == RT_NULL)
{
		rt_kprintf("do_motor_sem fail!\n");
		rt_free(do_motor_sem);
}	 
	
	

//	��ʼ���ź�������ʼֵ��0 
err = rt_sem_init(&motor485_sem, "motor485_sem", 0, RT_IPC_FLAG_FIFO);
	if(err != RT_EOK)
{
		rt_kprintf("motor485_sem no ok!\r\n");
}
/*
//������ ��ʼ��
file_spimutex = rt_mutex_create("file_spimutex", RT_IPC_FLAG_FIFO);

// �����ʼ�� 
lora_TARsend_MB = rt_mb_create("lora_TARsend_MB", 20, RT_IPC_FLAG_FIFO);
if (lora_TARsend_MB == RT_NULL)
{
		rt_kprintf("lora_TARsend_MB Mailbox init fail!\n");
		rt_free(lora_TARsend_MB);
}

*/
}










