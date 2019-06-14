

#include "threadinit.h"


/********线程控制块*******/
//STM32F429 一共 256KB 的 SRAM

ALIGN(RT_ALIGN_SIZE)                // 代表 为 DWORD 

//  线程 循环 询问电机驱动器 
static struct rt_thread motor485_thread;//线程控制块
static rt_uint8_t rt_motor485_thread_stack[256];//线程栈  大小为 1024*4=4k  DWORD 
#define  rt_motor485_prio 10
//motor_thread_entry(void* parameter);

static struct rt_thread motor_thread;//线程控制块
static rt_uint8_t rt_motor_thread_stack[256];//线程栈  大小为 1024*4=4k  DWORD 
#define  rt_motor_prio  2


static struct rt_thread motor_upinfor_thread;//线程控制块
static rt_uint8_t rt_motor_upinfor_thread_stack[256];//线程栈  大小为 1024*4=4k  DWORD 
#define  rt_motor_upinfor_prio  5



static struct rt_thread max9611_thread;//线程控制块
static rt_uint8_t rt_max9611_thread_stack[256];//线程栈  大小为 1024*4=4k  DWORD 
#define  rt_max9611_prio  24




/********信号量控制块********************/
struct rt_semaphore motor485_sem;



/*******互斥锁*************/
//rt_mutex_t  file_spimutex;


/*********邮箱*************/
rt_mailbox_t   do_motor_sem;       //存储按键邮箱
//rt_mailbox_t   lora_TARsend_MB;       //存储按键邮箱

void Thread_Init()
{
	//信号量初始化
  Sem_Init();	
		 								 						 								 
//  线程 循环 询问电机驱动器 
    rt_thread_init(&motor485_thread,                		//线程控制块
                   "motor485",                       		//线程名字，在shell里面可以看到
                   motor485_thread_entry,           	 	//线程入口函数
                   RT_NULL,                     		//线程入口函数参数
                   &rt_motor485_thread_stack[0],         	//线程栈起始地址
                   sizeof(rt_motor485_thread_stack),     	//线程栈大小
                   rt_motor485_prio,    								//线程的优先级
                   20);                         		//线程时间片
     rt_thread_startup(&motor485_thread);             		//启动线程 
									 
	    rt_thread_init(&motor_thread,                		//线程控制块
                   "motor",                       		//线程名字，在shell里面可以看到
                   motor_thread_entry,           	 	//线程入口函数
                   RT_NULL,                     		//线程入口函数参数
                   &rt_motor_thread_stack[0],         	//线程栈起始地址
                   sizeof(rt_motor_thread_stack),     	//线程栈大小
                   rt_motor_prio,    								//线程的优先级
                   20);                         		//线程时间片
     rt_thread_startup(&motor_thread);             		//启动线程 								 
									 
		 rt_thread_init(&motor_upinfor_thread,                		//线程控制块
                   "motor_upinfor",                       		//线程名字，在shell里面可以看到
                   motor_upinfor_thread_entry,           	 	//线程入口函数
                   RT_NULL,                     		//线程入口函数参数
                   &rt_motor_upinfor_thread_stack[0],         	//线程栈起始地址
                   sizeof(rt_motor_upinfor_thread_stack),     	//线程栈大小
                   rt_motor_upinfor_prio,    								//线程的优先级
                   20);                         		//线程时间片
     rt_thread_startup(&motor_upinfor_thread);             		//启动线程 		

		 rt_thread_init(&max9611_thread,                		//线程控制块
                   "max9611",                       		//线程名字，在shell里面可以看到
                   max9611_thread_entry,           	 	//线程入口函数
                   RT_NULL,                     		//线程入口函数参数
                   &rt_max9611_thread_stack[0],         	//线程栈起始地址
                   sizeof(rt_max9611_thread_stack),     	//线程栈大小
                   rt_max9611_prio,    								//线程的优先级
                   20);                         		//线程时间片
     rt_thread_startup(&max9611_thread);             		//启动线程 											 
					 					 								 
}

void Sem_Init()
{
rt_err_t err;

/********信号量初始化*****************/

// 邮箱初始化 
do_motor_sem = rt_mb_create("do_motor_sem", 20, RT_IPC_FLAG_FIFO);
if (do_motor_sem == RT_NULL)
{
		rt_kprintf("do_motor_sem fail!\n");
		rt_free(do_motor_sem);
}	 
	
	

//	初始化信号量，初始值是0 
err = rt_sem_init(&motor485_sem, "motor485_sem", 0, RT_IPC_FLAG_FIFO);
	if(err != RT_EOK)
{
		rt_kprintf("motor485_sem no ok!\r\n");
}
/*
//互斥锁 初始化
file_spimutex = rt_mutex_create("file_spimutex", RT_IPC_FLAG_FIFO);

// 邮箱初始化 
lora_TARsend_MB = rt_mb_create("lora_TARsend_MB", 20, RT_IPC_FLAG_FIFO);
if (lora_TARsend_MB == RT_NULL)
{
		rt_kprintf("lora_TARsend_MB Mailbox init fail!\n");
		rt_free(lora_TARsend_MB);
}

*/
}










