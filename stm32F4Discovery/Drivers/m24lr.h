#include "STM32F4xx.h"

/* Public */
void m24lrInit(void);
uint8_t m24lrRead(uint32_t);

/* Private */
static int32_t Codec_WriteRegister(uint8_t RegisterAddr, uint8_t RegisterValue);
static int32_t Codec_ReadRegister(uint8_t RegisterAddr);
