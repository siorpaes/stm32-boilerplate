#include <stdio.h>
#include <string.h>
#include <rtl.h>

#include "stm32f4xx.h"
#include "lis3dh.h"
#include "leds.h"

int16_t x, y, z;

static __task void main_task(void)
{
	ledsInit();
	
	/* Initialize LIS3DH device */
	lis3dhConfig();
	
  while (1)
  {
		/* Acquire data from LIS3DH */
		lis3dhGetAcc(&x, &y, &z);
		printf("%i %i %i\n", x, y, z);
    
    /* Insert delay */
		os_dly_wait(10);
    
    /* Toggle some leds */
		ledToggle(GREEN);
		ledToggle(BLUE);
		ledOn(RED);
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


