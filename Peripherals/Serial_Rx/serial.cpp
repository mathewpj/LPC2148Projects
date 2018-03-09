/**********************************************************
*
* Program to read serial data on the ttyUSB0 port
*
* gcc serial.cpp -o serial
* 
* Author : Mathew P Joseph <mathew.p.joseph@gmail.com>
*	
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>  /* File Control Definitions          */
#include <termios.h>/* POSIX Terminal Control Definitions*/
#include <unistd.h> /* UNIX Standard Definitions         */
#include <errno.h>  /* ERROR Number Definitions          */

//http://www.unixwiz.net/techtips/termios-vmin-vtime.html


int main()
{
  int fd, i;
  struct termios SerialPortSettings;
  char read_buffer[41];                
  int  bytes_read = 0;         

  fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY);
  if(fd == 1)
     printf("\n  Error! in Opening ttyUSB0\n");
  else
     printf("\n  ttyUSB0 Opened Successfully\n");
 
  tcgetattr(fd, &SerialPortSettings);
  cfsetispeed(&SerialPortSettings,B9600); 
  SerialPortSettings.c_cflag &= ~CSIZE;  // Clear CSIZE 
  SerialPortSettings.c_cflag |= CS8;     // Charatcter Size 8 Bits 
  SerialPortSettings.c_cflag &= ~PARENB; // No Parity
  SerialPortSettings.c_cflag &= ~CSTOPB; // No Stop Bit

  SerialPortSettings.c_cflag &= ~CRTSCTS;

  SerialPortSettings.c_cflag |= CREAD;   // Enable Receiver
  SerialPortSettings.c_cflag |= CLOCAL;  // Ignore modem status lines

  // Turn off Software Based Flow Control
  SerialPortSettings.c_iflag &= ~IXON; 
  SerialPortSettings.c_iflag &= ~IXOFF; 
  SerialPortSettings.c_iflag &= ~IXANY; 

 
  // Turn on Non Canonical Mode
  SerialPortSettings.c_lflag &= ICANON;
  SerialPortSettings.c_lflag &= ECHO;
  SerialPortSettings.c_lflag &= ECHOE;
  SerialPortSettings.c_lflag &= ISIG;
  
  //SerialPortSettings.c_cc[VMIN]  = 100; /* Read 10 characters */  
  //SerialPortSettings.c_cc[VTIME] = 0;   /* Wait indefinitely  */

  SerialPortSettings.c_cc[VMIN]  = 41; /* Read 10 characters */  
  SerialPortSettings.c_cc[VTIME] = 0;   /* Wait indefinitely  */
  tcsetattr(fd,TCSANOW,&SerialPortSettings);

  fcntl(fd, F_SETFL, 0);
  while(1){
     i=read(fd, &read_buffer, 41);
     printf("number read = %d, string = %s \n", i, read_buffer);
     for(int k =0; k < 100000; k++){}
     //memset(read_buffer, 0, 100);
   }
#if 0
  while(1){
     i %= 101;
     read(fd, &read_buffer[i], 100);
     if( i == 100){
    	printf("string = %s \n", read_buffer);
	memset(read_buffer, 0, 100);
     }
     i++;
  }
#endif
  close(fd);
  return(0);
}
