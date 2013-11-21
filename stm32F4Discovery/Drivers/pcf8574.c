/**
 * Extremely simple driver for NXP PCF8574 Port Expander device
 * 
 * Pinout
 * I2C1_SCL   PB6
 * I2C1_SDA   PB9
 */


#include "STM32F4xx.h"
#include "pcf8574.h"

#define PCF8574_ADDRESS 0x70

int pcf8574Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	/* Enable GPIOB peripheral clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* I2C SCL and SDA pins configuration I2C1 on GPIOB6, GPIOB9 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	     
	/* Connect pins to I2C peripheral */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);  
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);


	/* Enable the I2C peripheral clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  
	/* pcf8574_I2C peripheral configuration */
	I2C_DeInit(I2C1);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x33;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 10000;

	/* Enable the I2C peripheral */
	I2C_Cmd(I2C1, ENABLE);  
	I2C_Init(I2C1, &I2C_InitStructure);
   
	return 0;
}



uint32_t pcf8574Write(uint8_t data)
{
  /*!< While the bus is busy */
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

  /* Start the config sequence */
  I2C_GenerateSTART(I2C1, ENABLE);

  /* Test on EV5 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(I2C1, PCF8574_ADDRESS, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  /* Write data */
  I2C_SendData(I2C1, data);

  /* Test on EV8 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));

  /* End the configuration sequence */
  I2C_GenerateSTOP(I2C1, ENABLE);  
  
  return 0;  
}
