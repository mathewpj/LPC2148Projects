
#include "debug.h"
#include "uart0.h"

/* Initilize an array with ASCII values of 0 to F */
// 0 = 48
// ...
// 9 = 57
// A = 65
// ...
// 70 = F  
char ascii_string[16] ={48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69, 70};

void printChar_BCD(char reg)
{

	unsigned char hn,ln;
	char send[2];

	hn  = reg >> 4;
  	reg = reg << 4;
  	reg = reg >> 4;
  	ln  = reg;
	send[0]	= ascii_string[hn]; 
	send[1]	= ascii_string[ln]; 
	   	
	uart_TxChar(send[0]);
	uart_TxChar(send[1]);

	return;
}

void printString(char *string)
{
	int i;	
 
 	for(i=0;string[i];i++)  	//transmit a predefined string
       	uart_TxChar(string[i]);

}
