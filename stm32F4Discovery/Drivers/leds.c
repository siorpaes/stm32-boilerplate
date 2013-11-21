#include "stm32f4xx.h"
#include "leds.h"

GPIO_TypeDef* ledPorts[] = {GPIOD, GPIOD, GPIOD, GPIOD};
unsigned int ledPins[] = {GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};

void ledsInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* GPIOD Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* Configure PD12, PD13, PD14 and PD15 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}


void ledOn(int led)
{
	if(led > 3)
		return;
	
	GPIO_SetBits(ledPorts[led], ledPins[led]);
}


void ledOff(int led)
{
	if(led > 3)
		return;
	
	GPIO_ResetBits(ledPorts[led], ledPins[led]);
}


void ledToggle(int led)
{
	if(led > 3)
		return;
	
	GPIO_ToggleBits(ledPorts[led], ledPins[led]);
}

