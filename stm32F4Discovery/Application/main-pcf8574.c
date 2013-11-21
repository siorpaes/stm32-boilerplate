/**
 * MAN74A segment/bit assignment
 *
 *  _6___
 *  |   |
 * 4| 5 |7
 *  ----|
 * 2|   |0
 *  | 1 |
 *  -----
 *        *3
 */

#include <stdio.h>
#include <string.h>
#include <rtl.h>

#include "stm32f4xx.h"
#include "pcf8574.h"
#include "leds.h"


/* Display coding of zero..nine, dot */
int8_t numbers[] = {0xd7, 0x81, 0xe6, 0xe3, 0xb1, 0x73, 0x37, 0xc1, 0xf7, 0xf1, 0x08};

int8_t data;

static __task void main_task(void)
{
	
	/* Initialize peripherals */
	ledsInit();
	pcf8574Init();
	
  while (1)
  {
		/* Send data to PCF8574 Port Expander */		
		pcf8574Write(numbers[data]);
		data++;
		data %= sizeof(numbers)/sizeof(numbers[0]);
		
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


