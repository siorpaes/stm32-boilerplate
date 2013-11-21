#include <stdio.h>
#include <string.h>
#include <rtl.h>

#include "stm32f4xx.h"
#include "m24lr.h"
#include "leds.h"
#include "button.h"

void Delay(__IO uint32_t nCount);

int8_t buffer[5];

static __task void main_task(void)
{
	int i = 0;
	
	ledsInit();
	buttonInit();
	
	/* Initialize M24LR memory */
	m24lrInit();
	
	os_dly_wait(100);
	
  while (1)
  {
		/* Read from 0 */
		buffer[0] = m24lrRead(0);
		buffer[1] = m24lrRead(1);
		buffer[2] = m24lrRead(2);
		buffer[3] = m24lrRead(3);
		buffer[4] = 0;
	
		printf("%s %i   [Press button to read again]\n", buffer, i++);
		   
    /* Wait for key press */
		while(getButton() == 0){
			os_dly_wait(1);
		}
		/* Anti bounce */
    os_dly_wait(30);
		
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


