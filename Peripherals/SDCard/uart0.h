#ifndef _UART0_H_
#define _UART0_H_

#include <stdint.h>
#include "systemInit.h"
#include "stdutils.h"

#define SBIT_WordLenght    0x00u
#define SBIT_DLAB          0x07u
#define SBIT_FIFO          0x00u
#define SBIT_RxFIFO        0x01u
#define SBIT_TxFIFO        0x02u

#define TX0_PINSEL            0	 // P0.0 Pin on Port 0 
#define RX0_PINSEL            2	 // P0.2 Pin on Port 0 
#define SBIT_RDR           0x00u
#define SBIT_THRE          0x05u


/* Function to initialize the UART0 at specifief baud rate */
void uart_init(uint32_t baud_rate);

/* Function to transmit a char */
void uart_TxChar(char);

/* Function to Receive a char */
char uart_RxChar(void);

#endif //_UART0_H_
