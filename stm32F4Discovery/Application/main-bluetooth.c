#include <stdio.h>
#include <string.h>
#include <rtl.h>

#include "stm32f4xx.h"
#include "spbt2632.h"
#include "leds.h"
#include "sinwave.h"

int sensor;

static __task void main_task(void)
{
	char buffer[34];
	int counter, i;
	
	ledsInit();
	bluetoothConfig();
	bluetoothOpen();
	
  while (1){
		ledToggle(GREEN);

		if(getBtStatus() == BT_CONNECTED){
			ledOn(BLUE);
			
			sensor = (sinwave[counter++]-1024) >> 6;
			memset(buffer, 0, sizeof(buffer));
			for(i=0; i<32; i++){
				if(sensor == i)
					buffer[i] = '*';
				else
					buffer[i] = ' ';
			}
			buffer[32] = '\n';
			buffer[33] = '\r';

			counter %= 64;
			
			bluetoothSendData((uint8_t*)buffer, 34);
		}
		else{
			ledOff(BLUE);
		}
    
		/* Insert delay */
		os_dly_wait(5);
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


