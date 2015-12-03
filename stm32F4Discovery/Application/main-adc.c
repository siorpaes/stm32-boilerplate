#include <stdio.h>
#include <string.h>
#include <rtl.h>

#include "stm32f4xx.h"
#include "adc.h"
#include "leds.h"

void Delay(__IO uint32_t nCount);

unsigned int adcval;

static void drawBar(int val)
{
	char buffer[60];
	int i;
	
	val -= 240;
	
	for(i=0; i<60; i++){
		if(val*60/100 > i)
			buffer[i] = ']';
		else
			buffer[i] = 0;
	}
	
	printf("[%03i]%s\n", val, buffer);
}

static __task void main_task(void)
{
	ledsInit();
	
	/* Initialize ADC */
	ADC1_Config();
	
  while (1)
  {
		/* Run acquisition */
		adcval = ADC_SingleAcquisition();

		/* Show data */
		drawBar(adcval);
    
    /* Insert delay */
		os_dly_wait(1);
    
    /* Toggle some leds */
		ledToggle(ORANGE);
		ledToggle(BLUE);
  }
}


int main(void)
{
	/* Initialize and start the Real-Time eXecutive (RTX) kernel */
	os_sys_init_prio(main_task, 2);
	
	while(1);
}


#define USE_FULL_ASSERT
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


