#include "portb.h"

#include <avr/interrupt.h>
#include <avr/io.h>

PortB::PortB()
{
	init();
}


void PortB::init()
{
	mFuncPtr = NULL;
	//mChanged = false;
	mPortHistory = 0xFF;

	    DDRB &= ~((1 << DDB0) | (1 << DDB1) | (1 << DDB2)); // Clear the PB0, PB1, PB2 pin
    // PB0,PB1,PB2 (PCINT0, PCINT1, PCINT2 pin) are now inputs

    PORTB |= ((1 << PORTB0) | (1 << PORTB1) | (1 << PORTB2)); // turn On the Pull-up
    // PB0, PB1 and PB2 are now inputs with pull-up enabled
    
    PCICR |= (1 << PCIE0);     // set PCIE0 to enable PCMSK0 scan
    PCMSK0 |= (1 << PCINT0);   // set PCINT0 to trigger an interrupt on state change 
}


void PortB::updatePort()
{
	
}

void PortB::portChanged()
{
	uint8_t changedBits = PINB ^ mPortHistory;
	mPortHistory = PINB;
	
	if(changedBits & (1<<PINB0))
	{

	}
	if(changedBits & (1<<PINB1))
	{

	}
	if(changedBits & (1<<PINB2))
	{

	}
	if(changedBits & (1<<PINB3))
	{

	}
	if(changedBits & (1<<PINB4))
	{

	}
	if(changedBits & (1<<PINB5))
	{

	}
	if(changedBits & (1<<PINB6))
	{

	}
	if(changedBits & (1<<PINB7))
	{

	}
}

void PortB::setPinTriggeredFunc(funcPtr aFuncPtr)
{
	//mFuncPtr = aFuncPtr;
}
