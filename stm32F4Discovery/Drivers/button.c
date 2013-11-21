#include "stm32f4xx.h"
#include "button.h"

/** Initializes PA0 as button input
 */
void buttonInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* GPIOA Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  /* Configure PA0 as input mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}


int getButton(void)
{	
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
}
