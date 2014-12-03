/** @file lis3dh.c
 * @author david.siorpaes@st.com
 * LIS3DH accelerometer device driver. Uses SPI2 interface.
 * See spiConfig() documentation for port pin configuration. 
 */
 

#include "stm32f4xx.h"
#include "lis3dh.h"
#include "lis3dh_reg.h"


/** @defgroup LIS3DH device driver public APIs
 * @{
 */
 
 
 /** @brief Initializes LIS3DH device with following configuration:
  * 100Hz sampling rate
  * Enable all axis
	* @retval None
  */
void lis3dhConfig(void)
{
	uint8_t val;
	
	/* Configure SPI pins and port */
	spiConfig();
	
	/* Configure LIS3DH device: enable X, Y, Z axis, se 100Hz sampling rate */
	val = CTRL_REG1_XEN | CTRL_REG1_YEN | CTRL_REG1_ZEN | CTRL_REG1_ODR_100;
	spiWrite(CTRL_REG1, &val, 1);
}


/** @brief Returns current acceleration value
 * @param x: pointer where to store X acceleration value
 * @param y: pointer where to store Y acceleration value
 * @param z: pointer where to store Z acceleration value
 * @retval always 0
 */
int lis3dhGetAcc(int16_t* x, int16_t* y, int16_t* z)
{
	uint8_t buffer[6];
	
	/* Read out all 6 bytes in one shot */
	spiRead(OUT_X_L, buffer, 6);

	*x = *((int16_t*)buffer);
	*y = *((int16_t*)(buffer + 2));
	*z = *((int16_t*)(buffer + 4));
	
	return 0;
}


/** @brief Set accelerometer range
 * @param range: range to be set. Can be one out of
 *               RANGE_2G, RANGE_4G, RANGE_8G, RANGE_16G
 */
void lis3dhSetRange(int8_t range)
{
	uint8_t regval;
	
	assert_param(range == RANGE_2G || range == RANGE_4G || range == RANGE_8G || range == RANGE_16G);
	
	regval = spiRead(CTRL_REG4, &regval, 1);
	regval &= ~(3 << 4);
	regval |= range;
	spiWrite(CTRL_REG4, &regval, 1);
}


/**
 * @}
 */


/** @defgroup LIS3DH device driver private APIs
 * @{
 */


/** @brief Set SPI Chip select OFF
 * @retval none
 */
void csOff(void)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_SET);
}


/** @brief Set SPI Chip select ON
 * @retval none
 */
void csOn(void)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_RESET);
}


/** @brief configures SPI2 pins as follows
 *  SPI2_CS      PB11
 *  SPI2_SCK     PB13
 *  SPI2_MISO    PB14
 *  SPI2_MOSI    PB15
 *
 *  Set SPI2 port in MODE1, clock 500kHz.
 * @retval Always 0
 */
int spiConfig(void)
{
	GPIO_InitTypeDef GPIO_struct;
	SPI_InitTypeDef SPI_InitStructure;
	
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	
	
	/* Configure SPI2 signals: SCK, MISO, MOSI */
	GPIO_StructInit(&GPIO_struct);
	GPIO_struct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_struct.GPIO_OType = GPIO_OType_PP;
	GPIO_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_struct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_struct);

	/* SPI2 pin mappings */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
	
	/* Configure SPI2 CS signal */
	GPIO_StructInit(&GPIO_struct);
	GPIO_struct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_struct.GPIO_OType = GPIO_OType_PP;
	GPIO_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_struct.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_struct);

	/* Enable SPI2 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	/* Configure SPI2 port */
	SPI_StructInit(&SPI_InitStructure);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; /* Using PB11 as CS */
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(SPI2, &SPI_InitStructure);

	/* CS Off */
	csOff();

	/* SPI2 enable */
	SPI_Cmd(SPI2, ENABLE);
		
	return 0;
}


/** @brief Reads given amount of data from sensor
 * @param addr: Register address to read from
 * @param buffer: Destination buffer
 * @param len: Number of bytes to be read
 * @retval Zero on success, -1 otherwise
 */
int spiRead(uint8_t addr, uint8_t* buffer, int len)
{
	if(len <= 0)
		return -1;
	
	/* It's a read operation */
	addr |= SPI_READ;
	
	/* It's a multiple read operation */
	if(len > 1)
		addr |= SPI_MULTI_ACCESS;
		
	/* Transmission start: pull CS low */
	csOn();
	
	/* Send address */
	while(SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET){}
	SPI_SendData(SPI2, addr);
			
	/* Dummy read to make sure shift register is empty.
	 * Note that TXE=1 just tells the Transmit Buffer is empty
	 * and therefore new data can be put in Data Register, not
	 * that actual data on Shift Register has all been put on wire.
	 */
	while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE)== RESET){}
	SPI_ReceiveData(SPI2);  

	while(len--){
		/* Dummy write */
		while(SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET){}
		SPI_SendData(SPI2, 0xff);
	      
		/* Actual read */
		while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE)== RESET){}
		*buffer++ = SPI_ReceiveData(SPI2);
	}

	/* Transmission end: pull CS high */
	csOff();

	return 0;
}


/** @brief Writes given amount of data to sensor
 * @param addr: Register address to write to
 * @param buffer: Source buffer
 * @param len: Number of bytes to be written
 * @retval Zero on success, -1 otherwise
 */
int spiWrite(uint8_t addr, uint8_t* buffer, int len)
{	
	if(len <= 0)
		return -1;
	
	/* It's a multiple read operation */
	if(len > 1)
		addr |= SPI_MULTI_ACCESS;
	
	/* Transmission start: pull CS low */
	csOn();
	
	/* Send address */
	while(SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET){}
	SPI_SendData(SPI2, addr);
	
	/* Wait data hits slave */ 
	while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE) == RESET);
	SPI_ReceiveData(SPI2);
		
	/* Send data */
	while(len--){
		while(SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET){}
		SPI_SendData(SPI2, *buffer++);
			
		while(SPI_GetFlagStatus(SPI2, SPI_FLAG_RXNE) == RESET);
		SPI_ReceiveData(SPI2);
	}

	/* Transmission end: pull CS high */
	csOff();
	
	return 0;
}

/**
 * @}
 */
