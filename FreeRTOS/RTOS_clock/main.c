/* 
 *  Program to implement a Real Time Clock using RTOS
 *  delay functions. 
 *  This obviates the need for setting up a ISR for Timers.
 *  FreeRTOS automatically takes care of this when you call
 *  vTaskDelay(). 
 *  mathew.p.joseph@gmail.com
*/

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "support.h"
#include "LCDRoutines.h"

/* Configure PLL0 with CCLK = 60MHz and PCLK (Peripheral Clock) also at 60Mhz */
#define PLOCK 0x00000400
#define DELAY_MS 1000

extern void lcd_init(void);
extern void lcd_print(unsigned char*);
extern void goto_cursor(unsigned char i);	

void clock_func(void* p_void)
{
	unsigned char hn, ln, temp;
  	static char hour=0, min=0, sec=0;
  	
	// These variables need to be defined in the ISR (and not as global variables)
	// to be accessed within the ISR
	/* Initilize an array with ASCII values of 0-9 numbers */
	static char ascii_string[10] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57};
	/* Initilize the display string  */
    static unsigned char time_string[8] ={'0', '0', ':', '0', '0', ':', '0', '0'};

  	lcd_cursor_off();   	// Turn off cursor to prevent the annoying artifacts! 
	goto_cursor(0);	
	lcd_print(time_string);
			
	while(1){
		vTaskDelay(pdMS_TO_TICKS(DELAY_MS));
		sec = sec + 1;
		if(sec%60 == 0){
			sec = 0;
			min++;
		}
		
		temp = CHAR2BCD2(sec);
  		hn = temp >> 4;
  		temp = temp << 4;
  		temp = temp >> 4;
  		ln = temp; 
  		time_string[6] =  ascii_string[hn];	
  		time_string[7] =  ascii_string[ln];
	
   		temp = CHAR2BCD2(min);
  		hn = temp >> 4;
  		temp = temp << 4;
  		temp = temp >> 4;
  		ln = temp; 
  		time_string[3] =  ascii_string[hn];	
  		time_string[4] =  ascii_string[ln];

		if((min%60 == 0)&&(sec%60 == 0)){
			min = 0;
			hour++;
		}

  		temp = CHAR2BCD2(hour);
  		hn = temp >> 4;
  		temp = temp << 4;
  		temp = temp >> 4;
  		ln = temp; 
  		time_string[0] =  ascii_string[hn];	
  		time_string[1] =  ascii_string[ln];	
		if((hour%24 == 0)&&(min%60 == 0)&&(sec%60 == 0))
			hour = 0;
		
		goto_cursor(0);	
		lcd_print(time_string);
    }
		
}

void ClockInit(void)
{

   PLL0CON = 0x01; 			// PPLE=1 & PPLC=0, only PLL enabled but not connected 
   PLL0CFG = 0x24; 			// set the multipler to 5 (i.e actually 4)  12x5 = 60 Mhz (M - 1 = 4)
                   			// Set P=2 since we want FCCO in range!!!
                   			// So , Assign PSEL =01 in PLL0CFG as per the table.
   
   PLL0FEED = 0xAA; 		// Unlock the PLL registers by sending the sequence(0xAA-0x55)
   PLL0FEED = 0x55;
   
   // check whether PLL has locked on to the  desired freq by reading the lock bit
   // in the PPL0STAT register
   while( !( PLL0STAT & PLOCK ))
   {}
  
   // now enable(again) and connect
   PLL0CON = 0x03;
 
   PLL0FEED = 0xAA; 		// lock the PLL registers after setting the required PLL
   PLL0FEED = 0x55;

   VPBDIV = 0x01;   		// PCLK is same as CCLK i.e 60Mhz  
}
		 
int main()
{
	lcd_init();				// Initialize the LCD
	ClockInit();

	xTaskCreate(clock_func, "CLOCK", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL); 
	
    vTaskStartScheduler();
	while(1); // We shoudnt be reaching here 
}
