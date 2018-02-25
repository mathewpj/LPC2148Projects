//https://www.youtube.com/watch?v=0rgEd3vAclc
//http://bikealive.nl/sd-v2-initialization.html
//https://luckyresistor.me/cat-protector/software/sdcard-2/
//http://www.dejazzer.com/ee379/lecture_notes/lec12_sd_card.pdf
//http://www.transcend-info.com/products/images/modelpic/539/SDHC10I_datasheet.pdf

#include <stdio.h>
#include <stdlib.h>
//#include "debug.h"
#include "LPC214x.H"	// LPC2148 MPU Register
#include "type.h"
#include "uart0.h"
#include "spi.h"
#include "sd.h"
#include "diskio.h"
#include "ff.h"

uint8_t CardType;

extern char ascii_string[16];
extern unsigned char hn, ln, temp;
extern uint8_t	send[2];

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

#if 0
void disk_initialize (void)
{
		int i;
		unsigned char tp1[]="\n\r After call SD_Init()";		
		unsigned char tp2[]="\n\r After call SD_ReadConfiguration()";

		SystemInit();
    	uart_init(9600);  // Initialize the UART0 for 9600 baud rate

		SD_Init(); 				
		for(i=0;tp1[i];i++)  //transmit a predefined string
        	uart_TxChar(tp1[i]);

		SD_ReadConfiguration();
		for(i=0;tp2[i];i++)  //transmit a predefined string
        	uart_TxChar(tp2[i]);
        return;
}
#endif



main()
{
	int i;
	unsigned char tp1[]="\n\r Before call to disk_initialize()";		
	unsigned char tp2[]="\n\r Return from disk_initialize(): ";
	unsigned char tp3[]="\n\r Return from f_mount(): ";	
	unsigned char tp4[]="\n\r Return from f_open(): ";	
	unsigned char tp5[]="\n\r Return from f_read(): ";		
	
	uint32_t byte_read=0;
	static uint8_t data_buffer[12];
	BYTE ret;
	FRESULT f_err_code;
	static FATFS FATFS_Obj;
	FIL fil_obj;

	for(i=0;tp1[i];i++)  //transmit a predefined string
        	uart_TxChar(tp1[i]);

 	ret = disk_initialize (0);
	{
	hn  = ret >> 4;
  	ret = ret << 4;
  	ret = ret >> 4;
  	ln  = ret;
	send[0]	= ascii_string[hn]; 
	send[1]	= ascii_string[ln]; 

	for(i=0;tp2[i];i++)  //transmit a predefined string
        	uart_TxChar(tp2[i]);
	   	
	uart_TxChar(send[0]);
	uart_TxChar(send[1]);
	}

	f_err_code = f_mount(0, &FATFS_Obj);
	{
	for(i=0;tp3[i];i++)  //transmit a predefined string
        	uart_TxChar(tp3[i]);
	
	hn  = f_err_code >> 4;
  	f_err_code = f_err_code << 4;
  	f_err_code = f_err_code >> 4;
  	ln  = f_err_code;
	send[0]	= ascii_string[hn]; 
	send[1]	= ascii_string[ln];
	uart_TxChar(send[0]);
	uart_TxChar(send[1]); 
	}

	ret = f_open((&fil_obj), "/1.txt", FA_READ);
	{
		for(i=0;tp4[i];i++)  //transmit a predefined string
        	uart_TxChar(tp4[i]);
		hn  = ret >> 4;
  		ret = ret << 4;
  		ret = ret >> 4;
  		ln  = ret;
		send[0]	= ascii_string[hn]; 
		send[1]	= ascii_string[ln];  
	 	uart_TxChar(send[0]);
		uart_TxChar(send[1]); 
	}

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

}
