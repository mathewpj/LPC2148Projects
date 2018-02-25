//https://www.youtube.com/watch?v=0rgEd3vAclc
//http://bikealive.nl/sd-v2-initialization.html
//https://luckyresistor.me/cat-protector/software/sdcard-2/
//http://www.dejazzer.com/ee379/lecture_notes/lec12_sd_card.pdf

#include <stdio.h>
#include <stdlib.h>
#include "LPC214x.H"	// LPC2148 MPU Register
#include "type.h"
#include "uart0.h"
#include "spi.h"
#include "sd.h"

uint8_t CardType;



/***********************/
/* Delay Time Function */
/*    1-4294967296     */
/***********************/
void delay(unsigned long int count1)
{
  while(count1 > 0) {count1--;}							// Loop Decrease Counter	
}

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/
void disk_initialize (void)
{
		int i;
		unsigned char tp1[]="\n\rTest Link Established before SD_Init";
		unsigned char tp2[]="\n\rTest Link Established after  SD_Init";		

		SystemInit();
    	uart_init(9600);  // Initialize the UART0 for 9600 baud rate

		for(i=0;tp1[i];i++)  //transmit a predefined string
        	uart_TxChar(tp1[i]);
					
        SD_Init(); 

		for(i=0;tp2[i];i++)  //transmit a predefined string
        	uart_TxChar(tp2[i]);

        return;
}

main()
{
	disk_initialize ();

}
