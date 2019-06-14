

#include "threadinit.h"


/********线程控制块*******/
//STM32F429 一共 256KB 的 SRAM

ALIGN(RT_ALIGN_SIZE)                // 代表 为 DWORD 

//  线程 按键检测；
static struct rt_thread key_thread;//线程控制块
static rt_uint8_t rt_key_thread_stack[256];//线程栈  大小为 1024*4=4k  DWORD 
#define  rt_key_prio 10


//  线程 按键检测；
static struct rt_thread adc_thread;//线程控制块
static rt_uint8_t rt_adc_thread_stack[1024];//线程栈  大小为 1024*4=4k  DWORD 
#define  rt_adc_prio 11



/********信号量控制块********************/
//struct rt_semaphore do_key_sem;
//struct rt_semaphore do_camera_sem;


/*******互斥锁*************/
//rt_mutex_t  file_spimutex;


/*********邮箱*************/
//rt_mailbox_t   lora_send_MB;       //存储按键邮箱
//rt_mailbox_t   lora_TARsend_MB;       //存储按键邮箱

void Thread_Init()
{
//	rt_thread_t tid;
	//信号量初始化
//	rt_thread_t tid;
  Sem_Init();	
		 								 						 
								 
// 创建线程
    rt_thread_init(&key_thread,                		//线程控制块
                   "key",                       		//线程名字，在shell里面可以看到
                   key_thread_entry,           	 	//线程入口函数
                   RT_NULL,                     		//线程入口函数参数
                   &rt_key_thread_stack[0],         	//线程栈起始地址
                   sizeof(rt_key_thread_stack),     	//线程栈大小
                   rt_key_prio,    								//线程的优先级
                   20);                         		//线程时间片
     rt_thread_startup(&key_thread);             		//启动线程 
									 
									 
									 // 创建线程
    rt_thread_init(&adc_thread,                		//线程控制块
                   "adc",                       		//线程名字，在shell里面可以看到
                   adc_thread_entry,           	 	//线程入口函数
                   RT_NULL,                     		//线程入口函数参数
                   &rt_adc_thread_stack[0],         	//线程栈起始地址
                   sizeof(rt_adc_thread_stack),     	//线程栈大小
                   rt_adc_prio,    								//线程的优先级
                   20);                         		//线程时间片
     rt_thread_startup(&adc_thread);             		//启动线程 
					 					 								 
}

void Sem_Init()
{
rt_err_t err;

/********信号量初始化*****************/

	/* 
	//初始化信号量，初始值是0
err = rt_sem_init(&do_key_sem, "do_key_sem", 0, RT_IPC_FLAG_FIFO);
	if(err != RT_EOK)
{
		rt_kprintf("led thread: sem take no ok!\r\n");
}


//	初始化信号量，初始值是0 
err = rt_sem_init(&do_camera_sem, "do_camera_sem", 0, RT_IPC_FLAG_FIFO);
	if(err != RT_EOK)
{
		rt_kprintf("camera thread: sem take no ok!\r\n");
}

//互斥锁 初始化
file_spimutex = rt_mutex_create("file_spimutex", RT_IPC_FLAG_FIFO);


// 邮箱初始化 
lora_send_MB = rt_mb_create("lora_send_MB", 20, RT_IPC_FLAG_FIFO);
if (lora_send_MB == RT_NULL)
{
		rt_kprintf("lora_send_MB Mailbox init fail!\n");
		rt_free(lora_send_MB);
}

// 邮箱初始化 
lora_TARsend_MB = rt_mb_create("lora_TARsend_MB", 20, RT_IPC_FLAG_FIFO);
if (lora_TARsend_MB == RT_NULL)
{
		rt_kprintf("lora_TARsend_MB Mailbox init fail!\n");
		rt_free(lora_TARsend_MB);
}

*/
}










