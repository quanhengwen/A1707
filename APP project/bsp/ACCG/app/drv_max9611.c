/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_max9611.c
*\Description   
*\Note          
*\Log           2019.01.28    Ver 1.0    baron
*               �����ļ���
***************************************************************************************************/
#include "drv_max9611.h"
#include "app_board.h"
#include "param.h"





const MAX9611_set_cfg config_buffer = {0x0a,0x07,0x0b,0x00};

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
	/*   //�ƻ���temp_msb��һ���ϵ�Ĭ������0x80��ΪID��ȡ�����豸���ϵ�ʱ�üĴ�������ΪоƬ�¶ȣ�����Ĭ�ϵ�0x80,��������ΪID��ȡ
	u16 default_temp_msb = 0;
    u16 default_temp_lsb = 0;
	static u8 num=0;
    //��ȡpor values ,��һ��Ĭ��temp data byte 1(msb):0x800   temp data byte 1(lsb):0x000
	if(num == 0)
	{
		stm32_simu_i2c_recv(max->I2C, MAX9611_ADDR, 0x08, (u8*)&default_temp_msb, 1);
		stm32_simu_i2c_recv(max->I2C, MAX9611_ADDR, 0x09, (u8*)&default_temp_lsb, 1);

		rt_kprintf("MAX9611:%04X, %04X\n", default_temp_msb, default_temp_lsb);
		if (default_temp_msb != MAX9611_TEMP_DATA_MSB || default_temp_lsb != MAX9611_TEMP_DATA_LSB)
		{
			rt_kprintf("MAX9611 init fail!\n");
			return;
		}
		num = 1;
   }
	*/
	//��ʼ��max9611����ģʽ  all channels in fast-read mode,sequentially every 2ms,uses last gain setting.
	stm32_simu_i2c_send(max->I2C, MAX9611_ADDR, (u8*)&config_buffer.con_reg1, 2);//control register1 set
	stm32_simu_i2c_send(max->I2C, MAX9611_ADDR, (u8*)&config_buffer.con_reg2, 2);//control register2 set
}
/***************************************************************************************************
*\Function      max9611_data_read
*\Description   ������ȡ
*\Parameter     max
*\Parameter     data
*\Return        bool
*\Note          
*\Log           2019.01.28    Ver 1.0    baron               
				����������
***************************************************************************************************/
bool max9611_data_read(MAX9611_type* max, MAX9611_data_type* data)
{
    u8 buf[10];
    u16 temperature = 0;

    if (max == RT_NULL || max->I2C == RT_NULL || data == RT_NULL || max->lock == RT_NULL)
    {
        rt_kprintf("MAX9611 NULL!\n");
        return FALSE;
    }

    rt_mutex_take(max->lock, RT_WAITING_FOREVER);

    //���Ĵ�������
    if (!stm32_simu_i2c_recv(max->I2C, MAX9611_ADDR, 0x00, buf, 10))
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
	
	//��������
	data->CSA = ((buf[0] << 8) + buf[1]) >> 4;
	data->Current = (data->CSA * Vref)/4096/100*1000;//ʵ�����ֵ ��������100m�� ��λmA
	realtime_param.current = data->Current;
	rt_mutex_release(max->lock);
	return TRUE;
}
