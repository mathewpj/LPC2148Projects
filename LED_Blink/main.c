/***************************************************
*
* Basic program to blink a series of leds connected
* to P0.0 to P0.7 pins
*
***************************************************/

#include "LPC214x.H" 									// LPC2148 MPU Register
#include <stdio.h>

/* pototype  section */

extern void delay(unsigned long int);							// Delay Function
extern void led_pattern(void);
														
/* Main Program Start Here */
int main(void)
{  
   #pragma ARM 

   // Ideally need to configure PINSEL0 register.
   // Since system powers up with default config as P0.0
   // no need to explicitly configure PINSEL0. Anyway doing it...

   PINSEL0 = 0x00000000;
   led_pattern();

}

