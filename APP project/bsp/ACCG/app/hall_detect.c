/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          hall_detect.c
*\Description   ��������߳�
*\Note          
*\Log           2019.04.17    Ver 1.0    ���
*               �����ļ���
***************************************************************************************************/
#include "hall_detect.h"
#include "app_board.h"
#include "canbus_middle.h"




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
*\Function      config_gpio_to_input
*\Description  ����GPIOΪ����״̬
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    ���
*               ����������
***************************************************************************************************/
void config_gpio_to_input(GpioType* gpio_dev)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(gpio_dev == RT_NULL)
	{
		rt_kprintf("gpio_dev no define!\r\n");
		return;
	}
	
	gpio_rcc_enable(gpio_dev->gpio);
	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		
    GPIO_InitStructure.GPIO_Pin = gpio_dev->gpio_pin;
    GPIO_Init(gpio_dev->gpio, &GPIO_InitStructure);
}

/***************************************************************************************************
*\Function      get_gpio_state
*\Description   ��ȡGPIO�ĵ�ƽ״̬
*\Parameter     
*\Return        �ߵ�ƽ���� TRUE�� �͵�ƽ���� FALSE
*\Note          
*\Log           2019.04.25    Ver 1.0    ���
*               ����������
***************************************************************************************************/
bool get_gpio_state(GpioType* gpio_dev)
{
    if (GPIO_ReadInputDataBit(gpio_dev->gpio, gpio_dev->gpio_pin))
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/***************************************************************************************************
*\Function      hall_detect_thread_entry
*\Description   ��������߳�
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    ���
*               ����������
***************************************************************************************************/
void hall_detect_thread_entry(void* p)
{
	uint8_t folding_first_flag = 0, folding_first_cnt = 0;
	uint8_t combination_first_flag = 0, combination_first_cnt = 00;
	uint8_t gpio1_state,gpio2_state;
	
	while(1)
	{

		/*�����Ǽ���۵�����������*/
		gpio1_state = get_gpio_state(&hall_1);
		gpio2_state = get_gpio_state(&hall_2);
		if((!gpio1_state) && (!gpio2_state) )
		{
			/*��⵽�۵�*/
			if(folding_first_flag == 0 || folding_first_flag == 3)
			{
				/*����ǵ�һ�μ�⵽�۵�*/
				folding_first_cnt++;
				
				if(folding_first_cnt > 5)
				{
					/*����5�μ�⵽��˵��ȷʵ�ı��˵�ƽ����ʱҪ����CAN�źţ����ı��������״̬*/
					report_data.folding_status = 0xFF;
					rt_kprintf("��⵽ͻȻ�ߵ�ƽ\r\n");
					/*������֮��Ҫ���־�����������*/
					folding_first_flag = 1;
					folding_first_cnt = 0;					
				}				
				
			}
			else
			{
				folding_first_cnt = 0;
			}
			

		}
		else if((gpio1_state) && (gpio2_state) )
		{
			/*��⵽չ��*/
			if(folding_first_flag == 1 || folding_first_flag == 3)
			{
				/*����ǵ�һ�μ�⵽�۵�*/
				folding_first_cnt++;
				
				if(folding_first_cnt > 5)
				{
					/*����5�μ�⵽��˵��ȷʵ�ı��˵�ƽ����ʱҪ����CAN�źţ����ı��������״̬*/
					report_data.folding_status = 0x00;
					report_data.ID = iap_param.ID;
					canbus_middle_send(&CAN, 0x500, (uint8_t *)&report_data, 8);
					rt_kprintf("��⵽ͻȻ�͵�ƽ\r\n");
					/*������֮��Ҫ���־�����������*/
					folding_first_flag = 0;
					folding_first_cnt = 0;					
				}				
				
			}
			else
			{
				if((gpio1_state) && (!gpio2_state))
				{
					/*��һ���ǻ����Ǹߵ�ƽ���ڶ����ǵ͵�ƽ*/
					report_data.folding_status = 0x0F;
				}
				else if((!gpio1_state) && (gpio2_state))
				{
					/*��һ���ǻ����ǵ͵�ƽ���ڶ����Ǹߵ�ƽ*/
					report_data.folding_status = 0xF0;					
				}
				folding_first_cnt = 0;
			}
		}
		else
		{
			folding_first_flag = 3;
			folding_first_cnt = 0;
		}



		/*�����Ǽ����ϵ���������*/
		gpio1_state = get_gpio_state(&hall_3);
		gpio2_state = get_gpio_state(&hall_4);
		if((!gpio1_state) && (!gpio2_state) )
		{
			/*��⵽�۵�*/
			if(folding_first_flag == 0 || folding_first_flag == 3)
			{
				/*����ǵ�һ�μ�⵽�۵�*/
				folding_first_cnt++;
				
				if(folding_first_cnt > 5)
				{
					/*����5�μ�⵽��˵��ȷʵ�ı��˵�ƽ����ʱҪ����CAN�źţ����ı��������״̬*/
					report_data.combination_status = 0xFF;
					rt_kprintf("��⵽ͻȻ�ߵ�ƽ\r\n");
					/*������֮��Ҫ���־�����������*/
					folding_first_flag = 1;
					folding_first_cnt = 0;					
				}				
				
			}
			else
			{
				folding_first_cnt = 0;
			}
			

		}
		else if((gpio1_state) && (gpio2_state) )
		{
			/*��⵽չ��*/
			if(folding_first_flag == 1 || folding_first_flag == 3)
			{
				/*����ǵ�һ�μ�⵽�۵�*/
				folding_first_cnt++;
				
				if(folding_first_cnt > 5)
				{
					/*����5�μ�⵽��˵��ȷʵ�ı��˵�ƽ����ʱҪ����CAN�źţ����ı��������״̬*/
					report_data.combination_status = 0x00;
					report_data.ID = iap_param.ID;
					canbus_middle_send(&CAN, 0x500, (uint8_t *)&report_data, 8);
					rt_kprintf("��⵽ͻȻ�͵�ƽ\r\n");
					/*������֮��Ҫ���־�����������*/
					folding_first_flag = 0;
					folding_first_cnt = 0;					
				}				
				
			}
			else
			{
				if((gpio1_state) && (!gpio2_state))
				{
					/*��һ���ǻ����Ǹߵ�ƽ���ڶ����ǵ͵�ƽ*/
					report_data.combination_status = 0x0F;
				}
				else if((!gpio1_state) && (gpio2_state))
				{
					/*��һ���ǻ����ǵ͵�ƽ���ڶ����Ǹߵ�ƽ*/
					report_data.combination_status = 0xF0;					
				}
				folding_first_cnt = 0;
			}
		}
		else
		{
			folding_first_flag = 3;
			folding_first_cnt = 0;
		}		
		
		
		rt_thread_delay(2);
	}	
}

