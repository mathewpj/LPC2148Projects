
#include <stdio.h>
#include <stdlib.h>
#include "LPC214x.H" 		// LPC2148 MPU Register
#include "LCDRoutines.h"
#include "timer.h"
#include "support.h"

#define MR0I (1<<0)    		//Interrupt When TC matches MR0
#define MR0R (1<<1)	   		//Reset TC when TC matches MR0
#define PRESCALE 60000 		//60000 PCLK clock cycles to increment TC by 1
#define DELAY_MS 1000  		//1000ms Second(s) Delay

/* Configure PLL0 with CCLK = 60MHz and PCLK (Peripheral Clock) also at 60Mhz */
#define PLOCK 0x00000400

extern void lcd_init(void);
extern void lcd_print(unsigned char*);
extern void goto_cursor(unsigned char i);	


__irq void T0ISR(void)
{
	unsigned char hn, ln, temp;
  	static char hour=0, min=0, sec=0;
  	long int regVal;
	// These variables need to be defined in the ISR (and not as global variables)
	// to be accessed within the ISR
	/* Initilize an array with ASCII values of 0-9 numbers */
	static char ascii_string[10] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57};
	/* Initilize the display string  */
    static unsigned char time_string[8] ={'0', '0', ':', '0', '0', ':', '0', '0'};

  	lcd_cursor_off();   	// Turn off cursor to prevent the annoying artifacts! 
	goto_cursor(0);	
	lcd_print(time_string);


	regVal = T0IR;     		//Read current IR value
	
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

	goto_cursor(0);	
	lcd_print(time_string);

	T0IR = regVal;     		//Write back to IR to clear Interrupt Flag
	VICVectAddr = 0x0; 		//This is to signal end of interrupt execution
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

void Timer0Init()
{
	T0CTCR = 0x0;	     	// Select Timer Mode
	T0PR = PRESCALE-1;   	// (Value in Decimal!) - Increment T0TC at every 60000 clock cycles
                         	// Count begins from zero hence subtracting 1
                         	// 60000 clock cycles @60Mhz = 1 mS
	T0MR0 = DELAY_MS-1;  	//(Value in Decimal!) Zero Indexed Count - hence subtracting 1
						 	// When Match Contro Register  = TC (in this case after 1000ms
						 	// Timer ISR gets called		
	T0MCR = MR0I | MR0R; 	//Set bit0 & bit1 to High which is to : Interrupt & Reset TC on MR0  

 	/*----------Setup Timer0 Interrupt-------------*/

	VICVectAddr4 = (unsigned )T0ISR; //Pointer Interrupt Function (ISR)

	VICVectCntl4 = 0x20 | 4; //0x20 (i.e bit5 = 1) -> to enable Vectored IRQ slot (Refer Section 7.4.9)
				             //0x4 (bit[4:0]) -> this the source number - 
							 //here its timer0 which has VIC channel mask # as 4
				             //You can get the VIC Channel number from Lpc214x 
							 //manual R2 - pg 58 / sec 5.5

	
	VICIntEnable = 0x10;     //Enable timer0 int
	
	T0TCR = 0x02;            //Reset Timer
}

int main(void)
{

	lcd_init();				// Initialize the LCD
	ClockInit();
	Timer0Init();
	T0TCR = 0x01; 			// Enable the timer
	while(1); 				//Infinite Idle Loop
	
	return 1;
}


