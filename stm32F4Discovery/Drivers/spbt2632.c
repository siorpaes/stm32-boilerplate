/** \file spbt2632.c
 * \author david.siorpaes@st.com
 * 
 * Simple driver for SPBT2632C2A Bluetooth module.
 * Uses UART2 as communication port.
 * 
 * Reset   PA8
 * TX      PA2
 * RX      PA3
 *
 * Includes low level UART2 driver including APIs for sending data
 * in poll and DMA modes and receiving data in interrupt mode.
 *
 * Usage:
 *
 * -Initialize Bleutooth driver with bluetoothConfig()
 * -Open Bluetooth driver with bluetoothOpen()
 * -Driver waits for incoming Bluetooth connections
 * -Use getBtConnectionStatus() to check whether device has been 'paged'
 * -Use bluetoothSendData() to send data to master device
 * -Use bluetoothClose() to close driver
 */

#include <string.h>
#include <stdio.h>
#include <rtl.h>

#include "stm32f4xx_usart.h"
#include "stm32f4xx_dma.h"
#include "spbt2632.h"

uint8_t uart_rx_buffer[RXBUFLEN];
static unsigned int rxWritePos;

/* Bluetooth module BDADDR */
char bdaddr[13];

static int bluetoothState;
OS_SEM bluetoothSem;


/** @defgroup Bluetooth public APIs
 * @{
 */


/** @brief Initializes Bluetooth module. Configures UART port and reset signal.
 * @param None
 * @note  Must be called in priviliged mode
 * @retval Always BT_NOERR
 */
int bluetoothConfig(void)
{
	memset(bdaddr, 0, sizeof(bdaddr));
	
	/* Configure reset pin */
	resetConfig();
	
	/* Configure UART */
	uartConfig();
	
	/* Reset module */
	GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_RESET);
	
	/* Hold reset for 10ms (5ms as per spec) */
	os_dly_wait(1);
	
	/* Leave reset */
	GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
			
	bluetoothState = BT_UNKNOWN;
	
	return BT_NOERR;
}


/** @brief Opens Bluetooth driver
 * @retval 0 on success
 *         Error code value if not successful
 */ 
int bluetoothOpen(void)
{
	int res = BT_NOERR;
	
	/* Enter critical section */
	tsk_lock();

	os_sem_init(&bluetoothSem, 0);
	
	/* Exit from critical section */
	tsk_unlock();

	return res;
}


/** @brief Sends data over Bluetooth
 * @param buffer: Data buffer to be sent
 * @param len: Length of buffer to be sent
 * @retval Number of transferred bytes if Bluetotoh is connected
 *         BT_NOTCONNECTED if Bluetooth is not connected
 */
int bluetoothSendData(uint8_t* buffer, int len)
{
	if(bluetoothState == BT_CONNECTED){
		uartDmaWrite(buffer, len);
		return len;
	}
	
	return BT_NOTCONNECTED;
}


/** @brief Returns Bluetooth module state
 * @retval Bluetooth state [BT_UNKNOWN/BT_CONNECTED/BT_DISCONNECTED]
 */
int getBtStatus(void)
{
	return bluetoothState;
}


/** @brief Sets the name of the Bluetooth module to FIT1-[BDADDR]
 * @retval  BT_INVALIDSTATE if module is not in BT_DISCONNECTED state
 *          BT_NOERR on success
 *         
 */
int setBtName(void)
{
	char buffer[48];
	
	if(getBtStatus() != BT_DISCONNECTED)
		return BT_INVALIDSTATE;
	
	memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "AT+AB LocalName FIT1-%s\n", bdaddr);
	printf("Setting device name to: FIT1-%s\n", bdaddr);
	
	uartDmaWrite((uint8_t*)buffer, strlen(buffer));

	return BT_NOERR;
}

/**
 * @}
 */



/** @defgroup UART low level APIs
 * @{
 */

/**
 * @brief Initializes UART2 with following pinout:
 *        USART2 TX    PA2
 *        USART2 RX    PA3
 * @param None
 * @retval None
 * @note  Must be called in priviliged mode
 */ 
void uartConfig(void)
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

	/* Configure UART2 RX interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	/* Following two parameters to be checked */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	/* Enable USART */
	USART_Cmd(USART2, ENABLE);

	rxWritePos = 0;
}


/** @brief Reconfigures UART to new baudrate
 * @param baudRate New baud rate
 * retval None
 */
void uartReconf(int baudRate)
{
	USART_InitTypeDef USART_InitStructure;
	char command [64];
	
	memset(command, 0, sizeof(command));
	sprintf(command, "AT+AB ChangeBaud %i\n", baudRate);
	
	os_dly_wait(10 * 100);
	uartDmaWrite((uint8_t*)command, strlen(command));
	os_dly_wait(10 * 100);
	
	/* Initialize UART3 at new baud rate, 8N1 */
	USART_Cmd(USART2, DISABLE);
	USART_DeInit(USART2);
	USART_InitStructure.USART_BaudRate = baudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	os_dly_wait(10 * 100);
}


/** @brief Sends data over UART3 using polling
 * @param pOutBuffer: Data buffer to be sent
 * @param ByteToWrite: Length of buffer to be sent
 * @retval None
 */
void uartWrite(uint8_t* buffer, int length)
{
	unsigned int counter;
				     
	for(counter = 0; counter < length; counter++){
		USART_SendData(USART2, (uint8_t)*(buffer + counter));	
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE)== RESET);   	
	}
}


/** @brief Sends data over UART3 using DMA and waits for transfer completion
 * @param buffer: Data buffer to be sent
 * @param length: Length of buffer to be sent
 */
void uartDmaWrite(uint8_t* buffer, int length)
{
	DMA_InitTypeDef DMA_InitStructure;

	USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);
	DMA_StructInit(&DMA_InitStructure);
	DMA_DeInit(DMA1_Stream6);
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = length;
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

	/* Wait for transfer completion */
	os_sem_wait(&bluetoothSem, 0xffff);
}


/** @brief UART2 RX interrupt handler
 * updates Bluetooth module state machine according to incoming response strings
 * @param None
 * @retval None
 */
void USART2_IRQHandler(void)
{
	uint16_t data;
	char* replyPos;
		
	data = USART_ReceiveData(USART2);
	uart_rx_buffer[rxWritePos] = data;
	rxWritePos++;
	rxWritePos %= RXBUFLEN;
		
	/* Spartan state machine management: identify end of module response and 
	 * distinguish among connected/disconnected states */
	if(data == '\n'){
		replyPos = strstr((char*)uart_rx_buffer, "AT-AB -BypassMode-");
		if(replyPos != NULL){
			bluetoothState = BT_CONNECTED;
			printf("Bluetooth connected\n");
		}
		else{		
			replyPos = strstr((char*)uart_rx_buffer, "AT-AB ConnectionDown");
			if(replyPos != NULL){
				bluetoothState = BT_DISCONNECTED;
				printf("Bluetooth disconnected\n");
			}
			else{
				replyPos = strstr((char*)uart_rx_buffer, "AT-AB BDAddress");
				if(replyPos != NULL){
					bluetoothState = BT_DISCONNECTED;
					printf("Bluetooth module reset\n");
					/* Store module's BDADDR */
					memcpy(bdaddr, replyPos + 16, 12);
				}
			}
		}
		/* Prepare for next module response */
		memset(uart_rx_buffer, 0, RXBUFLEN);
		rxWritePos = 0;
	}
	
	/* Cope with APB race */
	USART_GetITStatus(USART2, USART_IT_RXNE);
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
		isr_sem_send(&bluetoothSem);
	}
}


/**
 * @}
 */


/** @brief configure Bluetooth module reset pin to PA8
 * Since reset line is pulled up internally, use Open Drain mode.
 * @retval none
 */
void resetConfig(void)
{
	GPIO_InitTypeDef GPIO_struct;
	
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	/* Configure reset pin as open drain (reset is pulled up internally in module) */
	GPIO_StructInit(&GPIO_struct);
	GPIO_struct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_struct.GPIO_OType = GPIO_OType_OD;
	GPIO_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_struct.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA, &GPIO_struct);
}
