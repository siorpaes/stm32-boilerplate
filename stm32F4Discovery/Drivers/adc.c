/** Simple ADC driver
 *
 * ADC1 Channel8    PB0
 */

#include "stm32f4xx_adc.h"
#include "stm32f4xx_rcc.h"
#include "adc.h"

#define GPIO_PORT_ADCin1        (GPIOB)
#define GPIO_PIN_ADCin1         (GPIO_Pin_0)


void ADC1_Config(void)
{
	ADC_InitTypeDef   ADC_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Enable GPIO ports B */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	/* ADC1 Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	/* Configure Pin & Port as input push-pull for ADC channel 1 usage */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_ADCin1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIO_PORT_ADCin1, &GPIO_InitStructure);
	
	/* Reset ADC to default values */
	ADC_DeInit();

	/* ADC1 Configuration */
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_Cmd(ADC1, ENABLE);
}


uint16_t ADC_SingleAcquisition()
{
	uint16_t res;
	
	/* ADCx regular channel x configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_3Cycles);
	
	/* Start ADCx Software Conversion */ 
	ADC_SoftwareStartConv(ADC1);

	/* Wait until ADCx end of conversion */ 
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
	{
	}

	res = ADC_GetConversionValue(ADC1);

	return res;	
}



