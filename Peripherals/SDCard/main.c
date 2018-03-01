//https://www.youtube.com/watch?v=0rgEd3vAclc
//http://bikealive.nl/sd-v2-initialization.html
//https://luckyresistor.me/cat-protector/software/sdcard-2/
//http://www.dejazzer.com/ee379/lecture_notes/lec12_sd_card.pdf
//http://www.transcend-info.com/products/images/modelpic/539/SDHC10I_datasheet.pdf

#include <stdio.h>
#include <stdlib.h>
#include "LPC214x.H"	// LPC2148 MPU Register
#include "type.h"
#include "uart0.h"
#include "spi.h"
#include "sd.h"
#include "diskio.h"
#include "ff.h"
#include "debug.h"

uint8_t CardType;

extern char ascii_string[16];

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

main()
{
	
	char tp3[]="\n\r Return from f_mount(): ";	
	char tp4[]="\n\r Return from f_open(): ";	
	char tp5[]="\n\r Return from f_read(): ";		
	char tp6[]="\n\r Return from f_lseek(): ";	
	char tp7[]="\n\r Return from f_puts(): ";	
	uint32_t byte_read=0;
	char data_buffer[13];
	BYTE ret;
	
	static FATFS FATFS_Obj;
	FIL fil_obj;
	char write_data[] = "HELLO WORLD ";
	SystemInit();
   	uart_init(9600);  // Initialize the UART0 for 9600 baud rate
	
 	ret = f_mount(0, &FATFS_Obj);
	printString(tp3);
	printChar_BCD(ret);	
 
	ret = f_open((&fil_obj), "/WRITE.TXT", FA_READ|FA_WRITE);
	printString(tp4);
	printChar_BCD(ret);
					   
	ret = f_read((&fil_obj),data_buffer,11, &byte_read);
	data_buffer[12] = '\0'; 
	printString(tp5);
	printString(data_buffer);

	// Move the read/write pointer to the end of the file
    ret = f_lseek((&fil_obj), fil_obj.fsize);
	printString(tp6);
	printChar_BCD(ret);  
								
	ret = f_puts(write_data, &fil_obj);
	printString(tp7);
	printChar_BCD(ret);
 
   f_sync(&fil_obj);	
   f_close(&fil_obj);
   f_mount(0, NULL);
}


