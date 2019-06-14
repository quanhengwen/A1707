/***************************************************************************************************
*                    (c) Copyright 2008-2018  Syncretize technologies co.,ltd.
*										All Rights Reserved
*
*\File          adc.c
*\Description   
*\Note          
*\Log           2019.02.12    Ver 1.0    baron
*               创建文件。
***************************************************************************************************/
#include "adc.h"
#include "app_board.h"
#include "param.h"

vu16 ADC_Buf[2];
float CHA_V_DET_Value = 0;
u16 PWRIN_DET_Value = 0;
/*
 * 函数名：ADC1_GPIO_Config
 * 描述  ：使能ADC1和DMA1的时钟，初始化PC.01
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);
	/* Configure PA.02/03  as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);				// PA23,输入时不用设置速率
}


/* 函数名：ADC1_Mode_Config
 * 描述  ：配置ADC1的工作模式为MDA模式
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void ADC1_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 //ADC地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_Buf;//(u32)&ADC_ConvertedValue;//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = ADC1_BUF_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//DMA_MemoryInc_Disable;  //内存地址固定
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	/* Enable DMA channel2 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	/* ADC1 configuration */
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//独立ADC模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//DISABLE ; 	 //禁止扫描模式，扫描模式用于多通道采集
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//开启连续转换模式，即不停地进行ADC转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//不使用外部触发转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//采集数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = TOTAL_CONVERT_CH_NUM;	 	//要转换的通道数目1
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/*配置ADC时钟，为PCLK2的8分频，即9Hz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	/*配置ADC1的通道11为55.	5个采样周期，序列为1 */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 2, ADC_SampleTime_55Cycles5);
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);
	
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	/*复位校准寄存器 */   
	ADC_ResetCalibration(ADC1);
	/*等待校准寄存器复位完成 */
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	/* ADC校准 */
	ADC_StartCalibration(ADC1);
	/* 等待校准完成*/
	while(ADC_GetCalibrationStatus(ADC1));
	
	/* 由于没有采用外部触发，所以使用软件触发ADC转换 */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*
 * 函数名：ADC1_Init
 * 描述  ：无
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
}
/***************************************************************************************************
*\Function      adc_chapwr_thread_entry
*\Description   
*\Parameter     parameter
*\Return        void
*\Note          
*\Log           2019.02.12    Ver 1.0    baron
*               创建函数。
***************************************************************************************************/
void adc_chapwr_thread_entry(void* parameter)
{
	static float low_adc=0;
	while(1)
	{
		low_adc =(ADC_Buf[1]/4096.0*3318.0);	
		PWRIN_DET_Value = (u16)((0.963*low_adc + 17.139)*21.0);//电池电压检测
		realtime_param.voltage = PWRIN_DET_Value;
	  rt_thread_delay(10);
	  CHA_V_DET_Value =((float) ADC_Buf[0]/4096.0*3286.0);//*28.149/1.149; // 读取转换的AD值 mv 大于800mv即为连接充电器
	  rt_thread_delay(10);
		if(CHA_V_DET_Value > 800)
		{
		  state.charge_on = 1;
		}
		else
		{
		  state.charge_on = 0;
		}
		//电池放电测试
//		rt_thread_delay(600);
//		printf("电池电压：%dmV\n",PWRIN_DET_Value);
//		rt_thread_delay(10);
//		printf("放电电流：%dmA\n",(u16)max9611_data[0].Current);	
	}

}

/******************* (C) COPYRIGHT 2019 WildFire Team *****END OF FILE************/

