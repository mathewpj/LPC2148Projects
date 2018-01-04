/***************************************************************** 
*  Program for a LPC 2148 based Real Time Counter.
*  Counts from 00:00:00  						  
* 
*  PREINT (Prescaler Value, integrer value) = int(PCLK/32768) -1
*  i.e., int(15MHz/32768) - 1 =  456 = 0x16D
*  PREFRAC = PCLK - ((PREINT + 1) * 32768)
*  i.e.,  12Mhz - ((456 + 1) * 	32768) = 25024 =  0x61C0
******************************************************************/
					  

#include <stdio.h>
#include <stdlib.h>
#include "LPC214x.H" 									// LPC2148 MPU Register
#include "LCDRoutines.h"
#include "rtc.h"
#include "support.h"

extern void lcd_init(void);
extern void goto_cursor(unsigned char);
extern void lcd_print(unsigned char*);	
extern void delay(unsigned long int);


														
/* Main Program Start Here */
int main(void)
{  
   
  unsigned char time_string[8], hn, ln, temp;
  char hour, min, sec;
  char ascii_string[10];
  
/* Initilize an array with ASCII values of 0-9 numbers */
  ascii_string[0]= 48; // Ascii for 0 and so on ...
  ascii_string[1]= 49;
  ascii_string[2]= 50;
  ascii_string[3]= 51;
  ascii_string[4]= 52;
  ascii_string[5]= 53;
  ascii_string[6]= 54;
  ascii_string[7]= 55;  
  ascii_string[8]= 56;  
  ascii_string[9]= 57;  


/* Initilize the display string  */
  time_string[0] =  '0';	
  time_string[1] =  '0';	
  time_string[2] =  ':';	
  time_string[3] =  '0';	
  time_string[4] =  '0';	
  time_string[5] =  ':';	
  time_string[6] =  '0';	
  time_string[7] =  '0';	
  
  lcd_init();						// Initial LCD
  rtc_init();						// Initialize RTC

  while(1){

  	goto_cursor(0x00);				// Set Cursor Line-1
    
  	sec = SEC;	
	temp = CHAR2BCD2(sec);
  	hn = temp >> 4;
  	temp = temp << 4;
  	temp = temp >> 4;
  	ln = temp; 
  	time_string[6] =  ascii_string[hn];	
  	time_string[7] =  ascii_string[ln];	

  	min = MIN;
  	temp = CHAR2BCD2(min);
  	hn = temp >> 4;
  	temp = temp << 4;
  	temp = temp >> 4;
  	ln = temp; 
  	time_string[3] =  ascii_string[hn];	
  	time_string[4] =  ascii_string[ln];
  
  	hour = HOUR;
  	temp = CHAR2BCD2(hour);
  	hn = temp >> 4;
  	temp = temp << 4;
  	temp = temp >> 4;
  	ln = temp; 
  	time_string[0] =  ascii_string[hn];	
  	time_string[1] =  ascii_string[ln];	

  	lcd_print(time_string);				// Display LCD Line-2
  	
  }																	 
 
}

