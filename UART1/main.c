/****************************************************
*
*  Code for a duplex link over UART to PC.
*  UART configured at 9600 baud rate. When LPC 2148 
*  board boots up ist sends a "Test Link Established"
*  to Hyperterm. 
*  User can then send any charater from keyboard to 
*  LCD display on LPC2148 Board.
*
*****************************************************/   

#include <lpc214x.h>
#include "stdutils.h"	
#include "systemInit.h"
#include "LCDRoutines.h"
#define LCD_DISPAY_LEN	   16 	
#define SBIT_WordLenght    0x00u
#define SBIT_DLAB          0x07u
#define SBIT_FIFO          0x00u
#define SBIT_RxFIFO        0x01u
#define SBIT_TxFIFO        0x02u

#define SBIT_RDR           0x00u
#define SBIT_THRE          0x05u

#define TX0_PINSEL            0
#define RX0_PINSEL            2

extern void SystemInit(void);

/* Function to initialize the UART0 at specifief baud rate */
void uart_init(uint32_t baudrate)
{
    uint32_t var_RegValue_u32;

    PINSEL0 |= (1<<RX0_PINSEL) | (1<<TX0_PINSEL);   //Configure P0.0/P0.1 as RX0 and TX0 

    U0FCR = (1<<SBIT_FIFO) | (1<<SBIT_RxFIFO) | (1<<SBIT_TxFIFO); // Enable FIFO and reset Rx/Tx FIFO buffers    
    U0LCR = (0x03<<SBIT_WordLenght) | (1<<SBIT_DLAB);             // 8bit data, 1Stop bit, No parity

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


/* Function to Receive a char */
char uart_RxChar()
{
  char ch; 
    while(util_IsBitCleared(U0LSR,SBIT_RDR));  // Wait till the data is received
    ch = U0RBR;                                // Read received data  
  return ch;
}

void send_line_lcd(char* buf)
{
    lcd_write_control(0x01);            // Clear Display  (Clear Display,Set DD RAM Address=0) 
    goto_cursor(0x00);		 
	lcd_print(buf); 
}

int main()
{
  	
    char ch[16],a[]="\n\rTest Link Established";
    int i = 0;

	lcd_init();	
	lcd_write_control(0x01);  	        // Clear Display  (Clear Display,Set DD RAM Address=0) 
    goto_cursor(0x00);					// Set Cursor Line-1
    lcd_print(" INITIALIZING LCD");		// Display LCD Line-1 
	delay(100000);						// Display Delay 
	lcd_write_control(0x01);  			// Clear Display  (Clear Display,Set DD RAM Address=0


    SystemInit();
    uart_init(9600);  					// Initialize the UART0 for 9600 baud rate

    for(i=0;a[i];i++)  					//Transmit a predefined string
        uart_TxChar(a[i]);

	 while(1)
    {
         // Receive a char and display it on LCD 
		 // Display will wrap around after 16 characters
		 ch[i] = uart_RxChar();
		 send_line_lcd(ch); 
		 if(i == (LCD_DISPAY_LEN-1))
		 	i = 0;
		 i++; 
     }       
}
