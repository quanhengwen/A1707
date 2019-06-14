/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_max9611.c
*\Description   
*\Note          
*\Log           2019.01.28    Ver 1.0    baron
*               创建文件。
***************************************************************************************************/
#include "drv_max9611.h"
#include "app_board.h"
#include "param.h"





const MAX9611_set_cfg config_buffer = {0x0a,0x07,0x0b,0x00};

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
	/*   //计划把temp_msb第一次上电默认数据0x80作为ID读取，但设备不断电时该寄存器数据为芯片温度，不是默认的0x80,即不能作为ID读取
	u16 default_temp_msb = 0;
    u16 default_temp_lsb = 0;
	static u8 num=0;
    //读取por values ,第一次默认temp data byte 1(msb):0x800   temp data byte 1(lsb):0x000
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
	realtime_param.current = data->Current;
	rt_mutex_release(max->lock);
	return TRUE;
}
