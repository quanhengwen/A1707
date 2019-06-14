/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          hall_detect.c
*\Description   霍尔检测线程
*\Note          
*\Log           2019.04.17    Ver 1.0    杨贝贝
*               创建文件。
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
*\Function      config_gpio_to_input
*\Description  配置GPIO为输入状态
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    杨贝贝
*               创建函数。
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
*\Description   获取GPIO的电平状态
*\Parameter     
*\Return        高电平返回 TRUE， 低电平返回 FALSE
*\Note          
*\Log           2019.04.25    Ver 1.0    杨贝贝
*               创建函数。
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
*\Description   霍尔检测线程
*\Parameter     
*\Return        
*\Note          
*\Log           2019.04.25    Ver 1.0    杨贝贝
*               创建函数。
***************************************************************************************************/
void hall_detect_thread_entry(void* p)
{
	uint8_t folding_first_flag = 0, folding_first_cnt = 0;
	uint8_t combination_first_flag = 0, combination_first_cnt = 00;
	uint8_t gpio1_state,gpio2_state;
	
	while(1)
	{

		/*下面是检测折叠的两个霍尔*/
		gpio1_state = get_gpio_state(&hall_1);
		gpio2_state = get_gpio_state(&hall_2);
		if((!gpio1_state) && (!gpio2_state) )
		{
			/*检测到折叠*/
			if(folding_first_flag == 0 || folding_first_flag == 3)
			{
				/*如果是第一次检测到折叠*/
				folding_first_cnt++;
				
				if(folding_first_cnt > 5)
				{
					/*连续5次检测到，说明确实改变了电平，此时要发送CAN信号，并改变霍尔检测的状态*/
					report_data.folding_status = 0xFF;
					rt_kprintf("检测到突然高电平\r\n");
					/*发送完之后，要变标志不再这样检测*/
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
			/*检测到展开*/
			if(folding_first_flag == 1 || folding_first_flag == 3)
			{
				/*如果是第一次检测到折叠*/
				folding_first_cnt++;
				
				if(folding_first_cnt > 5)
				{
					/*连续5次检测到，说明确实改变了电平，此时要发送CAN信号，并改变霍尔检测的状态*/
					report_data.folding_status = 0x00;
					report_data.ID = iap_param.ID;
					canbus_middle_send(&CAN, 0x500, (uint8_t *)&report_data, 8);
					rt_kprintf("检测到突然低电平\r\n");
					/*发送完之后，要变标志不再这样检测*/
					folding_first_flag = 0;
					folding_first_cnt = 0;					
				}				
				
			}
			else
			{
				if((gpio1_state) && (!gpio2_state))
				{
					/*第一个是霍尔是高电平，第二个是低电平*/
					report_data.folding_status = 0x0F;
				}
				else if((!gpio1_state) && (gpio2_state))
				{
					/*第一个是霍尔是低电平，第二个是高电平*/
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



		/*下面是检测组合的两个霍尔*/
		gpio1_state = get_gpio_state(&hall_3);
		gpio2_state = get_gpio_state(&hall_4);
		if((!gpio1_state) && (!gpio2_state) )
		{
			/*检测到折叠*/
			if(folding_first_flag == 0 || folding_first_flag == 3)
			{
				/*如果是第一次检测到折叠*/
				folding_first_cnt++;
				
				if(folding_first_cnt > 5)
				{
					/*连续5次检测到，说明确实改变了电平，此时要发送CAN信号，并改变霍尔检测的状态*/
					report_data.combination_status = 0xFF;
					rt_kprintf("检测到突然高电平\r\n");
					/*发送完之后，要变标志不再这样检测*/
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
			/*检测到展开*/
			if(folding_first_flag == 1 || folding_first_flag == 3)
			{
				/*如果是第一次检测到折叠*/
				folding_first_cnt++;
				
				if(folding_first_cnt > 5)
				{
					/*连续5次检测到，说明确实改变了电平，此时要发送CAN信号，并改变霍尔检测的状态*/
					report_data.combination_status = 0x00;
					report_data.ID = iap_param.ID;
					canbus_middle_send(&CAN, 0x500, (uint8_t *)&report_data, 8);
					rt_kprintf("检测到突然低电平\r\n");
					/*发送完之后，要变标志不再这样检测*/
					folding_first_flag = 0;
					folding_first_cnt = 0;					
				}				
				
			}
			else
			{
				if((gpio1_state) && (!gpio2_state))
				{
					/*第一个是霍尔是高电平，第二个是低电平*/
					report_data.combination_status = 0x0F;
				}
				else if((!gpio1_state) && (gpio2_state))
				{
					/*第一个是霍尔是低电平，第二个是高电平*/
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

