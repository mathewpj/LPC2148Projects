#ifndef LCDROUTINES_H
#define LCDROUTINES_H

#include "LPC214x.H"				// LPC2148 MPU Register
#include <stdio.h>
#include <stdlib.h>

// Define LCD PinIO Mask 
#define  LCD_RS     0x00000400   						// P0.10(0000 0000 0000 000x 0000 0100 0000 0000)
#define  LCD_EN     0x00002000   						// P0.13(0000 0000 0000 00x0 0010 0000 0000 0000) 0x00002000
#define  LCD_D0     0x00010000   						// P0.16
#define  LCD_D1     0x00020000   						// P1.17
#define  LCD_D2     0x00040000   						// P0.18
#define  LCD_D3     0x00080000   						// P0.19
#define  LCD_D4     0x00100000   						// P0.20
#define  LCD_D5     0x00200000  						// P0.21
#define  LCD_D6     0x00400000  						// P0.22
#define  LCD_D7     0x00800000   						// P0.23
	
#define  LCD_DATA   (LCD_D7|LCD_D6|LCD_D5|LCD_D4|LCD_D3|LCD_D2|LCD_D1|LCD_D0)
#define  LCD_IOALL  (LCD_D7|LCD_D6|LCD_D5|LCD_D4||LCD_D3|LCD_D2|LCD_D1|LCD_D0|LCD_RS|LCD_EN)

#define  lcd_rs_set() IOSET0 = LCD_RS	 				// RS = 1 (Select Instruction)
#define  lcd_rs_clr() IOCLR0 = LCD_RS					// RS = 0 (Select Data)
#define  lcd_en_set() IOSET0 = LCD_EN					// EN = 1 (Enable)
#define  lcd_en_clr() IOCLR0 = LCD_EN					// EN = 0 (Disable)

#define  lcd_dir_write()  IODIR0 |= 0x01FFFFFF			// LCD Data Bus = Write
								// Pins 0:24 configured as O/P pins
								// uptill pin 24 been configured as o/p pin 
								// since P0.23 	is connected to D7 of LCD 16X2 component
//#define  lcd_dir_read()   IODIR1 &= 0xFFC3FFFF		// LCD Data Bus = Read 

/* pototype  section */
void lcd_init(void);					// Initial LCD
void lcd_out_data4(unsigned char);			// Strobe 4-Bit Data to LCD
void lcd_write_byte(unsigned char);			// Write 1 Byte Data to LCD
void lcd_write_control(unsigned char); 			// Write Instruction
void lcd_write_ascii(unsigned char); 			// Write LCD Display(ASCII)
void goto_cursor(unsigned char);			// Set Position Cursor LCD
void lcd_print(unsigned char*);				// Print Display to LCD
//char busy_lcd(void);					// Read Busy LCD Status
void enable_lcd(void);	 				// Enable Pulse
void delay(unsigned long int);				// Delay Function


#define  lcd_clear()          lcd_write_control(0x01)	// Clear Display
#define  lcd_cursor_home()    lcd_write_control(0x02)	// Set Cursor = 0
#define  lcd_display_on()     lcd_write_control(0x0E)	// LCD Display Enable
#define  lcd_display_off()    lcd_write_control(0x08)	// LCD Display Disable
#define  lcd_cursor_blink()   lcd_write_control(0x0F)	// Set Cursor = Blink
#define  lcd_cursor_on()      lcd_write_control(0x0E)	// Enable LCD Cursor
#define  lcd_cursor_off()     lcd_write_control(0x0C)	// Disable LCD Cursor
#define  lcd_cursor_left()    lcd_write_control(0x10)	// Shift Left Cursor
#define  lcd_cursor_right()   lcd_write_control(0x14)	// Shift Right Cursor
#define  lcd_display_sleft()  lcd_write_control(0x18)	// Shift Left Display
#define  lcd_display_sright() lcd_write_control(0x1C)	// Shift Right Display

#endif //LCDROUTINES_H
