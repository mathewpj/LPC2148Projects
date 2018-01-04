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
