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

#include "adc.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>
#include <math.h>

namespace adc
{
    bool enabled_channels[6] = {false};
    volatile float channel_value[6] = {0.0};

    volatile int current_channel = -1;

    void set_admux();

void init()
{	
    ADCSRA = 0;
    ADCSRA |= ((1<<ADPS2)|(1<<ADPS1));// |(1<<ADPS0));    //16Mhz/128 = 125Khz the ADC reference clock
    ADMUX |= (1<<REFS0);                //Voltage reference from Avcc (5v)

//    ADCSRA |= (1<<ADIE); // enable interupt
    ADCSRA |= 1<<ADEN;
    sei();

    // ADMUX setup is 0b01000000 hex 40

    ADCSRA |= (1<<ADSC);                //Do an initial conversion because this one is the slowest and to ensure that everything is up and running

}


void enable(channel c)
{
    enabled_channels[c] = true;
    if(current_channel > -1)
        return;
        
    current_channel = c;
    set_admux();
	ADCSRA |= (1 << ADIE); // enable interupt
    ADCSRA |= (1 << ADSC); // read value
}

void next_enabled_channel()
{
    // set current channel to the next enabled channel
    for(int i = 0; i<6; ++i)
    {
       channel next = channel((int)current_channel + 1 % 6);
       if(enabled_channels[(int)next])
       {
            current_channel = next;
            return;
       }
    }
}

void value_ready()
{
	uint8_t low = ADCL;
	uint16_t value = ADCH<<8 | low;
	if(value < 0)
		value = 0;
	else if(value > 1024)
		value = 1024;

	float scaled = 5.0 * (value/1024.0);
    
    channel_value[current_channel] = scaled;
}

void read_next()
{
	ADCSRA |= 1<<ADSC;
}

void set_admux()
{
	switch(current_channel)
	{
		case Adc0:
			ADMUX = 0x40;
			break;
		case Adc1:
			ADMUX = 0x41;
			break;
		case Adc2:
			ADMUX = 0x42;
			break;
        case Adc3:
            ADMUX = 0x43;
            break;
        case Adc4:
            ADMUX = 0x44;
            break;
        case Adc5:
            ADMUX = 0x45;
            break;
	}
}

ISR(ADC_vect)
{
    value_ready();
    next_enabled_channel();
    set_admux();
    read_next();
}


}// end namespace
