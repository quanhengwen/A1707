/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          mldg.c
*\Description   ���������������ƴ�����
*\Note          
*\Log           2019.02.15    Ver 1.0    baron
*               �����ļ���
***************************************************************************************************/
#include "mldg.h"

u8 mode = 0;

/***************************************************************************************************
*\Function      mldg_init
*\Description   �˿ڳ�ʼ��
*\Parameter     Mldg
*\Return        void
*\Note          
*\Log           2019.02.15    Ver 1.0    baron               
				����������
***************************************************************************************************/
void mldg_init(GpioType* mldg)
{
	if (mldg == RT_NULL)
	{
	  rt_kprintf("MLDG NULL!\n");
	  return;
	}
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	/* led pin (PB.00) configuration */
	GPIO_InitStructure.GPIO_Pin =  mldg->gpio_pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_SetBits(mldg->gpio, mldg->gpio_pin);

	GPIO_Init(mldg->gpio, &GPIO_InitStructure);
}
/***************************************************************************************************
*\Function      led Mldg_Mode1
*\Description   �ƴ���˸ģʽ1
*\Parameter     Mldg
*\Return        void
*\Note          
*\Log           2019.02.15    Ver 1.0    baron               
				����������
***************************************************************************************************/
void Mldg_Mode1(u8 time)
{
	u8 num = 0;
	switch(mode)
	{
		case 0:
		{
	      for(num=0;num<1;num++)//����һ��
	      {
	       GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	       rt_thread_delay(time);
		   GPIO_SetBits(GPIOB, GPIO_Pin_0);//off
	       rt_thread_delay(time);
	      }		
		}
		break;
		case 1:
		{
		
		}
		break;
		case 2:
		{
	      for(num=0;num<3;num++)//��������
	      {
	       GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	       rt_thread_delay(time);
		   GPIO_SetBits(GPIOB, GPIO_Pin_0);//off
	       rt_thread_delay(time);
	      }		
		}
		break;
		case 3:
		{
	      for(num=0;num<2;num++)//��������
	      {
	       GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	       rt_thread_delay(time);
		   GPIO_SetBits(GPIOB, GPIO_Pin_0);//off
	       rt_thread_delay(time);
	      }			
		}
		break;
		default:break;
	}
	mode = 1;
}
/***************************************************************************************************
*\Function      led Mldg_Mode2
*\Description   �ƴ���˸ģʽ2
*\Parameter     Mldg
*\Return        void
*\Note          
*\Log           2019.02.15    Ver 1.0    baron               
				����������
***************************************************************************************************/
void Mldg_Mode2(u8 time)
{
	u8 num = 0;
	switch(mode)
	{
		case 0:
		{
	      for(num=0;num<2;num++)//��������
	      {
	       GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	       rt_thread_delay(time);
		   GPIO_SetBits(GPIOB, GPIO_Pin_0);//off
	       rt_thread_delay(time);
	      }		
		}
		break;
		case 1:
		{
	      for(num=0;num<1;num++)//����һ��
	      {
	       GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	       rt_thread_delay(time);
		   GPIO_SetBits(GPIOB, GPIO_Pin_0);//off
	       rt_thread_delay(time);
	      }			
		}
		break;
		case 2:
		{
		
		}
		break;
		case 3:
		{
	      for(num=0;num<3;num++)//��������
	      {
	       GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	       rt_thread_delay(time);
		   GPIO_SetBits(GPIOB, GPIO_Pin_0);//off
	       rt_thread_delay(time);
	      }			
		}
		break;
		default:break;
	}
	mode = 2;
}
/***************************************************************************************************
*\Function      led Mldg_Mode3
*\Description   �ƴ���˸ģʽ3
*\Parameter     Mldg
*\Return        void
*\Note          
*\Log           2019.02.15    Ver 1.0    baron               
				����������
***************************************************************************************************/
void Mldg_Mode3(u8 time)
{
	u8 num = 0;
	switch(mode)
	{
		case 0:
		{
	      for(num=0;num<3;num++)//��������
	      {
	       GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	       rt_thread_delay(time);
		   GPIO_SetBits(GPIOB, GPIO_Pin_0);//off
	       rt_thread_delay(time);
	      }		
		}
		break;
		case 1:
		{
	      for(num=0;num<2;num++)//��������
	      {
	       GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	       rt_thread_delay(time);
		   GPIO_SetBits(GPIOB, GPIO_Pin_0);//off
	       rt_thread_delay(time);
	      }			
		}
		break;
		case 2:
		{
	      for(num=0;num<1;num++)//����һ��
	      {
	       GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	       rt_thread_delay(time);
		   GPIO_SetBits(GPIOB, GPIO_Pin_0);//off
	       rt_thread_delay(time);
	      }		
		}
		break;
		case 3:
		{
			
		}
		break;
		default:break;
	}
	mode = 3;
}
/***************************************************************************************************
*\Function      led Mldg_ModeOff
*\Description   �ƴ���˸ģʽ�ر�
*\Parameter     Mldg
*\Return        void
*\Note          
*\Log           2019.02.15    Ver 1.0    baron               
				����������
***************************************************************************************************/
void Mldg_ModeOff(u8 time)
{
  u8 num_off = 0;
  switch(mode)
  {
	case 0:
	{
		
	}
	break;
	case 1:
	{
	  for(num_off=0;num_off<3;num_off++)//�������ιص�
	  {
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);
		rt_thread_delay(time);
		GPIO_SetBits(GPIOB, GPIO_Pin_0);//off
		rt_thread_delay(time);
	  }
	}
	break;
	case 2:
	{
	  for(num_off=0;num_off<2;num_off++)//�������ιص�
	  {
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);
		rt_thread_delay(time);
		GPIO_SetBits(GPIOB, GPIO_Pin_0);//off
		rt_thread_delay(time);
	  }
	}
	break;
	case 3:
	{
	  for(num_off=0;num_off<1;num_off++)//����һ�ιص�
	  {
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);
		rt_thread_delay(time);
		GPIO_SetBits(GPIOB, GPIO_Pin_0);//off
		rt_thread_delay(time);
	  }		
	}
	break;
	default:break;
  }
  mode = 0;
}
