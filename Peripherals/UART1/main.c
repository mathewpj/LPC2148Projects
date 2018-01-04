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

    util_BitClear(U0LCR,(SBIT_DLAB));  // Clear DLAB after setting DLL,DLM
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

void send_line_lcd(unsigned char* buf)
{
    lcd_write_control(0x01);                   // Clear Display  (Clear Display,Set DD RAM Address=0) 
    goto_cursor(0x00);		 
	lcd_print(buf); 
}

int main()
{
  	unsigned char ch[16],a[]="\n\rTest Link Established";
    int i;

	lcd_init();	
	lcd_write_control(0x01);  							// Clear Display  (Clear Display,Set DD RAM Address=0) 
    goto_cursor(0x00);									// Set Cursor Line-1
    lcd_print("INITIALIZING LCD");						// Display LCD Line-1 
	delay(100000);									    // Display Delay 
	lcd_write_control(0x01);  							// Clear Display  (Clear Display,Set DD RAM Address=0


    SystemInit();
    uart_init(9600);  // Initialize the UART0 for 9600 baud rate

   

    for(i=0;a[i];i++)  //transmit a predefined string
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
