/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          max9611.c
*\Description   
*\Note          
*\Log           2019.01.28    Ver 1.0    baron
*               创建文件。
***************************************************************************************************/
#include "max9611.h"
#include "app_board.h"
#include "param.h"

MAX9611_set_cfg config_buffer = {0x0a,0x07,0x0b,0x00};
MAX9611_data_type max9611_data;
/***************************************************************************************************
*\Function      max9611_thread_entry
*\Description   
*\Parameter     parameter
*\Return        void
*\Note          
*\Log           2019.01.28    Ver 1.0    baron
*               创建函数。
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
/***************************************************************************************************
*\Function      max9611_init
*\Description   设备初始化
*\Parameter     max
*\Return        void
*\Note          
*\Log           2019.01.28    Ver 1.0    baron               
				创建函数。
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

    //初始化i2c
    stm32_simu_i2c_init(max->I2C);

		//初始化max9611工作模式  all channels in fast-read mode,sequentially every 2ms,uses last gain setting.
		stm32_simu_i2c_send(max->I2C, MAX9611_ADDR, (u8*)&config_buffer.con_reg1, 2);//control register1 set
		stm32_simu_i2c_send(max->I2C, MAX9611_ADDR, (u8*)&config_buffer.con_reg2, 2);//control register2 set
}
/***************************************************************************************************
*\Function      max9611_data_read
*\Description   参数读取
*\Parameter     max
*\Parameter     data
*\Return        bool
*\Note          
*\Log           2019.01.28    Ver 1.0    baron               
				创建函数。
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

    //读寄存器数据
    if (!stm32_simu_i2c_recv(max->I2C, MAX9611_ADDR, 0x00, buf, 10))
    {
        rt_kprintf("max9611 read error!\n");
        rt_mutex_release(max->lock);
        return FALSE;
    }
    //进行数据转换
	 //温度计算 精度0.5°C
	temperature = (buf[8] << 8) + buf[9];
	if((temperature >> 15) == 0)//温度正值
	{
	    data->Temp = (temperature >> 7) * 0.48;
	}
	else if((temperature >> 15) == 1)//温度负值
	{
	    temperature = ~((temperature >> 7)- 1);
	    data->Temp = -(temperature * 0.48);
	}
	 //电流计算
	data->CSA = ((buf[0] << 8) + buf[1]) >> 4;
	data->Current = (data->CSA * Vref)/4096/100*1000;//实测电流值 采样电阻100mΩ 单位mA
  realtime_param.current = (u16)data->Current;
	rt_mutex_release(max->lock);
	return TRUE;
}
