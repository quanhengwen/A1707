
#include "threadinit.h"
#include "app_board.h"

//#include "stm32f4xx_hal_rcc_ex.h"
//Ïß³Ì key
 void key_thread_entry(void* parameter)
{
	rt_err_t err;


while(1)
{
		LED_SYS_H;
    rt_thread_delay(100);
    LED_SYS_L;
	  rt_thread_delay(100);
//	err=rt_sem_release(&do_key_sem);		
		if(err != RT_EOK)
		{
			
		}
		 
 } 

}
