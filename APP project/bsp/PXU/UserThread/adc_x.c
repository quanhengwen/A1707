/********************

ADC  采集 MAX4080  监测模块 电流  

**********************************************************************************/
#include "threadinit.h"
#include "app_board.h"


// ADC1转换的电压值通过MDA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue;
float value[M];
     
 void adc_thread_entry(void* parameter)
{	
	
  u8  i;
//	ADC1_Init();
	
	while (1)
	{
	 rt_thread_delay(300);
		
	 filter();    
	 for(i=0;i<3;i++)     
	{
	 value[i]= (float)(After_filter[i]/4096.0*3.3/5.0);  //各路电压值 电阻为 0.05R 再计算出电流值 //两位小数
		// printf("AD %d value = %f V",i,value[i]); 
  }	
	
	realtime_param.current_EX1=(u16)value[0];
	realtime_param.current_EX2=(u16)value[1];
	realtime_param.current    =(u16)value[2];
}
}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
