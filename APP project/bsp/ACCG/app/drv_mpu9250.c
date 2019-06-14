/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          drv_mpu9250.c
*\Description   
*\Note          
*\Log           2019.03.25    Ver 1.0    ���
*               �����ļ���
***************************************************************************************************/
#include "drv_mpu9250.h"
#include "inv_mpu.h"

MPU9250_type* mpu9250_dev_pointer;


#define USE_MUP_DMP   //���ʹ��MPU�ٷ���DMP�ͺ궨�壬����ʹ�������Ԫ����


//#define USE_DRV_SIMU_I2C_C   //���ʹ�� drv_simu_i2c.c �ļ����������ͺ궨��



void i2c_Start(stm32_simu_i2c_type* i2c_dev);
void i2c_Stop(stm32_simu_i2c_type* i2c_dev);

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
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitI2C
*	����˵��: ����I2C���ߵ�GPIO������ģ��IO�ķ�ʽʵ��
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitI2C(stm32_simu_i2c_type* i2c_dev)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	gpio_rcc_enable(i2c_dev->scl->gpio);/* ��GPIOʱ�� */
    gpio_rcc_enable(i2c_dev->sda->gpio);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* ��©���ģʽ */
	
	GPIO_InitStructure.GPIO_Pin = i2c_dev->scl->gpio_pin;
	GPIO_Init(i2c_dev->scl->gpio, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	/* ��©���ģʽ */
	
	GPIO_InitStructure.GPIO_Pin = i2c_dev->sda->gpio_pin;;
	GPIO_Init(i2c_dev->sda->gpio, &GPIO_InitStructure);

	/* ��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ */
	i2c_Stop(i2c_dev);
}


/***************************************************************************************************
*\Function      i2c_Delay
*\Description   I2C�ӳ�
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/	
static void i2c_Delay(void)
{
	uint8_t i;

	/*��
		CPU��Ƶ168MHzʱ�����ڲ�Flash����, MDK���̲��Ż�����̨ʽʾ�����۲Ⲩ�Ρ�
		ѭ������Ϊ5ʱ��SCLƵ�� = 1.78MHz (����ʱ: 92ms, ��д������������ʾ����̽ͷ���ϾͶ�дʧ�ܡ�ʱ��ӽ��ٽ�)
		ѭ������Ϊ10ʱ��SCLƵ�� = 1.1MHz (����ʱ: 138ms, ���ٶ�: 118724B/s)
		ѭ������Ϊ30ʱ��SCLƵ�� = 440KHz�� SCL�ߵ�ƽʱ��1.0us��SCL�͵�ƽʱ��1.2us

		��������ѡ��2.2Kŷʱ��SCL������ʱ��Լ0.5us�����ѡ4.7Kŷ����������Լ1us

		ʵ��Ӧ��ѡ��400KHz���ҵ����ʼ���
	*/
	for (i = 0; i < 3; i++);
}


/***************************************************************************************************
*\Function      i2c_Start
*\Description   CPU����I2C���������ź�
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
static void i2c_Start(stm32_simu_i2c_type* i2c_dev)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
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
*\Description   CPU����I2C����ֹͣ�ź�
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
static void i2c_Stop(stm32_simu_i2c_type* i2c_dev)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
	i2c_dev->sda->gpio->BRR = i2c_dev->sda->gpio_pin;
	i2c_dev->scl->gpio->BSRR = i2c_dev->scl->gpio_pin;
	i2c_Delay();
	i2c_dev->sda->gpio->BSRR = i2c_dev->sda->gpio_pin;
	i2c_Delay();
}

/***************************************************************************************************
*\Function      i2c_SendByte
*\Description   CPU��I2C�����豸����8bit����
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
static void i2c_SendByte(stm32_simu_i2c_type* i2c_dev, uint8_t _ucByte)
{
	uint8_t i;

	/* �ȷ����ֽڵĸ�λbit7 */
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
			 i2c_dev->sda->gpio->BSRR = i2c_dev->sda->gpio_pin; // �ͷ�����
		}
		_ucByte <<= 1;	/* ����һ��bit */
		i2c_Delay();
	}
}

/***************************************************************************************************
*\Function      i2c_ReadByte
*\Description   CPU��I2C�����豸��ȡ8bit����
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
uint8_t i2c_ReadByte(stm32_simu_i2c_type* i2c_dev)
{
	uint8_t i;
	uint8_t value;

	/* ������1��bitΪ���ݵ�bit7 */
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
*\Description   CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
*\Parameter     
*\Return        ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
uint8_t i2c_WaitAck(stm32_simu_i2c_type* i2c_dev)
{
	uint8_t re;

	i2c_dev->sda->gpio->BSRR = i2c_dev->sda->gpio_pin;	/* CPU�ͷ�SDA���� */
	i2c_Delay();
	i2c_dev->scl->gpio->BSRR = i2c_dev->scl->gpio_pin;	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
	i2c_Delay();
	if (((i2c_dev->sda->gpio->IDR & i2c_dev->sda->gpio_pin) != 0))	/* CPU��ȡSDA����״̬ */
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
*\Description   CPU����һ��ACK�ź�
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
void i2c_Ack(stm32_simu_i2c_type* i2c_dev)
{
	i2c_dev->sda->gpio->BRR = i2c_dev->sda->gpio_pin;	/* CPU����SDA = 0 */
	i2c_Delay();
	i2c_dev->scl->gpio->BSRR = i2c_dev->scl->gpio_pin;	/* CPU����1��ʱ�� */
	i2c_Delay();
	i2c_dev->scl->gpio->BRR = i2c_dev->scl->gpio_pin;
	i2c_Delay();
	i2c_dev->sda->gpio->BSRR = i2c_dev->sda->gpio_pin;	/* CPU�ͷ�SDA���� */
}

/***************************************************************************************************
*\Function      i2c_NAck
*\Description   CPU����1��NACK�ź�
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
void i2c_NAck(stm32_simu_i2c_type* i2c_dev)
{
	i2c_dev->sda->gpio->BSRR = i2c_dev->sda->gpio_pin;	/* CPU����SDA = 1 */
	i2c_Delay();
	i2c_dev->scl->gpio->BSRR = i2c_dev->scl->gpio_pin;	/* CPU����1��ʱ�� */
	i2c_Delay();
	i2c_dev->scl->gpio->BRR = i2c_dev->scl->gpio_pin;
	i2c_Delay();
}

/***************************************************************************************************
*\Function      i2c_CheckDevice
*\Description   ���I2C�����豸��CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
*\Parameter     Address���豸��I2C���ߵ�ַ
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
uint8_t i2c_CheckDevice(stm32_simu_i2c_type* i2c_dev,uint8_t _Address)
{
	uint8_t ucAck;

	if (((i2c_dev->sda->gpio->IDR & i2c_dev->sda->gpio_pin) != 0) && ((i2c_dev->scl->gpio->IDR & i2c_dev->scl->gpio_pin) != 0))
	{
		i2c_Start(i2c_dev);		/* ���������ź� */

		/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
//		i2c_SendByte(_Address | 1);
		ucAck = i2c_WaitAck(i2c_dev);	/* ����豸��ACKӦ�� */

		i2c_Stop(i2c_dev);			/* ����ֹͣ�ź� */

		return ucAck;
	}
	return 1;	/* I2C�����쳣 */
}


/***************************************************************************************************
*\Function      delay_us
*\Description   �ӳ�us
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD��ֵ	    	 
	ticks=nus*72; 						//��Ҫ�Ľ����� 
	told=SysTick->VAL;        				//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};
}
 

/***************************************************************************************************
*\Function      delay_ms
*\Description   �ӳ�ms
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
void delay_ms(u16 nms)
{
	delay_us(1000*nms);
}		


#if 0
/***************************************************************************************************
*\Function      mpu9250_set_accel_fsr
*\Description   ����MPU9250���ٶȴ����������̷�Χ
*\Parameter     fsr:0,��2g;1,��4g;2,��8g;3,��16g
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
uint32_t mpu9250_set_accel_fsr(MPU9250_type* mpu, uint8_t fsr)
{
	uint8_t send_buf[2];
	
	send_buf[0] = MPU_ACCEL_CFG_REG;
	send_buf[1] = fsr<<3;
	
	return stm32_simu_i2c_send(mpu->I2C, MPU9250_ADDR, send_buf, 2);//���ü��ٶȴ����������̷�Χ  
}

/***************************************************************************************************
*\Function      mpu9250_set_gyro_fsr
*\Description   ����MPU9250�����Ǵ����������̷�Χ
*\Parameter     fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
uint32_t mpu9250_set_gyro_fsr(MPU9250_type* mpu, uint8_t fsr)
{
	uint8_t send_buf[2];
	
	send_buf[0] = MPU_GYRO_CFG_REG;
	send_buf[1] = fsr<<3;
	
	return stm32_simu_i2c_send(mpu->I2C, MPU9250_ADDR, send_buf, 2);//���������Ǵ����������̷�Χ  
}

/***************************************************************************************************
*\Function      mpu9250_set_lpf
*\Description   ����MPU9250�����ֵ�ͨ�˲���
*\Parameter     lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
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
	
	return stm32_simu_i2c_send(mpu->I2C, MPU9250_ADDR, send_buf, 2);//�������ֵ�ͨ�˲���
}

/***************************************************************************************************
*\Function      mpu9250_set_rate
*\Description   ����MPU9250�Ĳ�����(�ٶ�Fs=1KHz)
*\Parameter     rate:4~1000(Hz)
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
uint32_t mpu9250_set_rate(MPU9250_type* mpu, uint16_t rate)
{
	uint8_t send_buf[2];
	
	send_buf[0] = MPU_SAMPLE_RATE_REG;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	send_buf[1]=1000/rate-1;
	
	stm32_simu_i2c_send(mpu->I2C, MPU9250_ADDR, send_buf, 2);//�������ֵ�ͨ�˲���
	return   mpu9250_set_lpf(mpu, rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}

/***************************************************************************************************
*\Function      mpu9250_get_temperature
*\Description   ����¶�ֵ
*\Parameter    
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
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
*\Description   ���������ֵ(ԭʼֵ)
*\Parameter    	gx,gy,gz:������x,y,z���ԭʼ����(������)
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
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
*\Description   ��ü��ٶ�ֵ(ԭʼֵ)
*\Parameter    	gx,gy,gz:������x,y,z���ԭʼ����(������)
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
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
*\Description   ��ô�����ֵ(ԭʼֵ)
*\Parameter    	mx,my,mz:������x,y,z���ԭʼ����(������)
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
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
	stm32_simu_i2c_send(mpu->I2C, AK8963_ADDR, send_buf, 2);//AK8963ÿ�ζ����Ժ���Ҫ��������Ϊ���β���ģʽ
}

#endif

/***************************************************************************************************
*\Function      rt_hw_us_delay
*\Description   �ӳ�us
*\Parameter     
*\Return        
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
void rt_hw_us_delay(rt_uint32_t us)
{
   rt_uint32_t delta;
   rt_uint32_t current_delay;
   us = us * (SysTick->LOAD/(1000000/RT_TICK_PER_SECOND));

   delta = SysTick->VAL;

  /* ѭ����õ�ǰʱ�䣬ֱ���ﵽָ����ʱ����˳�ѭ�� */ 
    do 
    { 
        if ( delta > SysTick->VAL ) 
            current_delay = delta - SysTick->VAL; 
        else 
        /* ��ʱ��Խ��һ��OS tick�ı߽�ʱ�Ĵ��� */ 
            current_delay = SysTick->LOAD + delta - SysTick->VAL; 
    } while( current_delay < us ); 
}


/***************************************************************************************************
*\Function      MPU_Set_Gyro_Fsr
*\Description   ����MPU9250�����Ǵ����������̷�Χ
*\Parameter     fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
*\Return        ����ֵ:0,�ɹ�  ����,�������
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_GYRO_CFG_REG,(fsr<<3)|3);//���������������̷�Χ  
}

/***************************************************************************************************
*\Function      MPU_Set_Accel_Fsr
*\Description   ����MPU9250���ٶȴ����������̷�Χ
*\Parameter     fsr:0,��2g;1,��4g;2,��8g;3,��16g
*\Return        ����ֵ:0,�ɹ�  ����,�������
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU9250_ADDR,MPU_ACCEL_CFG_REG,fsr<<3);//���ü��ٶȴ����������̷�Χ  
}

/***************************************************************************************************
*\Function      MPU_Set_LPF
*\Description   ����MPU9250�����ֵ�ͨ�˲���
*\Parameter     lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
*\Return        ����ֵ:0,�ɹ�  ����,�������
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
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
	return MPU_Write_Byte(MPU9250_ADDR,MPU_CFG_REG,data);//�������ֵ�ͨ�˲���  
}

/***************************************************************************************************
*\Function      MPU_Set_Rate
*\Description   ����MPU9250�Ĳ�����(�ٶ�Fs=1KHz)
*\Parameter     rate:4~1000(Hz)
*\Return        ����ֵ:0,�ɹ�  ����,�������
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU9250_ADDR,MPU_SAMPLE_RATE_REG,data);	//�������ֵ�ͨ�˲���
 	return MPU_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}

/***************************************************************************************************
*\Function      MPU_Get_Temperature
*\Description   �õ��¶�ֵ
*\Parameter     
*\Return        ����ֵ:�¶�ֵ(������100��)
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
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
*\Description   �õ�������ֵ(ԭʼֵ)
*\Parameter     gx,gy,gz:������x,y,z���ԭʼ����(������)
*\Return        ����ֵ:0,�ɹ�  ����,�������
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
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
*\Description   �õ����ٶ�ֵ(ԭʼֵ)
*\Parameter     gx,gy,gz:������x,y,z���ԭʼ����(������)
*\Return        ����ֵ:0,�ɹ�  ����,�������
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
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
*\Description   �õ�������ֵ(ԭʼֵ)
*\Parameter     mx,my,mz:������x,y,z���ԭʼ����(������)
*\Return        ����ֵ:0,�ɹ�  ����,�������
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
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
	MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11); //AK8963ÿ�ζ����Ժ���Ҫ��������Ϊ���β���ģʽ
    return res;;
}

/***************************************************************************************************
*\Function      mpu_read_byte
*\Description   ��ȡһ���ֽ�
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
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
    i2c_SendByte(mpu_dev->I2C, (addr<<1)|0); //����������ַ+д����
    i2c_WaitAck(mpu_dev->I2C);             //�ȴ�Ӧ��
    i2c_SendByte(mpu_dev->I2C, reg);         //д�Ĵ�����ַ
    i2c_WaitAck(mpu_dev->I2C);             //�ȴ�Ӧ��
	i2c_Start(mpu_dev->I2C);                
    i2c_SendByte(mpu_dev->I2C, (addr<<1)|1); //����������ַ+������
    i2c_WaitAck(mpu_dev->I2C);             //�ȴ�Ӧ��
	result = i2c_ReadByte(mpu_dev->I2C);//������,����nACK 
	i2c_NAck(mpu_dev->I2C);
    i2c_Stop(mpu_dev->I2C);                 //����һ��ֹͣ����
    return result; 	
#endif	
}

/***************************************************************************************************
*\Function      mpu_write_byte
*\Description   д��һ���ֽ�
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
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
    i2c_SendByte(mpu_dev->I2C, (addr<<1)|0); //����������ַ+д����
    if(i2c_WaitAck(mpu_dev->I2C))          //�ȴ�Ӧ��
    {
        i2c_Stop(mpu_dev->I2C);
        return 1;
    }
    i2c_SendByte(mpu_dev->I2C, reg);         //д�Ĵ�����ַ
    i2c_WaitAck(mpu_dev->I2C);             //�ȴ�Ӧ��
    i2c_SendByte(mpu_dev->I2C, data);        //��������
    if(i2c_WaitAck(mpu_dev->I2C))          //�ȴ�ACK
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
*\Description   ��ȡ���ֽ�
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
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
    i2c_SendByte(mpu_dev->I2C, (addr<<1)|0); //����������ַ+д����
    if(i2c_WaitAck(mpu_dev->I2C))          //�ȴ�Ӧ��
    {
        i2c_Stop(mpu_dev->I2C);
        return 1;
    }
    i2c_SendByte(mpu_dev->I2C, reg);         //д�Ĵ�����ַ
    i2c_WaitAck(mpu_dev->I2C);             //�ȴ�Ӧ��
	i2c_Start(mpu_dev->I2C);                
    i2c_SendByte(mpu_dev->I2C, (addr<<1)|1); //����������ַ+������
    i2c_WaitAck(mpu_dev->I2C);             //�ȴ�Ӧ��
    while(len)
    {
        if(len==1)
		{
			*buf=i2c_ReadByte(mpu_dev->I2C);//������,����nACK 
			i2c_NAck(mpu_dev->I2C);
		}
		else
		{
			*buf=i2c_ReadByte(mpu_dev->I2C);		//������,����ACK  
			i2c_Ack(mpu_dev->I2C);
		}
		
		len--;
		buf++;  
    }
    i2c_Stop(mpu_dev->I2C);                 //����һ��ֹͣ����
    return 0; 
	
#endif	
}

/***************************************************************************************************
*\Function      mpu_read_len
*\Description   д����ֽ�
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
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
    i2c_SendByte(mpu_dev->I2C, (addr<<1)|0); //����������ַ+д����
    if(i2c_WaitAck(mpu_dev->I2C))          //�ȴ�Ӧ��
    {
        i2c_Stop(mpu_dev->I2C);
        return 1;
    }
    i2c_SendByte(mpu_dev->I2C, reg);         //д�Ĵ�����ַ
    i2c_WaitAck(mpu_dev->I2C);             //�ȴ�Ӧ��
    for(i=0;i<len;i++)
    {
        i2c_SendByte(mpu_dev->I2C, buf[i]);  //��������
        if(i2c_WaitAck(mpu_dev->I2C))      //�ȴ�ACK
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
*\Description   д����ֽ�  DMP�ⲿ����
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
u8 MPU_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
	mpu_write_len(mpu9250_dev_pointer, addr, reg, len, buf);	
} 


/***************************************************************************************************
*\Function      MPU_Read_Len
*\Description   ��ȡ���ֽ�  DMP�ⲿ����
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
u8 MPU_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
	mpu_read_len(mpu9250_dev_pointer, addr, reg, len, buf);
}


/***************************************************************************************************
*\Function      MPU_Write_Byte
*\Description   д��һ���ֽ�  DMP�ⲿ����
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
u8 MPU_Write_Byte(u8 addr,u8 reg,u8 data)
{
	return mpu_write_byte(mpu9250_dev_pointer, addr, reg, data);
}


/***************************************************************************************************
*\Function      MPU_Read_Byte
*\Description   ��ȡһ���ֽ�  DMP�ⲿ����
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
***************************************************************************************************/
u8 MPU_Read_Byte(u8 addr,u8 reg)
{
    return mpu_read_byte(mpu9250_dev_pointer, addr, reg);  
}


/***************************************************************************************************
*\Function      mpu9250_init
*\Description   �豸��ʼ��
*\Parameter     hdc
*\Return        void
*\Note          
*\Log           2019.03.25    Ver 1.0    ���            
				����������
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
	
	if(MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X80))//��λMPU9250
	{
		rt_kprintf("mpu9250_init err 1 \r\n");
	}
    
    delay_ms(100);  //��ʱ100ms
	
    if(MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X00))//����MPU9250
	{
		rt_kprintf("mpu9250_init err 2 \r\n");
	}		
    MPU_Set_Gyro_Fsr(3);					        	//�����Ǵ�����,��2000dps
	MPU_Set_Accel_Fsr(0);					       	 	//���ٶȴ�����,��2g
    MPU_Set_Rate(50);						       	 	//���ò�����50Hz
    if(MPU_Write_Byte(MPU9250_ADDR,MPU_INT_EN_REG,0X00))   //�ر������ж�
	{
		rt_kprintf("mpu9250_init err 3 \r\n");
	}
	if(MPU_Write_Byte(MPU9250_ADDR,MPU_USER_CTRL_REG,0X00))//I2C��ģʽ�ر�
	{
		rt_kprintf("mpu9250_init err 4 \r\n");
	}
	if(MPU_Write_Byte(MPU9250_ADDR,MPU_FIFO_EN_REG,0X00))	//�ر�FIFO
	{
		rt_kprintf("mpu9250_init err 5 \r\n");
	}
	if(MPU_Write_Byte(MPU9250_ADDR,MPU_INTBP_CFG_REG,0X82))//INT���ŵ͵�ƽ��Ч������bypassģʽ������ֱ�Ӷ�ȡ������
	{
		rt_kprintf("mpu9250_init err 6 \r\n");
	}
    res=MPU_Read_Byte(MPU9250_ADDR,MPU_DEVICE_ID_REG);  //��ȡMPU6500��ID
	rt_kprintf("MPU6500_ID:%02x\n", res);
    if(res==MPU6500_ID1||res==MPU6500_ID2) 				//����ID��ȷ
    {
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT1_REG,0X01);  	//����CLKSEL,PLL X��Ϊ�ο�
        MPU_Write_Byte(MPU9250_ADDR,MPU_PWR_MGMT2_REG,0X00);  	//���ٶ��������Ƕ�����
		MPU_Set_Rate(50);						       			//���ò�����Ϊ50Hz   
		
		rt_kprintf("MPU6500 OK!\n");
    }
	
 
    res=MPU_Read_Byte(AK8963_ADDR,MAG_WIA);    			//��ȡAK8963 ID   
	rt_kprintf("AK8963_ID:%02x\n", res);
    if(res==AK8963_ID)
    {
        MPU_Write_Byte(AK8963_ADDR,MAG_CNTL2,0X01);		//��λAK8963
		delay_ms(50);
        MPU_Write_Byte(AK8963_ADDR,MAG_CNTL1,0X11);		//����AK8963Ϊ���β���
		
		rt_kprintf("AK8963 OK!\n");
    }

    
	
	/*���ʹ��DMP�����ȡ�����Ŵ�����������*/
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
