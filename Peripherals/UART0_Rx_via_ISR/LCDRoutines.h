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
