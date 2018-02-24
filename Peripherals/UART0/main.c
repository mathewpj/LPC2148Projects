/*********************************************************
*
* Program to establish serial communication via UART 0.
*  
* Setup : Connect a serial cable (or via serial to USB 
* coverter) to the computer's serial (or USB Port). Open
* a Tera term( or Hyperterm) configured for 9600 Baud. 
* 
* On sucessfull link inilialization, the program sends
* a string "Test Link Established" to the Terminal.
* Now the system is ready to receive any keyboard presses
* via UART0 and display it on the LCD display.
*
* Author : Mathew Joseph
* e-mail : mathew.p.joseph@gmail.com
*
*********************************************************/   

#include <lpc214x.h>
#include "stdutils.h"	
#include "systemInit.h"
#include "LCDRoutines.h"
#include "uart0.h"

#define LCD_DSIPLAY_LEN 16

void lcd_init(void);					
void lcd_write_control(unsigned char);
void lcd_write_ascii(unsigned char); 	
void goto_cursor(unsigned char);		
void lcd_print(unsigned char*);			
void enable_lcd(void);	 				
void delay(unsigned long int);			
void send_line_lcd(unsigned char* buf)
{
    lcd_write_control(0x01);         // Clear Display  (Clear Display,Set DD RAM Address=0) 
    goto_cursor(0x00);		 
	lcd_print(buf); 
}

int main()
{
  	unsigned char ch[16],a[]="\n\rTest Link Established";
    int i;

	lcd_init();	
	lcd_write_control(0x01);  		// Clear Display  (Clear Display,Set DD RAM Address=0) 
    goto_cursor(0x00);				// Set Cursor Line-1
    lcd_print("INITIALIZING LCD");	// Display LCD Line-1 
	delay(100000);					// Display Delay 
	lcd_write_control(0x01);  		// Clear Display  (Clear Display,Set DD RAM Address=0


    SystemInit();
    uart_init(9600);  				// Initialize the UART0 for 9600 baud rate

   

    for(i=0;a[i];i++)  				//Transmit a predefined string
        uart_TxChar(a[i]);

	i = 0;
    while(1)
    {
         ch[i] = uart_RxChar();
		 send_line_lcd(ch); 
		 if(i == (LCD_DSIPLAY_LEN-1))
		 	i = 0;
		 i++; 
    }       
}
