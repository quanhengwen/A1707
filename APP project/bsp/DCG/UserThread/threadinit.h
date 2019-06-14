#ifndef __THREADINIT_H__
#define __THREADINIT_H__

#include <rtthread.h>
#include "app_board.h"

extern rt_mailbox_t   do_motor_sem; 
extern struct rt_semaphore motor485_sem;

// motor_x
#define Stop_low   0x02  // »ºÂýÍ£³µ
#define Stop_fast  0x03  // ½ô¼±É²³µ


extern u8  Motor_cotorl;
extern u8  Motor_state;
// motor_x

// motor485_x

// motor485_x


void Thread_Init(void);
void Sem_Init(void);
void key_thread_entry(void* parameter);
void motor485_thread_entry(void* parameter);
void motor_thread_entry(void* parameter);

void motor_upinfor_thread_entry(void* parameter);

void max9611_thread_entry(void* parameter);

#endif

