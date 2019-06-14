/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_stepper_motor.c
*\Description   两相四线步进电机驱动
*\Note          
*\Log           2019.04.17    Ver 1.0    杨贝贝
*               创建文件。
***************************************************************************************************/
#include "drv_stepper_motor.h"
#include "app_board.h"





void stepper_timer_callback(drv_timer_type *dev)
{
	Stepper_motor_device_type* step_dev = (Stepper_motor_device_type *)dev->user_data;
	
	/*反转*/
	if(step_dev->forward_backward_flag)
	{
		switch(step_dev->pulse_state)
		{
			
			case 0:
				GPIO_SetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
				GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
				GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
				GPIO_SetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
			
				step_dev->pulse_state = 1;
			
			break;	

			case 1:
				GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
				GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
				GPIO_SetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
				GPIO_SetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
			
				step_dev->pulse_state = 2;
			
			break;

			case 2:
				GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
				GPIO_SetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
				GPIO_SetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
				GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
			
				step_dev->pulse_state = 3;
			
			break;		
			
			case 3:
				GPIO_SetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
				GPIO_SetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
				GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
				GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
			
				step_dev->pulse_state = 0;
			
				/*一圈后，发送信号量*/
				rt_sem_release(step_dev->sem_one_step_complete);
			
			break;
						
			default:
				step_dev->pulse_state = 0;
			break;
			
		}		
	}
	else
	{
		/*正转*/
		switch(step_dev->pulse_state)
		{
			case 0:
				GPIO_SetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
				GPIO_SetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
				GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
				GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
			
				step_dev->pulse_state = 1;
			
			break;
			
			case 1:
				GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
				GPIO_SetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
				GPIO_SetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
				GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
			
				step_dev->pulse_state = 2;
			
			break;
			
			case 2:
				GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
				GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
				GPIO_SetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
				GPIO_SetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
			
				step_dev->pulse_state = 3;
			break;
			
			case 3:
				GPIO_SetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
				GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
				GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
				GPIO_SetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
			
				step_dev->pulse_state = 0;
				/*一圈后，发送信号量*/
				rt_sem_release(step_dev->sem_one_step_complete);
			
			break;
			
			default:
				step_dev->pulse_state = 0;
			break;
			
		}
	}		
}


/***************************************************************************************************
*\Function      gpio_rcc_enable
*\Description   
*\Parameter     gpio
*\Return        void
*\Note          
*\Log           2016.07.11    Ver 1.0    Job
*               创建函数。
***************************************************************************************************/
static void gpio_rcc_enable(GPIO_TypeDef* gpio)
{
    //使能端口时钟
    switch((u32)gpio)
    {
    case GPIOA_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        break;
    case GPIOB_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        break;
    case GPIOC_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        break;
    case GPIOD_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
        break;
    case GPIOE_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
        break;
    case GPIOF_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
        break;
    case GPIOG_BASE:
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
        break;
    default:
        rt_kprintf("gpio clk error!\n");
        break;
    }
}

/***************************************************************************************************
*\Function      stepper_to_zero
*\Description   将状态清零
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.17    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void stepper_to_zero(Stepper_motor_device_type* step_dev)
{
	GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
	GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
	GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
	GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
}

/***************************************************************************************************
*\Function      stepper_motor_delay
*\Description   脉冲间延时函数
*\Parameter     
*\Return        void
*\Note          
*\Log           2019.04.17    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void stepper_motor_delay(Stepper_motor_device_type* step_dev)
{
	uint32_t i;
	i = step_dev->speed;
	while(i--);
//	rt_thread_delay(1);
}

/***************************************************************************************************
*\Function      step_8_beat_one_pulse
*\Description   8节拍步进电机一个脉冲控制
*\Parameter     direction 方向，0：正转，1：反转
*\Return        void
*\Note          
*\Log           2019.04.17    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
int tmp;
void stepper_8_beat_one_pulse(Stepper_motor_device_type* step_dev, uint8_t direction)
{
	
	
	if(direction == 0)
	{
		step_dev->pulse_num++;
	}
	else
	{
		step_dev->pulse_num--;
	}
	
	tmp = step_dev->pulse_num % 8;
	
	switch(abs(step_dev->pulse_num % 8))
	{
		case 0: // 1000
			GPIO_SetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
			GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
			GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
			GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
		break;
		
		case 1: // 1010
			GPIO_SetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
			GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
			GPIO_SetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
			GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
		break;
		
		case 2: // 0010
			GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
			GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
			GPIO_SetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
			GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
		break;
		
		case 3: //0110
			GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
			GPIO_SetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
			GPIO_SetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
			GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
		break;
		
		case 4: //0100
			GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
			GPIO_SetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
			GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
			GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
		break;
		
		case 5: //0101
			GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
			GPIO_SetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
			GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
			GPIO_SetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
		break;
		
		case 6: //0001
			GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
			GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
			GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
			GPIO_SetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
		break;
		
		case 7: // 1001
			GPIO_SetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
			GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
			GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
			GPIO_SetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
		break;
	}
	
	stepper_motor_delay(step_dev);
}



/***************************************************************************************************
*\Function      step_8_beat_forward_ctrl
*\Description   8节拍正转控制
*\Parameter     pulse_num 需要转的脉冲数
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void stepper_8_beat_forward_ctrl(Stepper_motor_device_type* step_dev, uint16_t pulse_num)
{
	uint16_t cnt;
	
	for(cnt = 0; cnt < pulse_num; cnt++)
	{
		stepper_8_beat_one_pulse(step_dev, 0);
	}	
}

/***************************************************************************************************
*\Function      step_8_beat_forward_ctrl
*\Description   8节拍反转控制
*\Parameter     pulse_num 需要转的脉冲数
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void stepper_8_beat_backforward_ctrl(Stepper_motor_device_type* step_dev, uint16_t pulse_num)
{
	uint16_t cnt;
	
	for(cnt = 0; cnt < pulse_num; cnt++)
	{
		stepper_8_beat_one_pulse(step_dev, 1);
	}	
}

/***************************************************************************************************
*\Function      step_4_beat_one_pulse
*\Description   4节拍步进电机一个脉冲控制
*\Parameter     direction 方向，0：正转，1：反转
*\Return        void
*\Note          
*\Log           2019.04.17    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void stepper_4_beat_one_pulse(Stepper_motor_device_type* step_dev, uint8_t direction)
{
	
	
	if(direction == 0)
	{
		step_dev->pulse_num++;
	}
	else
	{
		step_dev->pulse_num--;
	}
	
	tmp = step_dev->pulse_num % 4;
	
	switch(abs(step_dev->pulse_num % 4))
	{		
		case 0: // 1100
			GPIO_SetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
			GPIO_SetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
			GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
			GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
		break;
		
		case 1: // 0110
			GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
			GPIO_SetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
			GPIO_SetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
			GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
		break;
		
		case 2: //0011
			GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
			GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
			GPIO_SetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
			GPIO_SetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
		break;
		
		case 3: // 1001
			GPIO_SetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
			GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
			GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
			GPIO_SetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
		break;
		
//		case 0: // 1001
//			GPIO_SetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
//			GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
//			GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
//			GPIO_SetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
//		break;
//		
//		case 1: // 1010
//			GPIO_SetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
//			GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
//			GPIO_SetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
//			GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
//		break;
//		
//		case 2: // 0110
//			GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
//			GPIO_SetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
//			GPIO_SetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
//			GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
//		break;
//		
//		case 3: //0101
//			GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
//			GPIO_SetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
//			GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
//			GPIO_SetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
//		break;
	}
	
	stepper_motor_delay(step_dev);
}


void motor_Z(Stepper_motor_device_type* step_dev)	// 电机 正转
{
	GPIO_SetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
	GPIO_SetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
	GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
	GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);

	stepper_motor_delay(step_dev);
	
	GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
	GPIO_SetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
	GPIO_SetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
	GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);

	stepper_motor_delay(step_dev);
	
	GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
	GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
	GPIO_SetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
	GPIO_SetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);

	stepper_motor_delay(step_dev);
	
	GPIO_SetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
	GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
	GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
	GPIO_SetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);

	stepper_motor_delay(step_dev);
}


//void motor_F(u8 vn)	//电机 反转
//{
//	motor_A=1; motor_B=0; motor_AP=0; motor_BP=1; // + - - +	
//	rt_thread_delay(vn);
//	
//	motor_A=0; motor_B=0; motor_AP=1; motor_BP=1; // - - + + 
//	rt_thread_delay(vn);
//	
//	motor_A=0; motor_B=1; motor_AP=1; motor_BP=0; // - + + - 
//	rt_thread_delay(vn);
//	
//	motor_A=1; motor_B=1; motor_AP=0; motor_BP=0; // + + - -
//	rt_thread_delay(vn);
//}


/***************************************************************************************************
*\Function      step_4_beat_forward_ctrl
*\Description   4节拍正转控制
*\Parameter     pulse_num 需要转的脉冲数
*\Return        void
*\Note          750是实际的一圈
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
uint8_t stepper_4_beat_forward_ctrl(Stepper_motor_device_type* step_dev, uint32_t total_num)
{
	uint32_t cnt = 0;
	rt_err_t rt_err;
	
	step_dev->pulse_state = 0;
	step_dev->forward_backward_flag = 0;
	stepper_to_zero(step_dev);
	
	turn_on_timer(step_dev->timer);
	
	while(cnt <= total_num)
	{
		rt_err = rt_sem_take(step_dev->sem_one_step_complete, 100);
		if(rt_err == RT_EOK)
		{
			cnt++;
		}
		else
		{
			turn_off_timer(step_dev->timer);			
			step_dev->pulse_state = 0;
			rt_kprintf("step timeout\r\n");
			return 1;
		}		
		
	}
	turn_off_timer(step_dev->timer);
	stepper_to_zero(step_dev);
	step_dev->pulse_state = 0;
	
	return 0;
}


/***************************************************************************************************
*\Function      step_4_beat_forward_ctrl
*\Description   4节拍反转控制
*\Parameter     pulse_num 需要转的脉冲数
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
uint8_t stepper_4_beat_backforward_ctrl(Stepper_motor_device_type* step_dev, uint16_t total_num)
{
	uint32_t cnt = 0;
	rt_err_t rt_err;
	
	step_dev->pulse_state = 0;
	/*反转*/
	step_dev->forward_backward_flag = 1;
	stepper_to_zero(step_dev);
	
	turn_on_timer(step_dev->timer);
	
	while(cnt <= total_num)
	{
		rt_err = rt_sem_take(step_dev->sem_one_step_complete, 100);
		if(rt_err == RT_EOK)
		{
			cnt++;
		}
		else
		{
			turn_off_timer(step_dev->timer);			
			step_dev->pulse_state = 0;
			
			return 1;
		}		
		
	}
	
	stepper_to_zero(step_dev);
	step_dev->pulse_state = 0;
	
	return 0;
}

/***************************************************************************************************
*\Function      stepper_stop
*\Description   停止步进电机转动
*\Parameter     pulse_num 需要转的脉冲数
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void stepper_stop(Stepper_motor_device_type* step_dev)
{
	uint32_t cnt = 0;
	rt_err_t rt_err;
	
	turn_off_timer(step_dev->timer);
	
	stepper_to_zero(step_dev);		

	step_dev->pulse_state = 0;
}


/***************************************************************************************************
*\Function      step_motor_on
*\Description   打开步进电机驱动器
*\Parameter    
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void stepper_motor_on(Stepper_motor_device_type* step_dev)
{
	GPIO_SetBits(step_dev->en_pin->gpio, step_dev->en_pin->gpio_pin);
}

/***************************************************************************************************
*\Function      step_motor_off
*\Description   关闭步进电机驱动器
*\Parameter    
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void stepper_motor_off(Stepper_motor_device_type* step_dev)
{
	GPIO_ResetBits(step_dev->en_pin->gpio, step_dev->en_pin->gpio_pin);
}

/***************************************************************************************************
*\Function      stepper_motor_init
*\Description   步进电机控制初始化
*\Parameter    
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void stepper_motor_init(Stepper_motor_device_type* step_dev)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
    if (step_dev == RT_NULL || step_dev->out1 == RT_NULL || step_dev->out2 == RT_NULL
		 || step_dev->out3 == RT_NULL || step_dev->out4 == RT_NULL)
    {
        rt_kprintf("Stepper motor NULL!\n");
    }

    step_dev->lock = rt_mutex_create("step_Mut", RT_IPC_FLAG_FIFO);
    if (step_dev->lock == RT_NULL)
    {
        rt_kprintf("mpu mutex create fail!\n");
    }
	
	if(step_dev->speed == 0)
	{
		step_dev->speed = 10000;
	}
	
    /*使能端口时钟*/
	gpio_rcc_enable(step_dev->out1->gpio);
	gpio_rcc_enable(step_dev->out2->gpio);
	gpio_rcc_enable(step_dev->out3->gpio);
	gpio_rcc_enable(step_dev->out4->gpio);
	

	GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
	GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
	GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
	GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);
	
    /*引脚初始化,pp模式*/
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
	
    GPIO_InitStructure.GPIO_Pin = step_dev->out1->gpio_pin;
    GPIO_Init(step_dev->out1->gpio, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin = step_dev->out2->gpio_pin;
    GPIO_Init(step_dev->out2->gpio, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin = step_dev->out3->gpio_pin;
    GPIO_Init(step_dev->out3->gpio, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin = step_dev->out4->gpio_pin;
    GPIO_Init(step_dev->out4->gpio, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = step_dev->en_pin->gpio_pin;
    GPIO_Init(step_dev->en_pin->gpio, &GPIO_InitStructure);

	GPIO_ResetBits(step_dev->out1->gpio, step_dev->out1->gpio_pin);
	GPIO_ResetBits(step_dev->out2->gpio, step_dev->out2->gpio_pin);
	GPIO_ResetBits(step_dev->out3->gpio, step_dev->out3->gpio_pin);
	GPIO_ResetBits(step_dev->out4->gpio, step_dev->out4->gpio_pin);

	/*使能步进电机驱动器*/
	stepper_motor_on(step_dev);
	
	/*初始化硬件定时器*/
	init_timer(step_dev->timer);
	
	step_dev->timer->user_data = step_dev;
	
	set_timer_callback(step_dev->timer,stepper_timer_callback);
	
	step_dev->sem_one_step_complete = rt_sem_create("step_sem", 0, RT_IPC_FLAG_FIFO);

}


