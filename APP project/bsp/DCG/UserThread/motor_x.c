
#include "threadinit.h"
#include "app_board.h"

u8  Motor_cotorl=0;
u8  Motor_state =0;
void Stop_low0()
{	
	 while(CCR_Va0>0&&Motor_cotorl==0)
	 {
		 PWM_out(CCR_Va0);
		 CCR_Va0--;
		 rt_thread_delay(5);
	 }
	 if(CCR_Va0==0) Stop_motor;
	 Motor_state=0;
}

void Stop_fast0()
{	
	 PWM_out(0); //PWM 输出 0
	 Stop_motor;  //刹车信号
	 Motor_state=0;
}


//线程 key
 void motor_thread_entry(void* parameter)
{
	rt_err_t err;
	u32 do_motor_value;

 rt_thread_delay(100);
	while(1)
	{
    err=rt_mb_recv(do_motor_sem, (rt_uint32_t*)&do_motor_value, RT_WAITING_FOREVER); //   等待		
		if(err != RT_EOK)
		{
			continue;
		}
		 
		switch (do_motor_value)
		{
		  case Stop_low:
				Stop_low0();
			break;
		  case  Stop_fast:
			  Stop_fast0();
			 break;
			default : break;
		}
 } 

}
