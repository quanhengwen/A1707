#ifndef __THREADINIT_H__
#define __THREADINIT_H__

#include <rtthread.h>



void Thread_Init(void);
void Sem_Init(void);
void key_thread_entry(void* parameter);

 void adc_thread_entry(void* parameter);


#endif

