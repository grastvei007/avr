#include "i2c.h"

#include <avr/io.h>


#define TW_READY (TWCR & 0x80) // ready when TWINT returns to logic 1.
#define TW_STATUS (TWSR & 0xF8) // returns value of status register
#define TW_SEND 0x84 // send data (TWINT,TWEN)
#define TW_STOP 0x94 // send stop condition (TWINT,TWSTO,TWEN)


void i2c_init()
{
	TWSR = 0;
	 TWBR = ((F_CPU/F_SCL)-16)/2;
}


int i2c_start()
{
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	while(!TW_READY);
	
	return (TW_STATUS == 0x08);
}


int i2c_sendAddress(int8_t aAddress)
{
	TWDR = aAddress;
	TWCR = TW_SEND;
	while(!TW_READY);
	
	return (TW_STATUS == 0x18);
}


int i2c_write(int8_t byte)
{
	TWDR = byte;
	TWCR = TW_SEND;
	while(!TW_READY);

	return (TW_STATUS != 0x28); 
}


int i2c_stop()
{
	TWCR = TW_STOP;
}
