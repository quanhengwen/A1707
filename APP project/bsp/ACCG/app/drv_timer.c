/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_timer.c
*\Description   
*\Note          
*\Log           2019.01.03    Ver 1.0    ���
*               �����ļ���
***************************************************************************************************/
#include "drv_timer.h"

//#define USE_TIM1
#define USE_TIM2
#define USE_TIM3
//#define USE_TIM4
//#define USE_TIM5
//#define USE_TIM6
//#define USE_TIM7
//#define USE_TIM8

drv_timer_type *timer_dev_TIM1;
drv_timer_type *timer_dev_TIM2;
drv_timer_type *timer_dev_TIM3;
drv_timer_type *timer_dev_TIM4;
drv_timer_type *timer_dev_TIM5;
drv_timer_type *timer_dev_TIM6;
drv_timer_type *timer_dev_TIM7;
drv_timer_type *timer_dev_TIM8;

/*
	����STM32F103���ܹ�ʹ����������Ķ�ʱ����TIM1��TIM1��TIM2��TIM3��TIM4��TIM5��TIM8����ֻ��ʹ��CH1��CH2
*/
/***************************************************************************************************
*\Function      gpio_rcc_enable
*\Description   
*\Parameter     gpio
*\Return        void
*\Note          
*\Log           2016.07.11    Ver 1.0    Job
*               ����������
***************************************************************************************************/
static void gpio_rcc_enable(GPIO_TypeDef* gpio)
{
    //ʹ�ܶ˿�ʱ��
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
*\Function      stm32_get_gpio_pinsource
*\Description   
*\Parameter     pin
*\Return        u32
*\Note          
*\Log           2015.09.28    Ver 1.0    �Ų�
*               ����������
***************************************************************************************************/
static u32 stm32_get_gpio_pinsource(u32 pin)
{
    u32 pinsource = 0;

    switch(pin)
    {
    case GPIO_Pin_0:
        pinsource = GPIO_PinSource0;
        break;
    case GPIO_Pin_1:
        pinsource = GPIO_PinSource1;
        break;
    case GPIO_Pin_2:
        pinsource = GPIO_PinSource2;
        break;
    case GPIO_Pin_3:
        pinsource = GPIO_PinSource3;
        break;
    case GPIO_Pin_4:
        pinsource = GPIO_PinSource4;
        break;
    case GPIO_Pin_5:
        pinsource = GPIO_PinSource5;
        break;
    case GPIO_Pin_6:
        pinsource = GPIO_PinSource6;
        break;
    case GPIO_Pin_7:
        pinsource = GPIO_PinSource7;
        break;
    case GPIO_Pin_8:
        pinsource = GPIO_PinSource8;
        break;
    case GPIO_Pin_9:
        pinsource = GPIO_PinSource9;
        break;
    case GPIO_Pin_10:
        pinsource = GPIO_PinSource10;
        break;
    case GPIO_Pin_11:
        pinsource = GPIO_PinSource11;
        break;
    case GPIO_Pin_12:
        pinsource = GPIO_PinSource12;
        break;
    case GPIO_Pin_13:
        pinsource = GPIO_PinSource13;
        break;
    case GPIO_Pin_14:
        pinsource = GPIO_PinSource14;
        break;
    case GPIO_Pin_15:
        pinsource = GPIO_PinSource15;
        break;
    default:
        break;
    }

    return pinsource;
}

/***************************************************************************************************
*\Function      stm32_get_rcc_of_tim
*\Description   ����TIM �õ�RCC�Ĵ���
*\Parameter     TIMx
*\Return        void
*\Note          
*\Log           2018.08.17    Ver 1.0    ���               
				����������
***************************************************************************************************/
static uint32_t stm32_get_rcc_of_tim(TIM_TypeDef* TIMx)
{
    uint32_t rcc = 0;

    /*
        APB1 ��ʱ���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
        APB2 ��ʱ���� TIM1, TIM8 ,TIM9, TIM10, TIM11
    */
    if (TIMx == TIM1)
    {
        rcc = RCC_APB2Periph_TIM1;
    }
    else if (TIMx == TIM8)
    {
        rcc = RCC_APB2Periph_TIM8;
    }
    else if (TIMx == TIM9)
    {
        rcc = RCC_APB2Periph_TIM9;
    }
    else if (TIMx == TIM10)
    {
        rcc = RCC_APB2Periph_TIM10;
    }
    else if (TIMx == TIM11)
    {
        rcc = RCC_APB2Periph_TIM11;
    }
    /* ������ APB1ʱ�� */
    else if (TIMx == TIM2)
    {
        rcc = RCC_APB1Periph_TIM2;
    }
    else if (TIMx == TIM3)
    {
        rcc = RCC_APB1Periph_TIM3;
    }
    else if (TIMx == TIM4)
    {
        rcc = RCC_APB1Periph_TIM4;
    }
    else if (TIMx == TIM5)
    {
        rcc = RCC_APB1Periph_TIM5;
    }
    else if (TIMx == TIM6)
    {
        rcc = RCC_APB1Periph_TIM6;
    }
    else if (TIMx == TIM7)
    {
        rcc = RCC_APB1Periph_TIM7;
    }
    else if (TIMx == TIM12)
    {
        rcc = RCC_APB1Periph_TIM12;
    }
    else if (TIMx == TIM13)
    {
        rcc = RCC_APB1Periph_TIM13;
    }
    else if (TIMx == TIM14)
    {
        rcc = RCC_APB1Periph_TIM14;
    }

    return rcc;
}

/***************************************************************************************************
*\Function      stm32_pwm_gpio_config
*\Description   ����GPIO��TIMʱ�ӣ� GPIO���ӵ�TIM���ͨ��
*\Parameter     GPIOx
*\Parameter     GPIO_PinX
*\Parameter     TIMx
*\Parameter     _ucChannel
*\Return        void
*\Note          
*\Log           2018.08.17    Ver 1.0    ���               
				����������
***************************************************************************************************/
static void stm32_timer_rcc_config(drv_timer_type* timer)
{
    /* ʹ��TIMʱ�� */
    if ((timer->TIM == TIM1) || (timer->TIM == TIM8))
    {
        RCC_APB2PeriphClockCmd(stm32_get_rcc_of_tim(timer->TIM), ENABLE);
    }
    else
    {
        RCC_APB1PeriphClockCmd(stm32_get_rcc_of_tim(timer->TIM), ENABLE);
    }

}


void stm32_timer_NVIC_config(drv_timer_type* timer)
{
	NVIC_InitTypeDef NVIC_InitStructure;	

	if(timer->TIM == TIM1)
	{
		NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	}
	else if(timer->TIM == TIM2)
	{
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	}
	else if(timer->TIM == TIM3)
	{
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	}
	else if(timer->TIM == TIM4)
	{
		NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	}
	else if(timer->TIM == TIM5)
	{
		NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	}
	else if(timer->TIM == TIM6)
	{
		NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	}
	else if(timer->TIM == TIM7)
	{
		NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	}
	else if(timer->TIM == TIM8)
	{
		NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;
	}

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void stm32_timer_TIM_config(drv_timer_type* timer)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;  
    uint16_t usPeriod;
    uint16_t usPrescaler;
    uint32_t uiTIMxCLK;	
	
	/*��ǰ���еĶ�ʱ��ʱ��Ϊ72M*/
	if ((timer->TIM == TIM1) || (timer->TIM == TIM8) || (timer->TIM == TIM9) || (timer->TIM == TIM10) || (timer->TIM == TIM11))
    {
        /* APB2 ��ʱ�� */
        uiTIMxCLK = SystemCoreClock;
    }
    else	/* APB1 ��ʱ�� */
    {
        uiTIMxCLK = SystemCoreClock;//SystemCoreClock / 2;
    }

	/*�����С��10K�Ķ�ʱƵ�ʣ��� TIM_Prescaler = 7199����72M/7200=10K�����Ϊ10K����*/
    if (timer->Freq < 10000)
    {
		usPrescaler = uiTIMxCLK / 10000  - 1;					/* ��Ƶ�� = 7200 */
		usPeriod =  (uiTIMxCLK / 7200) / timer->Freq  - 1;		/* �Զ���װ��ֵ */
    }
	/*����Ǵ���10KС��100K�Ķ�ʱƵ�ʣ��� TIM_Prescaler = 719����72M/720=100K�����Ϊ100K����*/
    else if (timer->Freq < 100000)
    {
        usPrescaler =  uiTIMxCLK / 100000  - 1;				/* ��Ƶ�� = 720 */
        usPeriod =  (uiTIMxCLK / 720) / timer->Freq  - 1;		/* �Զ���װ��ֵ */
    }
	/*����Ǵ���100KС��1M�Ķ�ʱƵ�ʣ��� TIM_Prescaler = 71����72M/72=1M�����Ϊ1M����*/
    else if (timer->Freq < 1000000)	/* ����4K��Ƶ�ʣ������Ƶ */
    {
        usPrescaler =uiTIMxCLK / 1000000  - 1;					/* ��Ƶ�� = 72 */
        usPeriod = (uiTIMxCLK / 72) / timer->Freq  - 1;	/* �Զ���װ��ֵ */
    }
//	else if (timer->Freq < 1000000)	/* ����4K��Ƶ�ʣ������Ƶ */
//    {
//        usPrescaler =uiTIMxCLK / 1000000  - 1;					/* ��Ƶ�� = 72 */
//        usPeriod = (uiTIMxCLK / 72) / timer->Freq  - 1;	/* �Զ���װ��ֵ */
//    }
	
	timer->period = usPeriod;
	timer->prescaler = usPrescaler;
	
    TIM_DeInit(timer->TIM);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = usPeriod;//�Զ�װ��ֵ Ҳ�Ǽ����������ֵ���������жϵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler;//��Ƶϵ��  
    TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1 ;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(timer->TIM, &TIM_TimeBaseStructure);              

	stm32_timer_NVIC_config(timer);
	TIM_ClearFlag(timer->TIM, TIM_FLAG_Update);
	TIM_ITConfig(timer->TIM, TIM_IT_Update, ENABLE);

//    TIM_Cmd(timer->TIM, ENABLE);
}
/*���õ���ʱ�䣬������Ƶ��,��λ����*/
static void stm32_timer_TIM_config_time_us(drv_timer_type* timer, uint32_t time_ms)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
    TIM_DeInit(timer->TIM);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = time_ms * 10  - 1;//�Զ�װ��ֵ Ҳ�Ǽ����������ֵ���������жϵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / 7200 - 1;;//��Ƶϵ��  
    TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1 ;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(timer->TIM, &TIM_TimeBaseStructure);              

	TIM_ClearFlag(timer->TIM, TIM_FLAG_Update);
	TIM_ITConfig(timer->TIM, TIM_IT_Update, ENABLE);

}
static void stm32_timer_save_ponter(drv_timer_type* timer)
{
    if (timer->TIM == TIM1)
    {
		timer_dev_TIM1 = timer;
    }
    else if (timer->TIM == TIM2)
    {
        timer_dev_TIM2 = timer;
    }
    else if (timer->TIM == TIM3)
    {
        timer_dev_TIM3 = timer;
    }
    else if (timer->TIM == TIM4)
    {
        timer_dev_TIM4 = timer;
    }
    else if (timer->TIM == TIM5)
    {
        timer_dev_TIM5 = timer;
    }
    else if (timer->TIM == TIM6)
    {
        timer_dev_TIM6 = timer;
    }
    else if (timer->TIM == TIM7)
    {
        timer_dev_TIM7 = timer;
    }
    else if (timer->TIM == TIM8)
    {
        timer_dev_TIM8 = timer;
    }
}
/***************************************************************************************************
*\Function      init_pwm
*\Description   ��ʼ��TIM��PWM���ܣ���ʼ����Ĭ�ϲ������������Ҫִ�п��������������
*\Parameter     pwm
*\Return        void
*\Note          
*\Log           2018.08.17    Ver 1.0    ���               
				����������
***************************************************************************************************/
void init_timer(drv_timer_type* timer)
{
    if (timer->TIM == RT_NULL)
    {
        rt_kprintf("InitTIM TIMER NULL!\n");
        return;
    } 
	
	stm32_timer_save_ponter(timer);
	stm32_timer_rcc_config(timer);	/* ʹ��GPIO��TIMʱ�ӣ�������TIMͨ����GPIO */
	stm32_timer_TIM_config(timer);
	
    timer->lock = rt_mutex_create("timer_Mut", RT_IPC_FLAG_FIFO);
    if (timer->lock == RT_NULL)
    {
        rt_kprintf("timer mutex create fail!\n");
        return;
    }
}
void set_timer_freq(drv_timer_type* timer, uint32_t _ulFreq)
{
    if (timer->TIM == RT_NULL)
    {
        rt_kprintf("TIM TIMER NULL!\n");
        return;
    }

    TIM_Cmd(timer->TIM , DISABLE);
	
	timer->Freq = _ulFreq;

	stm32_timer_TIM_config(timer);
}
/***************************************************************************************************
*\Function      turn_on_pwm
*\Description   ������ʱ����ʹ��PWM����������ǳ�ʼ�����Ǹı�Ƶ�ʡ�ռ�ձȵĲ�����һ��Ҫʵ����������������PWM
*\Parameter     pwm
*\Return        void
*\Note          
*\Log           2018.08.17    Ver 1.0    ���               
				����������
***************************************************************************************************/
void turn_on_timer(drv_timer_type* timer)
{
    if (timer->TIM == RT_NULL)
    {
        rt_kprintf("TIM TIMER NULL!\n");
        return;
    }

    TIM_Cmd(timer->TIM , ENABLE);
}

/***************************************************************************************************
*\Function      turn_off_pwm
*\Description   �رն�ʱ�����ر�PWM�����
*\Parameter     pwm
*\Return        void
*\Note          
*\Log           2018.08.17    Ver 1.0    ���               
				����������
***************************************************************************************************/
void turn_off_timer(drv_timer_type* timer)
{
    if (timer->TIM == RT_NULL)
    {
        rt_kprintf("TIM TIMER NULL!\n");
        return;
    }

    TIM_Cmd(timer->TIM , DISABLE);	
}

void set_timer_callback(drv_timer_type* timer,void (*func)(struct drv_timer_type_st *dev))
{
    if (timer->TIM == RT_NULL)
    {
        rt_kprintf("TIM TIMER NULL!\n");
        return;
    }
	
	timer->TIMCallback = func;	
}


#ifdef USE_TIM1
void TIM1_UP_IRQHandler(void)
{
	if( TIM_GetITStatus ( TIM1, TIM_IT_Update) != RESET )               
	{	
		if(timer_dev_TIM1 != RT_NULL)
		{
			timer_dev_TIM1->TIMCallback();
		}
		TIM_ClearITPendingBit ( TIM1, TIM_FLAG_Update ); 		
	}
}
#endif

#ifdef USE_TIM2
void TIM2_IRQHandler(void)
{
	if( TIM_GetITStatus ( TIM2, TIM_IT_Update) != RESET )               
	{	
		TIM_ClearITPendingBit ( TIM2, TIM_FLAG_Update ); 		
		if(timer_dev_TIM2 != RT_NULL)
		{
			timer_dev_TIM2->TIMCallback(timer_dev_TIM2);
		}		
	}
}
#endif

#ifdef USE_TIM3
void TIM3_IRQHandler(void)
{
	if( TIM_GetITStatus ( TIM3, TIM_IT_Update) != RESET )               
	{	
		if(timer_dev_TIM3 != RT_NULL)
		{
			timer_dev_TIM3->TIMCallback(timer_dev_TIM3);
		}
		TIM_ClearITPendingBit ( TIM3, TIM_FLAG_Update ); 		
	}
}
#endif

#ifdef USE_TIM4
void TIM4_IRQHandler(void)
{
	if( TIM_GetITStatus ( TIM4, TIM_IT_Update) != RESET )               
	{
		if(timer_dev_TIM4 != RT_NULL)
		{
			timer_dev_TIM4->TIMCallback(timer_dev_TIM4);
		}
		TIM_ClearITPendingBit ( TIM4, TIM_FLAG_Update ); 		
	}
}
#endif

#ifdef USE_TIM5
void TIM5_IRQHandler(void)
{
	if( TIM_GetITStatus ( TIM5, TIM_IT_Update) != RESET )               
	{
		if(timer_dev_TIM5 != RT_NULL)
		{
			timer_dev_TIM5->TIMCallback(timer_dev_TIM5);
		}	
		TIM_ClearITPendingBit ( TIM5, TIM_FLAG_Update ); 		
	}
}
#endif

#ifdef USE_TIM6
void TIM6_IRQHandler(void)
{
	if( TIM_GetITStatus ( TIM6, TIM_IT_Update) != RESET )               
	{
		if(timer_dev_TIM6 != RT_NULL)
		{
			timer_dev_TIM6->TIMCallback(timer_dev_TIM6);
		}	
		TIM_ClearITPendingBit ( TIM6, TIM_FLAG_Update ); 		
	}
}
#endif

#ifdef USE_TIM7
void TIM7_IRQHandler(void)
{
	if( TIM_GetITStatus ( TIM7, TIM_IT_Update) != RESET )               
	{
		if(timer_dev_TIM7 != RT_NULL)
		{
			timer_dev_TIM7->TIMCallback(timer_dev_TIM7);
		}	
		TIM_ClearITPendingBit ( TIM7, TIM_FLAG_Update ); 		
	}
}
#endif

#ifdef USE_TIM8
void TIM8_UP_IRQHandler(void)
{
	if( TIM_GetITStatus ( TIM8, TIM_IT_Update) != RESET )               
	{	
		if(timer_dev_TIM8 != RT_NULL)
		{
			timer_dev_TIM8->TIMCallback(timer_dev_TIM8);
		}
		TIM_ClearITPendingBit ( TIM8, TIM_FLAG_Update ); 		
	}
}
#endif
