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
	char tp6[]="\n\r Result from f_lseek(): ";	
	char tp7[]="\n\r Result from f_write(): ";	
	uint32_t byte_read=0;
	uint8_t data_buffer[13];
	BYTE ret;
	
	static FATFS FATFS_Obj;
	FIL fil_obj;
	uint8_t write_data[] = "HELLO NXP";
	SystemInit();
   	uart_init(9600);  // Initialize the UART0 for 19200 baud rate
	
 	ret = f_mount(0, &FATFS_Obj);
	printString(tp3);
	printChar_BCD(ret);	
 
	ret = f_open((&fil_obj), "/WRITE.TXT", FA_READ|FA_WRITE);
	printString(tp4);
	printChar_BCD(ret);
					   
	ret = f_read((&fil_obj),data_buffer,12, &byte_read);
	data_buffer[13] = '\0'; 
	printString(tp5);
	printString(data_buffer);

 #if 1
 
 #if 0
      ret = f_lseek((&fil_obj), 0);
	  for(i=0;tp6[i];i++)  //transmit a predefined string
       	uart_TxChar(tp6[i]);
		hn  = ret >> 4;
  		ret = ret << 4;
  		ret = ret >> 4;
  		ln  = ret;
		send[0]	= ascii_string[hn]; 
		send[1]	= ascii_string[ln];  
	 	uart_TxChar(send[0]);
		uart_TxChar(send[1]); 
 #endif
    ret = f_write((&fil_obj),write_data,11, &byte_read);
	printString(tp7);
	printChar_BCD(ret);
#endif
   f_sync(&fil_obj);	
   f_close(&fil_obj);
}


