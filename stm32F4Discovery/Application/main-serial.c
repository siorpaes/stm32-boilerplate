#include <stdio.h>
#include <string.h>
#include <rtl.h>

#include "stm32f4xx.h"
#include "serial.h"
#include "leds.h"

void Delay(__IO uint32_t nCount);

void pe3Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* GPIOD Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
}

static __task void main_task(void)
{
	ledsInit();
	serialInit();
	serialOpen();
	pe3Init();
	
	while(1){
		GPIO_ToggleBits(GPIOE, GPIO_Pin_3);
		os_dly_wait(10);
	}
	
	
  while (1)
  {
    ledOn(GREEN);
		
		//serialSendDataPoll("ciao\n\r", strlen("ciao\n\r"));
		serialSendData("blaaa\n\r", strlen("blaaa\n\r"));
		
    
    /* Insert delay */
		os_dly_wait(10);
    
		ledOn(ORANGE);
    
    /* Insert delay */
		os_dly_wait(10);
  
		ledOn(RED);
    
    /* Insert delay */
    os_dly_wait(10);

		ledOn(BLUE);
    
    /* Insert delay */
		os_dly_wait(10);
    
		ledOff(GREEN);
		ledOff(ORANGE);
		ledOff(RED);
		ledOff(BLUE);
    
    /* Insert delay */
		os_dly_wait(10);
		
		printf("Hi\n");
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


