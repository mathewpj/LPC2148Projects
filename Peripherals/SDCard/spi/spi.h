#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>
#include "type.h"

#define SPI_SPSR_SPIF 0x80
#define SPI_SPCR_MSTR 5

/* SPI clock rate setting. 
SSP0_CLK = SystemCoreClock / divider,
The divider must be a even value between 2 and 254!
In SPI mode, max clock speed is 20MHz for MMC and 25MHz for SD */
#define SPI_CLOCKRATE_LOW   (uint32_t) (150)   /* 60MHz / 150 = 400kHz */
#define SPI_CLOCKRATE_HIGH  (uint32_t) (4)     /* 100MHz / 4  = 15MHz  */

/* Function definitions */

//void SPI_Init(void);
void    SPI_Init (void);
void    SPI_ConfigClockRate (uint32_t SPI_CLOCKRATE);
void    SPI_CS_Low (void);
void    SPI_CS_High (void);
uint8_t SPI_SendByte (uint8_t data);
uint8_t SPI_RecvByte (void);

#endif

