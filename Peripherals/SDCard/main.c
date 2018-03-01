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
	static uint8_t data_buffer[12];
	BYTE ret;
	
	static FATFS FATFS_Obj;
	FIL fil_obj;
	uint8_t write_data[] = "HELLO MATHEW";
	SystemInit();
   	uart_init(9600);  // Initialize the UART0 for 9600 baud rate
	//	uart_init(19200);  // Initialize the UART0 for 9600 baud rate 
 	ret = f_mount(0, &FATFS_Obj);
	printString(tp3);
	printChar_BCD(ret);	
 
	ret = f_open((&fil_obj), "/WRITE.TXT", FA_READ|FA_WRITE);
	printString(tp4);
	printChar_BCD(ret);
					   
#if 0
	f_read((&fil_obj),data_buffer,11, &byte_read);
	{
		for(i=0;tp5[i];i++)  //transmit a predefined string
        	uart_TxChar(tp5[i]);
		uart_TxChar(data_buffer[0]);
		uart_TxChar(data_buffer[1]);
		uart_TxChar(data_buffer[2]);
		uart_TxChar(data_buffer[3]);
		uart_TxChar(data_buffer[4]);
		uart_TxChar(data_buffer[5]);
		uart_TxChar(data_buffer[6]);
		uart_TxChar(data_buffer[7]);
		uart_TxChar(data_buffer[8]);
		uart_TxChar(data_buffer[9]);
		uart_TxChar(data_buffer[10]);
		uart_TxChar(data_buffer[11]);
		#if 0
		ret = data_buffer[0];
		hn  = ret >> 4;
  		ret = ret << 4;
  		ret = ret >> 4;
  		ln  = ret;
		send[0]	= ascii_string[hn]; 
		send[1]	= ascii_string[ln];  
	 	uart_TxChar(send[0]);
		uart_TxChar(send[1]);
		
		ret = data_buffer[1];
		hn  = ret >> 4;
  		ret = ret << 4;
  		ret = ret >> 4;
  		ln  = ret;
		send[0]	= ascii_string[hn]; 
		send[1]	= ascii_string[ln];  
	 	uart_TxChar(send[0]);
		uart_TxChar(send[1]); 

		ret = data_buffer[2];
		hn  = ret >> 4;
  		ret = ret << 4;
  		ret = ret >> 4;
  		ln  = ret;
		send[0]	= ascii_string[hn]; 
		send[1]	= ascii_string[ln];  
	 	uart_TxChar(send[0]);
		uart_TxChar(send[1]); 

		ret = data_buffer[3];
		hn  = ret >> 4;
  		ret = ret << 4;
  		ret = ret >> 4;
  		ln  = ret;
		send[0]	= ascii_string[hn]; 
		send[1]	= ascii_string[ln];  
	 	uart_TxChar(send[0]);
		uart_TxChar(send[1]); 
		#endif 
	}
 #endif
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


