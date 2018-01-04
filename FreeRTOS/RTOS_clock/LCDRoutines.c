#include "LCDRoutines.h"




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
      
  //lcd_out_data4((val>>4)&0x0F);			// Strobe 4-Bit High-Nibble to LCD
  lcd_out_data4(val);					// Strobe 4-Bit High-Nibble to LCD 
  enable_lcd();						// Enable Pulse
  
}

/****************************/
/* Write Instruction to LCD */
/****************************/
void lcd_write_control(unsigned char val)
{ 

  lcd_rs_clr();						// RS = 0 = Instruction Select
  lcd_write_byte(val);					// Strobe Command Byte	    
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

  PINSEL1  = 0x00000000;	                       // Configuring	P0.16 to P0.31 as GPIO pins
  				                       // GPIO1[31..16] = I/O Function
  lcd_dir_write();			               // lcd_direction_write		// LCD Data Bus = Write
  for (i=0;i<1000;i++);				       // Power-On Delay (15 mS)

  IOCLR0 = (LCD_IOALL);				       // Reset (RS,RW,EN,4-Bit Data) Pin
  IOSET0 = (LCD_D5|LCD_D4);			       // DDDD:EN,RW,RS,0:0000:0000:0000:0000:0000:0000 
  enable_lcd();					       // Enable Pulse	 
  for (i=0;i<100;i++);				       // Delay 4.1mS

  IOCLR0 = (LCD_IOALL);	  			       // Reset (RS,RW,EN,4-Bit Data) Pin
  IOSET0 = (LCD_D5|LCD_D4);			       // DDDD:EN,RW,RS,0:0000:0000:0000:0000:0000:0000 
  enable_lcd();					       // Enable Pulse
  for (i=0;i<100;i++);				       // delay 100uS
  IOCLR0 = (LCD_IOALL);	  			       // Reset (RS,RW,EN,4-Bit Data) Pin
  IOSET0 = (LCD_D5|LCD_D4);			       // DDDD:EN,RW,RS,0:0000:0000:0000:0000:0000:0000 
  enable_lcd();					       // Enable Pulse
  delay(10000);                                        //while(busy_lcd());      								                                              // Wait LCD Execute Complete

  IOCLR0 = (LCD_IOALL);	                               // Reset (RS,RW,EN,4-Bit Data) Pin
  IOSET0 = (LCD_D5);				       // DDDD:EN,RW,RS,0:0000:0000:0000:0000:0000:0000 
  enable_lcd();					       // Enable Pulse
  delay(10000);
      
  lcd_write_control(0x38);  							// Function Set (DL=0 4-Bit,N=1 2 Line,F=0 5X7)
  lcd_write_control(0x0E);  							// Display on/off Control (Entry Display,Cursor off,Cursor not Blink)
  lcd_write_control(0x06);  							// Entry Mode Set (I/D=1 Increment,S=0 Cursor Shift)
  lcd_write_control(0x01);  							// Clear Display  (Clear Display,Set DD RAM Address=0) 
  for (i=0;i<100000;i++);								// Wait Command Ready

}

/***************************/
/* Set LCD Position Cursor */
/***************************/
void goto_cursor(unsigned char i)
{
  i |= 0x80;											// Set DD-RAM Address Command
  lcd_write_control(i);  
}

/************************************/
/* Print Display Data(ASCII) to LCD */
/************************************/
void lcd_print(unsigned char* str)
{
  int i;

  
  for (i=0;i<16 && str[i]!=0;i++)  						// 16 Character Print
  {
     lcd_write_ascii(str[i]);							// Print Byte to LCD
  }
 
}

/******************/
/* Wait LCD Ready */
/******************/

/*char busy_lcd(void)
{
  unsigned long busy_status;							// Busy Status Read
  unsigned int i;										// Delay Count

  lcd_dir_read();										// LCD Data Bus = Read
  lcd_rs_clr();		 									// Instruction Select
  lcd_rw_set(); 										// Read Direction
  lcd_en_set();											// Start Read Busy

  for (i=0;i<1000;i++);	  								// Delay Before Read
  busy_status = (IOPIN1 & 0x80000000);					// Read LCD Data(DDDD:EN,RW,RS,0:0000:0000:0000:0000:0000:0000) 
  if(busy_status == 0x80000000)  						// Read & Check Busy Flag
  {
    lcd_en_clr();  										// Disable Read
	lcd_rw_clr();										// Default = Write Direction
	lcd_dir_write();									// LCD Data Bus = Write
    return 1;											// LCD Busy Status
  }
  else
  {
    lcd_en_clr();  										// Disable Read
	lcd_rw_clr();										// Default = Write Direction
	lcd_dir_write();									// LCD Data Bus = Write
    return 0;											// LCD Ready Status
  }
}
  */

/***********************/
/* Enable Pulse to LCD */
/***********************/
void enable_lcd(void)	 								// Enable Pulse
{
  unsigned int i;										// Delay Count

  
  lcd_en_set();  										// Enable ON
  for (i=0;i<4000;i++);
  lcd_en_clr();  										// Enable OFF 
}
