/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_mpu9250.c
*\Description   
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝
*               创建文件。
***************************************************************************************************/
#include "drv_mpu9250.h"
#include "inv_mpu.h"

MPU9250_type* mpu9250_dev_pointer;


#define USE_MUP_DMP   //如果使用MPU官方的DMP就宏定义，否则使用软件四元数发


//#define USE_DRV_SIMU_I2C_C   //如果使用 drv_simu_i2c.c 文件的驱动，就宏定义



void i2c_Start(stm32_simu_i2c_type* i2c_dev);
void i2c_Stop(stm32_simu_i2c_type* i2c_dev);

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
/*
*********************************************************************************************************
*	函 数 名: bsp_InitI2C
*	功能说明: 配置I2C总线的GPIO，采用模拟IO的方式实现
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitI2C(stm32_simu_i2c_type* i2c_dev)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	gpio_rcc_enable(i2c_dev->scl->gpio);/* 打开GPIO时钟 */
    gpio_rcc_enable(i2c_dev->sda->gpio);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* 开漏输出模式 */
	
	GPIO_InitStructure.GPIO_Pin = i2c_dev->scl->gpio_pin;
	GPIO_Init(i2c_dev->scl->gpio, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	/* 开漏输出模式 */
	
	GPIO_InitStructure.GPIO_Pin = i2c_dev->sda->gpio_pin;;
	GPIO_Init(i2c_dev->sda->gpio, &GPIO_InitStructure);

	/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
	i2c_Stop(i2c_dev);
}


/***************************************************************************************************
*\Function      i2c_Delay
*\Description   I2C延迟
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/	
static void i2c_Delay(void)
{
	uint8_t i;

	/*　
		CPU主频168MHz时，在内部Flash运行, MDK工程不优化。用台式示波器观测波形。
		循环次数为5时，SCL频率 = 1.78MHz (读耗时: 92ms, 读写正常，但是用示波器探头碰上就读写失败。时序接近临界)
		循环次数为10时，SCL频率 = 1.1MHz (读耗时: 138ms, 读速度: 118724B/s)
		循环次数为30时，SCL频率 = 440KHz， SCL高电平时间1.0us，SCL低电平时间1.2us

		上拉电阻选择2.2K欧时，SCL上升沿时间约0.5us，如果选4.7K欧，则上升沿约1us

		实际应用选择400KHz左右的速率即可
	*/
	for (i = 0; i < 3; i++);
}


/***************************************************************************************************
*\Function      i2c_Start
*\Description   CPU发起I2C总线启动信号
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
static void i2c_Start(stm32_simu_i2c_type* i2c_dev)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	i2c_dev->sda->gpio->BSRR = i2c_dev->sda->gpio_pin;
	i2c_dev->scl->gpio->BSRR = i2c_dev->scl->gpio_pin;
	i2c_Delay();
	i2c_dev->sda->gpio->BRR = i2c_dev->sda->gpio_pin;
	i2c_Delay();	
	i2c_dev->scl->gpio->BRR = i2c_dev->scl->gpio_pin;
	i2c_Delay();
}

/***************************************************************************************************
*\Function      i2c_Stop
*\Description   CPU发起I2C总线停止信号
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
static void i2c_Stop(stm32_simu_i2c_type* i2c_dev)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	i2c_dev->sda->gpio->BRR = i2c_dev->sda->gpio_pin;
	i2c_dev->scl->gpio->BSRR = i2c_dev->scl->gpio_pin;
	i2c_Delay();
	i2c_dev->sda->gpio->BSRR = i2c_dev->sda->gpio_pin;
	i2c_Delay();
}

/***************************************************************************************************
*\Function      i2c_SendByte
*\Description   CPU向I2C总线设备发送8bit数据
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
static void i2c_SendByte(stm32_simu_i2c_type* i2c_dev, uint8_t _ucByte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{
		if (_ucByte & 0x80)
		{
			i2c_dev->sda->gpio->BSRR = i2c_dev->sda->gpio_pin;
		}
		else
		{
			i2c_dev->sda->gpio->BRR = i2c_dev->sda->gpio_pin;
		}
		i2c_Delay();
		i2c_dev->scl->gpio->BSRR = i2c_dev->scl->gpio_pin;
		i2c_Delay();
		i2c_dev->scl->gpio->BRR = i2c_dev->scl->gpio_pin;
		if (i == 7)
		{
			 i2c_dev->sda->gpio->BSRR = i2c_dev->sda->gpio_pin; // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */
		i2c_Delay();
	}
}

/***************************************************************************************************
*\Function      i2c_ReadByte
*\Description   CPU从I2C总线设备读取8bit数据
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
uint8_t i2c_ReadByte(stm32_simu_i2c_type* i2c_dev)
{
	uint8_t i;
	uint8_t value;

	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		i2c_dev->scl->gpio->BSRR = i2c_dev->scl->gpio_pin;
		i2c_Delay();
		if (((i2c_dev->sda->gpio->IDR & i2c_dev->sda->gpio_pin) != 0))
		{
			value++;
		}
		i2c_dev->scl->gpio->BRR = i2c_dev->scl->gpio_pin;
		i2c_Delay();
	}
	return value;
}

/***************************************************************************************************
*\Function      i2c_WaitAck
*\Description   CPU产生一个时钟，并读取器件的ACK应答信号
*\Parameter     
*\Return        返回0表示正确应答，1表示无器件响应
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
uint8_t i2c_WaitAck(stm32_simu_i2c_type* i2c_dev)
{
	uint8_t re;

	i2c_dev->sda->gpio->BSRR = i2c_dev->sda->gpio_pin;	/* CPU释放SDA总线 */
	i2c_Delay();
	i2c_dev->scl->gpio->BSRR = i2c_dev->scl->gpio_pin;	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	i2c_Delay();
	if (((i2c_dev->sda->gpio->IDR & i2c_dev->sda->gpio_pin) != 0))	/* CPU读取SDA口线状态 */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	i2c_dev->scl->gpio->BRR = i2c_dev->scl->gpio_pin;
	i2c_Delay();
	return re;
}

/***************************************************************************************************
*\Function      i2c_Ack
*\Description   CPU产生一个ACK信号
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void i2c_Ack(stm32_simu_i2c_type* i2c_dev)
{
	i2c_dev->sda->gpio->BRR = i2c_dev->sda->gpio_pin;	/* CPU驱动SDA = 0 */
	i2c_Delay();
	i2c_dev->scl->gpio->BSRR = i2c_dev->scl->gpio_pin;	/* CPU产生1个时钟 */
	i2c_Delay();
	i2c_dev->scl->gpio->BRR = i2c_dev->scl->gpio_pin;
	i2c_Delay();
	i2c_dev->sda->gpio->BSRR = i2c_dev->sda->gpio_pin;	/* CPU释放SDA总线 */
}

/***************************************************************************************************
*\Function      i2c_NAck
*\Description   CPU产生1个NACK信号
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void i2c_NAck(stm32_simu_i2c_type* i2c_dev)
{
	i2c_dev->sda->gpio->BSRR = i2c_dev->sda->gpio_pin;	/* CPU驱动SDA = 1 */
	i2c_Delay();
	i2c_dev->scl->gpio->BSRR = i2c_dev->scl->gpio_pin;	/* CPU产生1个时钟 */
	i2c_Delay();
	i2c_dev->scl->gpio->BRR = i2c_dev->scl->gpio_pin;
	i2c_Delay();
}

/***************************************************************************************************
*\Function      i2c_CheckDevice
*\Description   检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
*\Parameter     Address：设备的I2C总线地址
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
uint8_t i2c_CheckDevice(stm32_simu_i2c_type* i2c_dev,uint8_t _Address)
{
	uint8_t ucAck;

	if (((i2c_dev->sda->gpio->IDR & i2c_dev->sda->gpio_pin) != 0) && ((i2c_dev->scl->gpio->IDR & i2c_dev->scl->gpio_pin) != 0))
	{
		i2c_Start(i2c_dev);		/* 发送启动信号 */

		/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
//		i2c_SendByte(_Address | 1);
		ucAck = i2c_WaitAck(i2c_dev);	/* 检测设备的ACK应答 */

		i2c_Stop(i2c_dev);			/* 发送停止信号 */

		return ucAck;
	}
	return 1;	/* I2C总线异常 */
}


/***************************************************************************************************
*\Function      delay_us
*\Description   延迟us
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*72; 						//需要的节拍数 
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};
}
 

/***************************************************************************************************
*\Function      delay_ms
*\Description   延迟ms
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void delay_ms(u16 nms)
{
	delay_us(1000*nms);
}		


#if 0
/***************************************************************************************************
*\Function      mpu9250_set_accel_fsr
*\Description   设置MPU9250加速度传感器满量程范围
*\Parameter     fsr:0,±2g;1,±4g;2,±8g;3,±16g
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
uint32_t mpu9250_set_accel_fsr(MPU9250_type* mpu, uint8_t fsr)
{
	uint8_t send_buf[2];
	
	send_buf[0] = MPU_ACCEL_CFG_REG;
	send_buf[1] = fsr<<3;
	
	return stm32_simu_i2c_send(mpu->I2C, MPU9250_ADDR, send_buf, 2);//设置加速度传感器满量程范围  
}

/***************************************************************************************************
*\Function      mpu9250_set_gyro_fsr
*\Description   设置MPU9250陀螺仪传感器满量程范围
*\Parameter     fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
uint32_t mpu9250_set_gyro_fsr(MPU9250_type* mpu, uint8_t fsr)
{
	uint8_t send_buf[2];
	
	send_buf[0] = MPU_GYRO_CFG_REG;
	send_buf[1] = fsr<<3;
	
	return stm32_simu_i2c_send(mpu->I2C, MPU9250_ADDR, send_buf, 2);//设置陀螺仪传感器满量程范围  
}

/***************************************************************************************************
*\Function      mpu9250_set_lpf
*\Description   设置MPU9250的数字低通滤波器
*\Parameter     lpf:数字低通滤波频率(Hz)
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
uint32_t mpu9250_set_lpf(MPU9250_type* mpu, uint16_t lpf)
{
	uint8_t send_buf[2];
	
	send_buf[0] = MPU_CFG_REG;
	if(lpf >= 188) send_buf[1] = 1;
	else if(lpf >= 98) send_buf[1] = 2;
	else if(lpf >= 42) send_buf[1] = 3;
	else if(lpf >= 20) send_buf[1] = 4;
	else if(lpf >= 10) send_buf[1] = 5;
	else send_buf[1]=6;
	
	return stm32_simu_i2c_send(mpu->I2C, MPU9250_ADDR, send_buf, 2);//设置数字低通滤波器
}

/***************************************************************************************************
*\Function      mpu9250_set_rate
*\Description   设置MPU9250的采样率(假定Fs=1KHz)
*\Parameter     rate:4~1000(Hz)
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
uint32_t mpu9250_set_rate(MPU9250_type* mpu, uint16_t rate)
{
	uint8_t send_buf[2];
	
	send_buf[0] = MPU_SAMPLE_RATE_REG;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	send_buf[1]=1000/rate-1;
	
	stm32_simu_i2c_send(mpu->I2C, MPU9250_ADDR, send_buf, 2);//设置数字低通滤波器
	return   mpu9250_set_lpf(mpu, rate/2);	//自动设置LPF为采样率的一半
}

/***************************************************************************************************
*\Function      mpu9250_get_temperature
*\Description   获得温度值
*\Parameter    
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
uint16_t mpu9250_get_temperature(MPU9250_type* mpu)
{
	uint8_t rev_buf[2];
    short raw;
	float temp;	
	
	stm32_simu_i2c_recv(mpu->I2C, MPU9250_ADDR, MPU_TEMP_OUTH_REG, (u8*)rev_buf, 2);	
	raw=((u16)rev_buf[0]<<8)|rev_buf[1];  
    temp=21+((double)raw)/333.87;  
    
	return temp*100;;
}

/***************************************************************************************************
*\Function      mpu9250_get_gyroscope
*\Description   获得陀螺仪值(原始值)
*\Parameter    	gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void mpu9250_get_gyroscope(MPU9250_type* mpu, uint16_t *gx,uint16_t *gy,uint16_t *gz)
{
	uint8_t rev_buf[6];	
	
	stm32_simu_i2c_recv(mpu->I2C, MPU9250_ADDR, MPU_GYRO_XOUTH_REG, (u8*)rev_buf, 6);	
	*gx=((u16)rev_buf[0]<<8)|rev_buf[1];  
	*gy=((u16)rev_buf[2]<<8)|rev_buf[3];  
	*gz=((u16)rev_buf[4]<<8)|rev_buf[5]; 
}

/***************************************************************************************************
*\Function      mpu9250_get_gyroscope
*\Description   获得加速度值(原始值)
*\Parameter    	gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void mpu9250_get_accelerometer(MPU9250_type* mpu, uint16_t *ax,uint16_t *ay,uint16_t *az)
{
	uint8_t rev_buf[6];	
	
	stm32_simu_i2c_recv(mpu->I2C, MPU9250_ADDR, MPU_ACCEL_XOUTH_REG, (u8*)rev_buf, 6);	
	*ax=((u16)rev_buf[0]<<8)|rev_buf[1];  
	*ay=((u16)rev_buf[2]<<8)|rev_buf[3];  
	*az=((u16)rev_buf[4]<<8)|rev_buf[5]; 
}

/***************************************************************************************************
*\Function      mpu9250_get_gyroscope
*\Description   获得磁力计值(原始值)
*\Parameter    	mx,my,mz:磁力计x,y,z轴的原始读数(带符号)
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void mpu9250_get_magnetometer(MPU9250_type* mpu, uint16_t *mx,uint16_t *my,uint16_t *mz)
{
	uint8_t rev_buf[6];	
	uint8_t send_buf[2];
	
	stm32_simu_i2c_recv(mpu->I2C, AK8963_ADDR, MPU_ACCEL_XOUTH_REG, (u8*)rev_buf, 6);	
	*mx=((u16)rev_buf[0]<<8)|rev_buf[1];  
	*my=((u16)rev_buf[2]<<8)|rev_buf[3];  
	*mz=((u16)rev_buf[4]<<8)|rev_buf[5]; 
	
	send_buf[0] = MAG_CNTL1;
	send_buf[0] = 0x11;
	stm32_simu_i2c_send(mpu->I2C, AK8963_ADDR, send_buf, 2);//AK8963每次读完以后都需要重新设置为单次测量模式
}

#endif

/***************************************************************************************************
*\Function      rt_hw_us_delay
*\Description   延迟us
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
void rt_hw_us_delay(rt_uint32_t us)
{
   rt_uint32_t delta;
   rt_uint32_t current_delay;
   us = us * (SysTick->LOAD/(1000000/RT_TICK_PER_SECOND));

   delta = SysTick->VAL;

  /* 循环获得当前时间，直到达到指定的时间后退出循环 */ 
    do 
    { 
        if ( delta > SysTick->VAL ) 
            current_delay = delta - SysTick->VAL; 
        else 
        /* 延时跨越了一次OS tick的边界时的处理 */ 
            current_delay = SysTick->LOAD + delta - SysTick->VAL; 
    } while( current_delay < us ); 
}


/***************************************************************************************************
*\Function      MPU_Set_Gyro_Fsr
*\Description   设置MPU9250陀螺仪传感器满量程范围
*\Parameter     fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
*\Return        返回值:0,成功  其他,错误代码
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_GYRO_CFG_REG,(fsr<<3)|3);//设置陀螺仪满量程范围  
}

/***************************************************************************************************
*\Function      MPU_Set_Accel_Fsr
*\Description   设置MPU9250加速度传感器满量程范围
*\Parameter     fsr:0,±2g;1,±4g;2,±8g;3,±16g
*\Return        返回值:0,成功  其他,错误代码
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围  
}

/***************************************************************************************************
*\Function      MPU_Set_LPF
*\Description   设置MPU9250的数字低通滤波器
*\Parameter     lpf:数字低通滤波频率(Hz)
*\Return        返回值:0,成功  其他,错误代码
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU9250_ADDR,MPU_CFG_REG,data);//设置数字低通滤波器  
}

/***************************************************************************************************
*\Function      MPU_Set_Rate
*\Description   设置MPU9250的采样率(假定Fs=1KHz)
*\Parameter     rate:4~1000(Hz)
*\Return        返回值:0,成功  其他,错误代码
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU9250_ADDR,MPU_SAMPLE_RATE_REG,data);	//设置数字低通滤波器
 	return MPU_Set_LPF(rate/2);	//自动设置LPF为采样率的一半
}

/***************************************************************************************************
*\Function      MPU_Get_Temperature
*\Description   得到温度值
*\Parameter     
*\Return        返回值:温度值(扩大了100倍)
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
short MPU_Get_Temperature(void)
{
    u8 buf[2]; 
    short raw;
	float temp;
	MPU_Read_Len(MPU9250_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw=((u16)buf[0]<<8)|buf[1];  
    temp=21+((double)raw)/333.87;  
    return temp*100;;
}

/***************************************************************************************************
*\Function      MPU_Get_Gyroscope
*\Description   得到陀螺仪值(原始值)
*\Parameter     gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
*\Return        返回值:0,成功  其他,错误代码
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6],res; 
	res=MPU_Read_Len(MPU9250_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((u16)buf[0]<<8)|buf[1];  
		*gy=((u16)buf[2]<<8)|buf[3];  
		*gz=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;;
}

/***************************************************************************************************
*\Function      MPU_Get_Accelerometer
*\Description   得到加速度值(原始值)
*\Parameter     gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
*\Return        返回值:0,成功  其他,错误代码
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6],res;  
	res=MPU_Read_Len(MPU9250_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((u16)buf[0]<<8)|buf[1];  
		*ay=((u16)buf[2]<<8)|buf[3];  
		*az=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;;
}

/***************************************************************************************************
*\Function      MPU_Get_Magnetometer
*\Description   得到磁力计值(原始值)
*\Parameter     mx,my,mz:磁力计x,y,z轴的原始读数(带符号)
*\Return        返回值:0,成功  其他,错误代码
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
u8 MPU_Get_Magnetometer(short *mx,short *my,short *mz)
{
    u8 buf[6],res;  
 	res=MPU_Read_Len(AK8963_ADDR,MAG_XOUT_L,6,buf);
	if(res==0)
	{
		*mx=((u16)buf[1]<<8)|buf[0];  
		*my=((u16)buf[3]<<8)|buf[2];  
		*mz=((u16)buf[5]<<8)|buf[4];
	} 	 
	MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11); //AK8963每次读完以后都需要重新设置为单次测量模式
    return res;;
}

/***************************************************************************************************
*\Function      mpu_read_byte
*\Description   读取一个字节
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
uint8_t mpu_read_byte(MPU9250_type* mpu_dev, uint8_t addr, uint8_t reg)
{
	 uint8_t result;
	
#ifdef USE_DRV_SIMU_I2C_C
	
	if(stm32_simu_i2c_recv(mpu9250_dev_pointer->I2C, addr<<1, reg, &result, 1) > 0)
	{
		return result;
	}
	else
	{
		return 1;
	}	
	
#else
	
    i2c_Start(mpu_dev->I2C);
    i2c_SendByte(mpu_dev->I2C, (addr<<1)|0); //发送器件地址+写命令
    i2c_WaitAck(mpu_dev->I2C);             //等待应答
    i2c_SendByte(mpu_dev->I2C, reg);         //写寄存器地址
    i2c_WaitAck(mpu_dev->I2C);             //等待应答
	i2c_Start(mpu_dev->I2C);                
    i2c_SendByte(mpu_dev->I2C, (addr<<1)|1); //发送器件地址+读命令
    i2c_WaitAck(mpu_dev->I2C);             //等待应答
	result = i2c_ReadByte(mpu_dev->I2C);//读数据,发送nACK 
	i2c_NAck(mpu_dev->I2C);
    i2c_Stop(mpu_dev->I2C);                 //产生一个停止条件
    return result; 	
#endif	
}

/***************************************************************************************************
*\Function      mpu_write_byte
*\Description   写入一个字节
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
uint8_t mpu_write_byte(MPU9250_type* mpu_dev, uint8_t addr, uint8_t reg, uint8_t data)
{
#ifdef USE_DRV_SIMU_I2C_C
	
	uint8_t send_buf[10];
	send_buf[0] = reg;
	memcpy(send_buf + 1, &data, 1);
	if(stm32_simu_i2c_send(mpu9250_dev_pointer->I2C, addr<<1, send_buf, 2) > 0)	
	{
		return 0;
	}
	else
	{
		return 1;
	}	
	
#else
	
    i2c_Start(mpu_dev->I2C);
    i2c_SendByte(mpu_dev->I2C, (addr<<1)|0); //发送器件地址+写命令
    if(i2c_WaitAck(mpu_dev->I2C))          //等待应答
    {
        i2c_Stop(mpu_dev->I2C);
        return 1;
    }
    i2c_SendByte(mpu_dev->I2C, reg);         //写寄存器地址
    i2c_WaitAck(mpu_dev->I2C);             //等待应答
    i2c_SendByte(mpu_dev->I2C, data);        //发送数据
    if(i2c_WaitAck(mpu_dev->I2C))          //等待ACK
    {
        i2c_Stop(mpu_dev->I2C);
        return 1;
    }
    i2c_Stop(mpu_dev->I2C);
    return 0;
	
#endif	
}

/***************************************************************************************************
*\Function      mpu_read_len
*\Description   读取多字节
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
uint8_t mpu_read_len(MPU9250_type* mpu_dev, uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{ 
#ifdef USE_DRV_SIMU_I2C_C

    if(stm32_simu_i2c_recv(mpu9250_dev_pointer->I2C, addr<<1, reg, buf, len) > 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
	
#else
	
    i2c_Start(mpu_dev->I2C);
    i2c_SendByte(mpu_dev->I2C, (addr<<1)|0); //发送器件地址+写命令
    if(i2c_WaitAck(mpu_dev->I2C))          //等待应答
    {
        i2c_Stop(mpu_dev->I2C);
        return 1;
    }
    i2c_SendByte(mpu_dev->I2C, reg);         //写寄存器地址
    i2c_WaitAck(mpu_dev->I2C);             //等待应答
	i2c_Start(mpu_dev->I2C);                
    i2c_SendByte(mpu_dev->I2C, (addr<<1)|1); //发送器件地址+读命令
    i2c_WaitAck(mpu_dev->I2C);             //等待应答
    while(len)
    {
        if(len==1)
		{
			*buf=i2c_ReadByte(mpu_dev->I2C);//读数据,发送nACK 
			i2c_NAck(mpu_dev->I2C);
		}
		else
		{
			*buf=i2c_ReadByte(mpu_dev->I2C);		//读数据,发送ACK  
			i2c_Ack(mpu_dev->I2C);
		}
		
		len--;
		buf++;  
    }
    i2c_Stop(mpu_dev->I2C);                 //产生一个停止条件
    return 0; 
	
#endif	
}

/***************************************************************************************************
*\Function      mpu_read_len
*\Description   写入多字节
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
uint8_t mpu_write_len(MPU9250_type* mpu_dev, uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
#ifdef USE_DRV_SIMU_I2C_C
	
	uint8_t send_buf[10];
	send_buf[0] = reg;
	memcpy(send_buf + 1, buf, len);
	if(stm32_simu_i2c_send(mpu9250_dev_pointer->I2C, addr<<1, send_buf, len + 1) > 0)	
	{
		return 0;
	}
	else
	{
		return 1;
	}	
	
#else
	
    uint8_t i;
    i2c_Start(mpu_dev->I2C);
    i2c_SendByte(mpu_dev->I2C, (addr<<1)|0); //发送器件地址+写命令
    if(i2c_WaitAck(mpu_dev->I2C))          //等待应答
    {
        i2c_Stop(mpu_dev->I2C);
        return 1;
    }
    i2c_SendByte(mpu_dev->I2C, reg);         //写寄存器地址
    i2c_WaitAck(mpu_dev->I2C);             //等待应答
    for(i=0;i<len;i++)
    {
        i2c_SendByte(mpu_dev->I2C, buf[i]);  //发送数据
        if(i2c_WaitAck(mpu_dev->I2C))      //等待ACK
        {
            i2c_Stop(mpu_dev->I2C);
            return 1;
        }
    }
    i2c_Stop(mpu_dev->I2C);
    return 0;	
	
#endif
	
}

/***************************************************************************************************
*\Function      MPU_Write_Len
*\Description   写入多字节  DMP外部调用
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
	mpu_write_len(mpu9250_dev_pointer, addr, reg, len, buf);	
} 


/***************************************************************************************************
*\Function      MPU_Read_Len
*\Description   读取多字节  DMP外部调用
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
	mpu_read_len(mpu9250_dev_pointer, addr, reg, len, buf);
}


/***************************************************************************************************
*\Function      MPU_Write_Byte
*\Description   写入一个字节  DMP外部调用
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
u8 MPU_Write_Byte(u8 addr,u8 reg,u8 data)
{
	return mpu_write_byte(mpu9250_dev_pointer, addr, reg, data);
}


/***************************************************************************************************
*\Function      MPU_Read_Byte
*\Description   读取一个字节  DMP外部调用
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
u8 MPU_Read_Byte(u8 addr,u8 reg)
{
    return mpu_read_byte(mpu9250_dev_pointer, addr, reg);  
}


/***************************************************************************************************
*\Function      mpu9250_init
*\Description   设备初始化
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    杨贝贝            
				创建函数。
***************************************************************************************************/
u8 mpu9250_init(MPU9250_type* mpu)
{
	u8 res=0;

    if (mpu == RT_NULL || mpu->I2C == RT_NULL)
    {
        rt_kprintf("MPU9250 NULL!\n");
    }

    mpu->lock = rt_mutex_create("MPU_Mut", RT_IPC_FLAG_FIFO);
    if (mpu->lock == RT_NULL)
    {
        rt_kprintf("mpu mutex create fail!\n");
    }

	mpu9250_dev_pointer = mpu;
	
#ifdef USE_DRV_SIMU_I2C_C
    stm32_simu_i2c_init(mpu->I2C);	
#else	
	bsp_InitI2C(mpu->I2C);
#endif
	
	if(MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X80))//复位MPU9250
	{
		rt_kprintf("mpu9250_init err 1 \r\n");
	}
    
    delay_ms(100);  //延时100ms
	
    if(MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X00))//唤醒MPU9250
	{
		rt_kprintf("mpu9250_init err 2 \r\n");
	}		
    MPU_Set_Gyro_Fsr(3);					        	//陀螺仪传感器,±2000dps
	MPU_Set_Accel_Fsr(0);					       	 	//加速度传感器,±2g
    MPU_Set_Rate(50);						       	 	//设置采样率50Hz
    if(MPU_Write_Byte(MPU9250_ADDR,MPU_INT_EN_REG,0X00))   //关闭所有中断
	{
		rt_kprintf("mpu9250_init err 3 \r\n");
	}
	if(MPU_Write_Byte(MPU9250_ADDR,MPU_USER_CTRL_REG,0X00))//I2C主模式关闭
	{
		rt_kprintf("mpu9250_init err 4 \r\n");
	}
	if(MPU_Write_Byte(MPU9250_ADDR,MPU_FIFO_EN_REG,0X00))	//关闭FIFO
	{
		rt_kprintf("mpu9250_init err 5 \r\n");
	}
	if(MPU_Write_Byte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0X82))//INT引脚低电平有效，开启bypass模式，可以直接读取磁力计
	{
		rt_kprintf("mpu9250_init err 6 \r\n");
	}
    res=MPU_Read_Byte(MPU9250_ADDR,MPU_DEVICE_ID_REG);  //读取MPU6500的ID
	rt_kprintf("MPU6500_ID:%02x\n", res);
    if(res==MPU6500_ID1||res==MPU6500_ID2) 				//器件ID正确
    {
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X01);  	//设置CLKSEL,PLL X轴为参考
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT2_REG,0X00);  	//加速度与陀螺仪都工作
		MPU_Set_Rate(50);						       			//设置采样率为50Hz   
		
		rt_kprintf("MPU6500 OK!\n");
    }
	
 
    res=MPU_Read_Byte(AK8963_ADDR,MAG_WIA);    			//读取AK8963 ID   
	rt_kprintf("AK8963_ID:%02x\n", res);
    if(res==AK8963_ID)
    {
        MPU_Write_Byte(AK8963_ADDR,MAG_CNTL2,0X01);		//复位AK8963
		delay_ms(50);
        MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11);		//设置AK8963为单次测量
		
		rt_kprintf("AK8963 OK!\n");
    }

    
	
	/*如果使用DMP，则获取不到磁传感器的数据*/
	if(mpu_dmp_init()) 
	{
		rt_kprintf("DMP failed!\n");
	}
	else
	{
		rt_kprintf("DMP OK!\n");
	}
	
	return 0;
}
