/*
 * Program that implements UART RX via a ISR
 * UART initiaized to 9600 Baud
 * Steps:
 *  1) Programs the PLLs
 *	2) Configure P0.0/P0.1 as RX0 and TX0
 *  3) Enable FIFO and reset Rx/Tx FIFO buffers
 *		- Somehow enabling Rx Trigger at 4,8,14 charcters does not seem to work.
 		  Interrupts still arrive at at 1 charater interrupts 	
 *  4) Enable Receive Data Available Interrupt : U0IER Bit 0
 *	5) 8bit data, 1Stop bit, No parity
 *  6) Program the BaudRate
 *  7) Clear DLAB after setting DLL,DLM
 *	8) Set Up the Interrupt Handler
 *		- VICIntSelect set to 0 to indicate Interrupts configured as IRQ(as opposed to FIQ)
 *		- Enable UART0 interrupt
 *		- Point to the ISR
 *		- Enable UART0 IRQ slot 
 *		- Enable RBR interrupt
 *
 * For comments	write to me : mathew.p.joseph@gmail.com
*/

#include <lpc214x.h>
#include "stdutils.h"	
#include "systemInit.h"
#include "LCDRoutines.h"

#define LCD_DSIPLAY_LEN 16

#define SBIT_WordLenght    0x00u
#define SBIT_DLAB          0x07u
#define SBIT_FIFO          0x00u
#define SBIT_RxFIFO        0x01u
#define SBIT_TxFIFO        0x02u

#define SBIT_RDR           0x00u
#define SBIT_THRE          0x05u

#define TX0_PINSEL            0
#define RX0_PINSEL            2

void lcd_init(void);					
void lcd_write_control(unsigned char);
void lcd_write_ascii(unsigned char); 	
void goto_cursor(unsigned char);		
void lcd_print(unsigned char*);			
void enable_lcd(void);	 				
void delay(unsigned long int);			
void led_pattern1(void);

/* Function to initialize the UART0 at specifief baud rate */
void uart_init(uint32_t baudrate)
{
    uint32_t var_RegValue_u32;

    PINSEL0 |= (1<<RX0_PINSEL) | (1<<TX0_PINSEL);   				// Configure P0.0/P0.1 as RX0 and TX0 

    U0FCR = (1<<SBIT_FIFO) | (1<<SBIT_RxFIFO) | (1<<SBIT_TxFIFO); 	// Enable FIFO and reset Rx/Tx FIFO buffers    
    U0LCR = (0x03<<SBIT_WordLenght) | (1<<SBIT_DLAB);             	// 8bit data, 1Stop bit, No parity

    var_RegValue_u32 = ( pclk / (16 * baudrate )); 
    U0DLL =  var_RegValue_u32 & 0xFF;
    U0DLM = (var_RegValue_u32 >> 0x08) & 0xFF;

    util_BitClear(U0LCR,(SBIT_DLAB));  			// Clear DLAB after setting DLL,DLM
}

/* Function to transmit a char */
void uart_TxChar(char ch)
{
    while(util_IsBitCleared(U0LSR,SBIT_THRE)); // Wait for Previous transmission
    U0THR=ch;                                  // Load the data to be transmitted
}

void send_line_lcd(unsigned char* buf)
{
    lcd_write_control(0x01);                   // Clear Display  (Clear Display,Set DD RAM Address=0) 
    goto_cursor(0x00);		 
	lcd_print(buf); 
}

__irq void UART0_Interrupt(void)
{

  unsigned char rx;
    
  /* U0IIR : UART0 Interrupt Identification Register */
  while( (U0IIR & 0x1) == 0){ /* at least one interrupt is pending */
	  if(U0IIR & 0x4) /* Receive Data Avaliable */
		 rx = U0RBR;
  }
  goto_cursor(0x00);	
  send_line_lcd(&rx);
    
  VICVectAddr = 0x00;	
}

int main()
{
  	unsigned char a[]="\n\rTest Link Established";
    int i;

	lcd_init();	
	lcd_write_control(0x01);  					// Clear Display  (Clear Display,Set DD RAM Address=0) 
    goto_cursor(0x00);							// Set Cursor Line-1 
    lcd_print("INITIALIZING LCD");				// Display LCD Line-1  
	delay(1000);							    // Display Delay 
	lcd_write_control(0x01);  					// Clear Display  (Clear Display,Set DD RAM Address=0
	lcd_cursor_off();

    SystemInit();
    uart_init(9600);  							// Initialize the UART0 for 9600 baud rate 
	VICIntSelect = 0x00000000;					// UART0 configured as IRQ 
	VICIntEnable = 0x00000040;					// Enable UART0 interrupt 
	VICVectAddr0 = (unsigned) UART0_Interrupt;	// UART0 ISR Address 
	VICVectCntl0 = 0x00000026;					// Enable UART0 IRQ slot 
	U0IER = 0x00000001;							// Enable RBR interrupt 
	  

    for(i=0;a[i];i++)  							// Transmit a predefined string
        uart_TxChar(a[i]);
	
	while(1)
	{}
 	       
}
