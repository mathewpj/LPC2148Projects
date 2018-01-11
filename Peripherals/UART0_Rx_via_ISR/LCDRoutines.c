#include "LCDRoutines.h"

#define  lcd_rs_set() IOSET0 = LCD_RS	 				// RS = 1 (Select Instruction)
#define  lcd_rs_clr() IOCLR0 = LCD_RS					// RS = 0 (Select Data)
#define  lcd_en_set() IOSET0 = LCD_EN					// EN = 1 (Enable)
#define  lcd_en_clr() IOCLR0 = LCD_EN					// EN = 0 (Disable)

#define  lcd_dir_write()  IODIR0 |= 0x01FFFFFF			// LCD Data Bus = Write
								// Pins 0:24 configured as O/P pins
								// uptill pin 24 been configured as o/p pin 
								// since P0.23 	is connected to D7 of LCD 16X2 component


/* pototype  section */
void lcd_init(void);					// Initial LCD
void lcd_out_data4(unsigned char);		// Strobe 4-Bit Data to LCD
void lcd_write_byte(unsigned char);		// Write 1 Byte Data to LCD
void lcd_write_control(unsigned char);	// Write Instruction
void lcd_write_ascii(unsigned char); 	// Write LCD Display(ASCII)
void goto_cursor(unsigned char);		// Set Position Cursor LCD
void lcd_print(unsigned char*);			// Print Display to LCD
//char busy_lcd(void);					// Read Busy LCD Status
void enable_lcd(void);	 				// Enable Pulse
void delay(unsigned long int);			// Delay Function


/****************************/
/* Strobe 4-Bit Data to LCD */
/****************************/
void lcd_out_data4(unsigned char val)
{
IOCLR0 = (LCD_DATA);	  				// Reset 4-Bit Pin Data
  IOSET0 = (val<<16); 					// EN,0,RW,RS:DDDD:0000:0000:0000:0000:0000:0000   
 
}


// to LCD
/****************************/
/* Write Data 1 Byte to LCD */
/****************************/
void lcd_write_byte(unsigned char val)
{  
      
  lcd_out_data4(val);				// Strobe 4-Bit High-Nibble to LCD 
  enable_lcd();						// Enable Pulse
  
  delay(800000);
}

/****************************/
/* Write Instruction to LCD */
/****************************/
void lcd_write_control(unsigned char val)
{ 

  lcd_rs_clr();						// RS = 0 = Instruction Select
  lcd_write_byte(val);				// Strobe Command Byte	    
}

/****************************/
/* Write Data(ASCII) to LCD */
/****************************/
void lcd_write_ascii(unsigned char c)
{  
  lcd_rs_set();						// RS = 1 = Data Select
  lcd_write_byte(c);					// Strobe 1 Byte to LCD    
}

/*******************************/
/* Initial 4-Bit LCD Interface */
/*******************************/
void lcd_init()
{
  unsigned int i;				       // LCD Initial Delay Count 

  PINSEL1  = 0x00000000;               // Configuring	P0.16 to P0.31 as GPIO pins
  				                       // GPIO1[31..16] = I/O Function
  lcd_dir_write();		               // lcd_direction_write		// LCD Data Bus = Write
  for (i=0;i<1000;i++);			       // Power-On Delay (15 mS)

  IOCLR0 = (LCD_IOALL);			       // Reset (RS,RW,EN,4-Bit Data) Pin
  IOSET0 = (LCD_D5|LCD_D4);		       // DDDD:EN,RW,RS,0:0000:0000:0000:0000:0000:0000 
  enable_lcd();			   		       // Enable Pulse	 
  for (i=0;i<100;i++);			       // Delay 4.1mS

  IOCLR0 = (LCD_IOALL);	  		       // Reset (RS,RW,EN,4-Bit Data) Pin
  IOSET0 = (LCD_D5|LCD_D4);		       // DDDD:EN,RW,RS,0:0000:0000:0000:0000:0000:0000 
  enable_lcd();					       // Enable Pulse
  for (i=0;i<100;i++);			       // delay 100uS
  IOCLR0 = (LCD_IOALL);	  		       // Reset (RS,RW,EN,4-Bit Data) Pin
  IOSET0 = (LCD_D5|LCD_D4);		       // DDDD:EN,RW,RS,0:0000:0000:0000:0000:0000:0000 
  enable_lcd();					       // Enable Pulse
  delay(10000);                        //while(busy_lcd());      								                                              // Wait LCD Execute Complete

  IOCLR0 = (LCD_IOALL);	               // Reset (RS,RW,EN,4-Bit Data) Pin
  IOSET0 = (LCD_D5);	     	       // DDDD:EN,RW,RS,0:0000:0000:0000:0000:0000:0000 
  enable_lcd();					       // Enable Pulse
  delay(10000);
      
  lcd_write_control(0x38);             // Function Set (DL=0 4-Bit,N=1 2 Line,F=0 5X7)
  lcd_write_control(0x0E);  		   // Display on/off Control (Entry Display,Cursor off,Cursor not Blink)
  lcd_write_control(0x06);  		   // Entry Mode Set (I/D=1 Increment,S=0 Cursor Shift)
  lcd_write_control(0x01);  		   // Clear Display  (Clear Display,Set DD RAM Address=0) 
  for (i=0;i<100000;i++);			   // Wait Command Ready

}

/***************************/
/* Set LCD Position Cursor */
/***************************/
void goto_cursor(unsigned char i)
{
  i |= 0x80;						   // Set DD-RAM Address Command
  lcd_write_control(i);  
}

/************************************/
/* Print Display Data(ASCII) to LCD */
/************************************/
void lcd_print(unsigned char* str)
{
  int i;

  
  for (i=0;i<16 && str[i]!=0;i++)  		// 16 Character Print
  {									   
     lcd_write_ascii(str[i]);    		// Print Byte to LCD
  }
 
}

/***********************/
/* Enable Pulse to LCD */
/***********************/
void enable_lcd(void)	 				// Enable Pulse
{
  unsigned int i;						// Delay Count

  
  lcd_en_set();  						// Enable ON
  for (i=0;i<400000;i++);		  
  lcd_en_clr();							// Enable OFF 
}

/***********************/
/* Delay Time Function */
/*    1-4294967296     */
/***********************/
void delay(unsigned long int count1)
{
  while(count1 > 0) {count1--;}			// Loop Decrease Counter	
}
