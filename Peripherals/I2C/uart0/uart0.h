
#ifndef _UART0_H_
#define	_UART0_H_

#include <lpc214x.h>
#include <stdint.h>

#define TX0_PINSEL            0
#define RX0_PINSEL            2
#define SBIT_WordLenght    0x00u
#define SBIT_DLAB          0x07u
#define SBIT_FIFO          0x00u
#define SBIT_RxFIFO        0x01u
#define SBIT_TxFIFO        0x02u

#define pclk 60000000UL  // PCLK set to 60Mhz using PLL configuration(refer systemInit.c file)

void uart_init(uint32_t baudrate);
void UART0_TxChar(char ch);
void UART0_SendString(char* str);

#endif //_UART0_H_