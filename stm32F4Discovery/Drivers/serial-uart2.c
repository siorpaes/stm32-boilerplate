/** \file serial.c
 * Simple UART driver, TX only, with DMA
 */

#include <stdint.h>
#include <rtl.h>

#include "stm32f4xx_usart.h"
#include "stm32f4xx_dma.h"

#include "serial.h"

/* DMA transmission synchronization semaphore */
OS_SEM serialSem;


/** @brief Initializes UART2 port with following pinout:
 * UART2_TX   PA2
 * UART2_RX   PA3
 * Enables DMA TX mode (DMA1 Channel4) and DMA TX interrupt 
 * @param None
 * @note  Must be called in priviliged mode, i.e.: NOT from RTX task
 *        if OS_RUNPRIV is not set in RTX_Conf_CM.c
 * @retval Always 0
 */
int serialInit(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_struct;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
     
	/* Enable USART clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	/* UART pin mappings */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
  
	/* Configure USART Tx and Rx pins */
	GPIO_struct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_struct.GPIO_OType = GPIO_OType_PP;
	GPIO_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_struct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_struct);

	/* Initialize UART2 at 115200, 8N1 */
	USART_DeInit(USART2);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
  	
	/* Configure UART2 TX DMA */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	/* Configure DMA1_Stream6 Channel4 */
	DMA_StructInit(&DMA_InitStructure);
  DMA_DeInit(DMA1_Stream6);
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream6, &DMA_InitStructure);

		
	/* Enable DMA Transfer Complete interrupt */
	DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE);
	
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable USART */
	USART_Cmd(USART2, ENABLE);

	return 0;
}


/** @brief Initializes serial port
 * @note Must be called AFTER RTX scheduler has started
 * @retval Always 0
 */ 
int serialOpen(void)
{
	/* Initialize DMA semaphore */
	os_sem_init(&serialSem, 0);
	
	return 0;
}


/** @brief Sends data over serial line
 * @param buffer: Data buffer to be sent
 * @param len: Length of buffer to be sent
 * @retval Always 0
 */
int serialSendData(uint8_t* buffer, int len)
{
	DMA_InitTypeDef DMA_InitStructure;

#if 0
	USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);
	DMA1_Stream6->M0AR = (uint32_t)buffer;
	DMA1_Stream6->NDTR = len;
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Stream6, ENABLE);
#else
	USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);
	DMA_StructInit(&DMA_InitStructure);
	DMA_DeInit(DMA1_Stream6);
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = len;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream6, &DMA_InitStructure);
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Stream6, ENABLE);
	
	DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE);

#endif	
	/* Wait for transfer completion */
	os_sem_wait(&serialSem, 0xffff);
	
	return 0;
}


/** @brief Sends data over serial line in poll mode.
 * Used just for test purposes.
 * @param buffer: Data buffer to be sent
 * @param len: Length of buffer to be sent
 */
int serialSendDataPoll(uint8_t* buffer, int length)
{
	unsigned int counter;
				     
	for(counter = 0; counter < length; counter++){
		USART_SendData(USART2, (uint8_t)*(buffer + counter));	
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE)== RESET);   	
	}
	
	return 0;
}


/** @brief DMA UART TX done interrupt
 * acknowledges interrupt and signals waiting thread transmission completion
 * @retval none
 */
void DMA1_Stream6_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream6, DMA_IT_TCIF6)){
		DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
		
		/* Signal transfer completion */
		isr_sem_send(&serialSem);
	}
}

