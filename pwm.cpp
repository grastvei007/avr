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

#include "pwm.h"

#ifndef AVR_TEST
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#else

#endif
#include <stdlib.h>

Pwm::Pwm()
{
	init();
}

Pwm::~Pwm()
{

}

void Pwm::init()
{
	// Set mode of operation to FastPWM
	TCCR0A |= (1<<WGM00 | 1<<WGM01);
	TCCR2A |= (1<<WGM20 | 1<<WGM21);
	
	// Set clock source (prescaler)
	TCCR0B |= (1<<CS01);
	TCCR2B |= (1<<CS21);
	
	// Set to 50% duty cycle
	OCR0A = 0x80;
	OCR0B = 0x80;
	OCR2A = 0x80;
	OCR2B = 0x80;
	
	// 4 PWM channel outputs
	DDRB |= 1<<PB3; // OC2A
	DDRD |= 1<<PD3; // OC2B
	DDRD |= 1<<PD5; // OC0B
	DDRD |= 1<<PD6; // OC0A	 
}


void Pwm::enable(Pwm::Channel aChannel)
{
	if(aChannel == eChan1)
		TCCR0A |=(1<<COM0A1);
	if(aChannel == eChan2)
		TCCR0A |= (1<<COM0B1);
	if(aChannel == eChan3)
		TCCR2A |= (1<<COM2A1);
	if(aChannel == eChan4)
		TCCR2A |= (1<<COM2B1);
}


void Pwm::disable(Pwm::Channel aChannel)
{
    if(aChannel == eChan1)
        TCCR0A &= ~(1<<COM0A1);
    if(aChannel == eChan2)
        TCCR0A &= ~(1<<COM0B1);
    if(aChannel == eChan3)
        TCCR2A &= ~(1<<COM2A1);
    if(aChannel == eChan4)
        TCCR2A &= ~(1<<COM2B1);
}


void Pwm::setDutyCycle(Pwm::Channel aChannel, uint8_t aDutyCycle)
{
	if(aChannel == eChan1)
		OCR0A = aDutyCycle;
	else if(aChannel == eChan2)
		OCR0B = aDutyCycle;
	else if(aChannel == eChan3)
		OCR2A = aDutyCycle;
	else if(aChannel == eChan4)
		OCR2B = aDutyCycle;
}

