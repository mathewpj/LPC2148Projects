
#include "i2c.h"
#include "stdutils.h"

void I2C_INIT(void)
{
	PINSEL0 = PINSEL0 | 0x00000050; /* P0.2 and P0.3 as SCL0 and SDA0 */
	I2C0CONSET = 0x40; /* I2C Enable */
	I2C0SCLL = 0x300; /* I2C bit frequency 100Khz, given PCLK @ 60Mhz */
	I2C0SCLH = 0x300;
}

void I2C_START(void)
{
	I2C0CONSET = 0x20; /* STA = 1 */
	while ( (I2C0CONSET & 0x08) == 0 ); /* Wait till SI = 1 */
	I2C0CONCLR = 0x28; /* Clear STA and SI */
}

void I2C_WRITE( char data )
{
	I2C0DAT = data;
	I2C0CONSET = 0x40; /* I2C Enable */
	while( (I2C0CONSET & 0x08) == 0 ); /* Wait till SI = 1 */
	I2C0CONCLR = 0x08; /* Clear SI */
}

unsigned char I2C_READ( void )
{
	I2C0CONSET = 0x44; /* I2C Enable with Acknowledge */
	while( (I2C0CONSET & 0x08) == 0 ); /* Wait till SI = 1 */
	I2C0CONCLR = 0x0C; /* Clear SI and Acknowledge */
	return I2C0DAT;
}

unsigned char I2C_READ1( void )
{
	I2C0CONSET = 0x40; /* I2C Enable */
	while( (I2C0CONSET & 0x08) == 0 ); /* Wait till SI = 1 */
	I2C0CONCLR = 0x08; /* Clear SI  */
	return I2C0DAT;
}

void I2C_MULTIREAD( char* arr , int bytes )
{
	uint8_t i = 0;
	while( ( bytes - 1 ) != 0 )
	{
		I2C0CONSET = 0x44; /* I2C Enable with Acknowledge */
		while( (I2C0CONSET & 0x08) == 0 ); /* Wait till SI = 1 */
		I2C0CONCLR = 0x0C; /* Clear SI and Acknowledge */
		*( arr + i ) = I2C0DAT ;
		bytes--;
		i++;
	}
	I2C0CONSET = 0x40; /* I2C Enable */
	while( (I2C0CONSET & 0x08) == 0 ); /* Wait till SI = 1 */
	I2C0CONCLR = 0x08; /* Clear SI */
	*( arr + i ) = I2C0DAT ;
}

void I2C_STOP( void )
{
	I2C0CONSET = 0x50; /* STO = 1 */
}
