
#include "threadinit.h"
#include "app_board.h"

motor485_type motor485;

//设置 电机的极个数14，和减速比1
u8 sendbuf1[]={0x01,0x10,0x00,0x73,0x00,0x02,0x04,0x00,0x0e,0x00,0x0a,0x55,0x5a};
// 连续读取 堵转 故障 电机转速 转速倍数
u8 sendbuf2[]={0x01,0x03,0x00,0x32,0x00,0x04,0xE5,0xC6};


 void motor485_thread_entry(void* parameter)
{
	rt_err_t err;
	
	//设置 电机的极个数14，和减速比1
	uart3SendStr(sendbuf1,13);
	rt_thread_delay(2);
	//设置 电机的极个数14，和减速比1
	uart3SendStr(sendbuf1,13);

	while(1)
	{
    rt_thread_delay(100);
		/*
		485 询问 电机实时信息
		*/
		motor485.rec_i=0; // 接收计数清零
		uart3SendStr(sendbuf2,8); // 连续读取 堵转 故障 电机转速 转速倍数
		 
 } 

}
