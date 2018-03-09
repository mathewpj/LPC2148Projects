#ifndef _I2C_H_
#define _I2C_H_

#include <lpc214x.h>


void I2C_INIT(void);
void I2C_START(void);
void I2C_WRITE( char data );
unsigned char I2C_READ( void );
unsigned char I2C_READ1( void );
void I2C_MULTIREAD( char* arr , int bytes );
void I2C_STOP( void );

#endif // _I2C_H_
