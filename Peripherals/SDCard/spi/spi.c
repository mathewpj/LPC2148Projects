
#include <stdint.h>
#include <lpc214x.h>
#include "spi.h"
#include "type.h"


/* SPI Init Function */

void SPI_Init(void)
{

	U32 spsr;

 	/* Enable SPI0 Block */
 	PCONP |= (1 << 10);

 	/* Configure other SSP pins: SCK0, MISO0, MOSI0, GPIO */
 	/* Dont configure P0.7 as SSEL0, this acts as a Input 
    	pin only and to be used only when the SPI0 is to be 
    	confgured as a slave */ 

	PINSEL0 |= (1 << 8);  // Enable SCK0
 	PINSEL0 |= (1 << 10); // Enable MISO0
 	PINSEL0 |= (1 << 12); // Enable MOSI0
 	//PINSEL0 |= (1 << 14); // Enable SSEL0

 	//
 	//  Set P0.7 as output, deselect by driving high
 	//
 	IODIR0 |= (1 << 7);
 	IODIR0 = (1<<7);


	/*  SPI configuration:
    	Master mode
    	Sample on 1st edge of SCK
    	SCK is active high
    	MSB out first 			: Bit 5
    	No interrupts
    	8 Bit
	*/

 	S0SPCR  = 1<<SPI_SPCR_MSTR; /* all others setting  dont require bits to be set */ 
  
 	/* Reading the status register clears it */
 	spsr = S0SPSR;
}

/**
  * @brief  Configure SSP0 clock rate.
  *
  * @param  SPI_CLOCKRATE: Specifies the SPI clock rate.
  *         The value should be SPI_CLOCKRATE_LOW or SPI_CLOCKRATE_HIGH.
  * @retval None 
  *
  * SSP0_CLK = CCLK / SPI_CLOCKRATE
  */
void SPI_ConfigClockRate (uint32_t SPI_CLOCKRATE)
{
    /* CPSR must be an even value between 2 and 254 */
    S0SPCCR = (SPI_CLOCKRATE & 0x1FE);
}

/**
  * @brief  Send one byte via MOSI and simutaniously receive one byte via MISO.
  *
  * @param  data: Specifies the byte to be sent out.
  * @retval Returned byte.
  *
  * Note: Each time send out one byte at MOSI, Rx FIFO will receive one byte. 
  */
uint8_t SPI_SendByte (uint8_t data)
{
    /* Put the data on the FIFO */
    S0SPDR = data;
    /* Wait for sending to complete */
    while (!(S0SPSR & SPI_SPSR_SPIF));
    /* Return the received value */
    return (S0SPDR);
}

uint8_t SPI_RecvByte (void)
{
    /* Send 0xFF to provide clock for MISO to receive one byte */
    return SPI_SendByte (0xFF);
}

/** Select the card */
#define SD_Select()  do {SPI_CS_Low();} while (0)
/** DeSelect the card */
#define SD_DeSelect() do { SPI_CS_High ();SPI_RecvByte();} while (0)


/**
  * @brief  Set SSEL to low: select spi slave.
  *
  * @param  None.
  * @retval None 
  */
void SPI_CS_Low (void)
{
  //
  //  Set P0.7 as output, select by driving low
  //
  IODIR0 |= 0x80;
  IOCLR0 |= 0x80;
 
}

/**
  * @brief  Set SSEL to high: de-select spi slave.
  *
  * @param  None.
  * @retval None 
  */
void SPI_CS_High (void)
{

  //
  //  Set P0.7 as output, deselect by driving high
  //
  IODIR0 |= 0x80;
  IOSET0 |= 0x80;

}



