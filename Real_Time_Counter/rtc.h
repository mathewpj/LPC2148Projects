
#include "LPC214x.H"


void rtc_init(void)
{
  CCR 		= 0x11; // Set CLKEN & CTCRST bits   
  PREINT	= 0x01C8;
  PREFRAC	= 0x61C0; 
  SEC		= 0;   //  Start counting form 00:00:00
  MIN		= 0;
  HOUR		= 0;
}
