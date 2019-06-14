#include "threadinit.h"
#include "app_board.h"

/***************************************************************************************************
*\Function      max9611_thread_entry
*\Description   
*\Parameter     parameter
*\Return        void
*\Note          
*\Log           2019.01.28    Ver 1.0    baron
*               ´´½¨º¯Êý¡£
***************************************************************************************************/
void max9611_thread_entry(void* parameter)
{
	rt_thread_delay(100);//
	
	while(1)
	{
    max9611_data_read(&MAX9611, &max9611_data);
	  rt_thread_delay(100);				
	}

}