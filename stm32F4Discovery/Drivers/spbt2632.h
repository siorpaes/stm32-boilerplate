/** \file spbt2632.h
 *
 */

#include <stdint.h>


#define RXBUFLEN 128
extern uint8_t uart_rx_buffer[];

/* Public APIs for Bluetotoh module management */
#define BT_UNKNOWN      0
#define BT_DISCONNECTED 1
#define BT_CONNECTED    2

int bluetoothConfig(void);
int bluetoothOpen(void);
int bluetoothClose(void);
int bluetoothSendData(uint8_t* buffer, int length);
int getBtStatus(void);
int setBtName(void);

/* Private APIs for UART management */
void uartConfig(void);
void uartReconf(int baudRate);
void uartWrite(uint8_t* buffer, int length);
void uartDmaWrite(uint8_t* buffer, int length);
void uartIrq(void);
void dmaIrq(void);

/* Private API for reset signal configuration */
void resetConfig(void);


/* Error codes */
#define BT_NOERR                0
#define BT_INVALID_TASK        -1
#define BT_BUSY                -2
#define BT_NOTCONNECTED        -3
#define BT_INVALIDNAME				 -5
#define BT_INVALIDSTATE        -6


