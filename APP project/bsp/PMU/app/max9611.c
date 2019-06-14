/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          max9611.c
*\Description   
*\Note          
*\Log           2019.01.28    Ver 1.0    baron
*               �����ļ���
***************************************************************************************************/
#include "max9611.h"
#include "app_board.h"

MAX9611_set_cfg config_buffer = {0x0a,0x07,0x0b,0x00};
MAX9611_data_type max9611_data[4];
u16 MAX9611_Chn[4]=
{
 MAX9611_1_ADDR,
 MAX9611_2_ADDR,
 MAX9611_3_ADDR,
 MAX9611_4_ADDR
};
MAX9611_data_type max9611_data_charge;//for fifth 9611
/***************************************************************************************************
*\Function      max9611_control_init
*\Description   ��ʼ����·max9611���ؿ���
*\Parameter     
*\Parameter     
*\Return        
*\Note          
*\Log           2019.02.11    Ver 1.0    baron               
				����������
***************************************************************************************************/
void max9611_control_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
   /* control pin (PA.04/05/06/07) configuration */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/***************************************************************************************************
*\Function      max9611_thread_entry
*\Description   
*\Parameter     parameter
*\Return        void
*\Note          
*\Log           2019.01.28    Ver 1.0    baron
*               ����������
***************************************************************************************************/
void max9611_thread_entry(void* parameter)
{
	u8 num=0;
	rt_thread_delay(100);//
	
	while(1)
	{
	  for(num=0;num<MAX9611_Num;num++)
	  {
			max9611_data_read(&MAX9611, &max9611_data[num] , num);
			rt_thread_delay(25);	
//			printf("max9611_chn1 current: %f\r\n", max9611_data[0].Current);
//			printf("max9611_chn1 temperature: %f\r\n", max9611_data[0].Temp);  
	  }			
	}
}
/***************************************************************************************************
*\Function      max9611_init
*\Description   �豸��ʼ��
*\Parameter     max
*\Return        void
*\Note          
*\Log           2019.01.28    Ver 1.0    baron               
				����������
***************************************************************************************************/
void max9611_init(MAX9611_type* max)
{
    u8 num = 0;
    if (max == RT_NULL || max->I2C == RT_NULL)
    {
        rt_kprintf("MAX9611 NULL!\n");
        return;
    }

    max->lock = rt_mutex_create("MAX_Mut", RT_IPC_FLAG_FIFO);
    if (max->lock == RT_NULL)
    {
        rt_kprintf("max mutex create fail!\n");
        return;
    }

    //��ʼ��i2c
    stm32_simu_i2c_init(max->I2C);
		for(num=0;num<MAX9611_Num;num++)
		{
			//��ʼ��max9611����ģʽ  all channels in fast-read mode,sequentially every 2ms,uses last gain setting.
			stm32_simu_i2c_send(max->I2C, MAX9611_Chn[num], (u8*)&config_buffer.con_reg1, 2);//control register1 set
			stm32_simu_i2c_send(max->I2C, MAX9611_Chn[num], (u8*)&config_buffer.con_reg2, 2);//control register2 set
		}
}
/***************************************************************************************************
*\Function      max9611_data_read
*\Description   ������ȡ
*\Parameter     max
*\Parameter     data
*\Parameter     num 
*\Return        bool
*\Note          
*\Log           2019.01.28    Ver 1.0    baron               
				����������
***************************************************************************************************/
bool max9611_data_read(MAX9611_type* max, MAX9611_data_type* data, u8 num)
{
	u8 buf[10];
	u16 temperature = 0;
  float current_temp;
	
	if (max == RT_NULL || max->I2C == RT_NULL || data == RT_NULL || max->lock == RT_NULL)
	{
			rt_kprintf("MAX9611 NULL!\n");
			return FALSE;
	}

	rt_mutex_take(max->lock, RT_WAITING_FOREVER);

	//���Ĵ�������
	if (!stm32_simu_i2c_recv(max->I2C, MAX9611_Chn[num], 0x00, buf, 10))
	{
			rt_kprintf("max9611 read error!\n");
			rt_mutex_release(max->lock);
			return FALSE;
	}
	//��������ת��
	//�¶ȼ��� ����0.5��C
	temperature = (buf[8] << 8) + buf[9];
	if((temperature >> 15) == 0)//�¶���ֵ
	{
	    data->Temp = (temperature >> 7) * 0.48;
	}
	else if((temperature >> 15) == 1)//�¶ȸ�ֵ
	{
	    temperature = ~((temperature >> 7)- 1);
	    data->Temp = -(temperature * 0.48);
	}
	if(num == 0)
	{
	    //��������
	    data->CSA = ((buf[0] << 8) + buf[1]) >> 4;
		  if(state.switch_chn1 == 1)
			{
	     current_temp = (data->CSA * Vref)/4096/3*1000;//ʵ�����ֵ ��������3m�� ��λmA
			 data->Current = (0.8632*current_temp) - 10.559;
			}
			else
			{
			 data->Current = 0;//�ر�״̬����Ϊ0
			}
	}
	else if(num == 1)
	{
	    //��������
	    data->CSA = ((buf[0] << 8) + buf[1]) >> 4;
		  if(state.switch_chn2 == 1)
			{
	     current_temp = (data->CSA * Vref)/4096/3*1000;//ʵ�����ֵ ��������3m�� ��λmA
			 data->Current = (0.8632*current_temp) - 10.559;
			}
			else
			{
			 data->Current = 0;//�ر�״̬����Ϊ0
			}
	}
	else if(num == 2)
	{
	    //��������
	    data->CSA = ((buf[0] << 8) + buf[1]) >> 4;
		  if(state.switch_chn3 == 1)
			{
	     current_temp = (data->CSA * Vref)/4096/3*1000;//ʵ�����ֵ ��������3m�� ��λmA
			 data->Current = (0.8632*current_temp) - 10.559;
			}
			else
			{
			 data->Current = 0;//�ر�״̬����Ϊ0
			}
	}
	else if(num == 3)
	{
	    //��������
	    data->CSA = ((buf[0] << 8) + buf[1]) >> 4;
		  if(state.switch_chn4 == 1)
			{
	     current_temp = (data->CSA * Vref)/4096/3*1000;//ʵ�����ֵ ��������3m�� ��λmA
			 data->Current = (0.8632*current_temp) - 10.559;
			}
			else
			{
			 data->Current = 0;//�ر�״̬����Ϊ0
			}
	}
	rt_mutex_release(max->lock);
	return TRUE;
}
/***************************************************************************************************
*\Function      max9611_init_charge
*\Description   �豸��ʼ��
*\Parameter     max
*\Return        void
*\Note          
*\Log           2019.01.28    Ver 1.0    baron               
				����������
***************************************************************************************************/
void max9611_init_charge(MAX9611_type* max)
{

    if (max == RT_NULL || max->I2C == RT_NULL)
    {
        rt_kprintf("MAX9611 NULL CHARGE!\n");
        return;
    }

    max->lock = rt_mutex_create("MAX_Mut_CHARGE", RT_IPC_FLAG_FIFO);
    if (max->lock == RT_NULL)
    {
        rt_kprintf("max mutex charge create fail!\n");
        return;
    }

    //��ʼ��i2c
    stm32_simu_i2c_init(max->I2C);
		
		//��ʼ��max9611����ģʽ  all channels in fast-read mode,sequentially every 2ms,uses last gain setting.
		stm32_simu_i2c_send(max->I2C, MAX9611_ADDR, (u8*)&config_buffer.con_reg1, 2);//control register1 set
		stm32_simu_i2c_send(max->I2C, MAX9611_ADDR, (u8*)&config_buffer.con_reg2, 2);//control register2 set
}
/***************************************************************************************************
*\Function      max9611_data_readt_charge
*\Description   ������ȡ
*\Parameter     max
*\Parameter     data
*\Return        bool
*\Note          
*\Log           2019.01.28    Ver 1.0    baron               
				����������
***************************************************************************************************/
bool max9611_data_read_charge(MAX9611_type* max, MAX9611_data_type* data)
{
    u8 buf[10];
    u16 temperature = 0;
    float current_temp;
	
    if (max == RT_NULL || max->I2C == RT_NULL || data == RT_NULL || max->lock == RT_NULL)
    {
        rt_kprintf("MAX9611 NULL CHARGE!\n");
        return FALSE;
    }

    rt_mutex_take(max->lock, RT_WAITING_FOREVER);

    //���Ĵ�������
    if (!stm32_simu_i2c_recv(max->I2C, MAX9611_ADDR, 0x00, buf, 10))
    {
        rt_kprintf("max9611 read charge error!\n");
        rt_mutex_release(max->lock);
        return FALSE;
    }
    //��������ת��
	 //�¶ȼ��� ����0.5��C
	temperature = (buf[8] << 8) + buf[9];
	if((temperature >> 15) == 0)//�¶���ֵ
	{
	    data->Temp = (temperature >> 7) * 0.48;
	}
	else if((temperature >> 15) == 1)//�¶ȸ�ֵ
	{
	    temperature = ~((temperature >> 7)- 1);
	    data->Temp = -(temperature * 0.48);
	}
	 //��������
	data->CSA = ((buf[0] << 8) + buf[1]) >> 4;
	if(state.charge_on == 1)//��������Ӻ��������ֵ
	{
		current_temp = (data->CSA * Vref)/4096/3*1000;//ʵ�����ֵ ��������3m�� ��λmA
		data->Current = (0.8632*current_temp) - 10.559;
	}
	else
	{
		data->Current = 0;//�����δ����ʱ������Ϊ0
	}
	rt_mutex_release(max->lock);
	return TRUE;
}
/***************************************************************************************************
*\Function      max9611_charge_thread_entry
*\Description   
*\Parameter     parameter
*\Return        void
*\Note          
*\Log           2019.01.28    Ver 1.0    baron
*               ����������
***************************************************************************************************/
void max9611_charge_thread_entry(void* parameter)
{
	rt_thread_delay(100);//
	
	while(1)
	{
    max9611_data_read_charge(&MAX9611_charge, &max9611_data_charge);
	  rt_thread_delay(100);				
	}

}

