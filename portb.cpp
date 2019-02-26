/*This file is part of June.

June is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <https://www.gnu.org/licenses/>.*/

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
