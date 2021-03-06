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

#ifndef AVR_TEST

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#else
#include <QDebug>
#endif

#include <stdlib.h>
#include <math.h>

#include "usart.h"

Adc::Adc()
{
	init();
}


Adc::~Adc()
{


}


void Adc::init()
{

	mCallback = NULL;
	for(int i=0; i<5; ++i)
	{
		mChannel[i] = false;
		mCurrentReading[i] = 0.0;
	}
	
#ifndef AVR_TEST
    ADCSRA = 0;
    ADCSRA |= ((1<<ADPS2)|(1<<ADPS1));// |(1<<ADPS0));    //16Mhz/128 = 125Khz the ADC reference clock
    ADMUX |= (1<<REFS0);                //Voltage reference from Avcc (5v)

//    ADCSRA |= (1<<ADIE); // enable interupt
    ADCSRA |= 1<<ADEN;
    sei();

    // ADMUX setup is 0b01000000 hex 40

    ADCSRA |= (1<<ADSC);                //Do an initial conversion because this one is the slowest and to ensure that everything is up and running

//	ADCSRA |= (1<<ADIE); // enable interupt

#endif

}


void Adc::enable()
{
#ifndef AVR_TEST
	ADCSRA |= (1<<ADIE); // enable interupt
#else
    qDebug() << __FUNCTION__;
#endif
}


void Adc::disable()
{

}


void Adc::enableChannel(Channel aChannel)
{
	mChannel[aChannel] = true;
#ifdef AVR_TEST
    qDebug() << __FUNCTION__ << " " << aChannel;
#endif

}


void Adc::disableChannel(Channel aChannel)
{
	mChannel[aChannel] = false;
}


void Adc::setCallbackFunc(callbackFuncAdc aFunc)
{
    mCallback = aFunc;
}


bool Adc::isChannelEnabled(Adc::Channel aChannel)
{
	return mChannel[aChannel];
}


void Adc::setChannelValue(float aValue, Adc::Channel aChannel)
{
	float e = mCurrentReading[aChannel] - aValue;
	if( fabs(e) < 0.01)
	{
//		readNext();
		return; 
	}
	mCurrentReading[aChannel] = aValue;
    if(mCallback)
	{
        mCallback(aChannel);
	}
}


Adc::Channel Adc::nextEnabledChannel()
{
	int channel = int(mCurrentChannel) + 1;
	for(int i=0; i<7; ++i)
	{
		if(mChannel[ (channel + i) % 6])
		{
			Channel c;
			c = Channel((channel +i)% 6);
			return c;
		}
	}
}


void Adc::valueReady()
{
#ifndef AVR_TEST
	static int ready = 0;
	uint8_t low = ADCL;
	uint16_t value = ADCH<<8 | low;
	if(value < 0)
		value = 0;
	else if(value > 1024)
		value = 1024;

	float scaled = 5.0 * (value/1024.0);

	if(ready > 0)
	{	
		setChannelValue(scaled, mCurrentChannel);
		mCurrentChannel = nextEnabledChannel();
		setAdmux();
        ready = 0;
		mDataReady = true;
	}
	else
	{
		ready++;
		ADCSRA |= 1<<ADSC;
	}

	// do a reading!
    //TODO: put into a free ride mode.
	//ADCSRA |= 1<<ADSC;
#else
    setChannelValue(1.0, mCurrentChannel);
    mCurrentChannel = nextEnabledChannel();
    setAdmux();
#endif
}

void Adc::readNext()
{
	mDataReady = false;
#ifndef AVR_TEST
	ADCSRA |= 1<<ADSC;
#endif
}

void Adc::setAdmux()
{
#ifndef AVR_TEST
	switch(mCurrentChannel)
	{
		case eAdc0:
			ADMUX = 0x40;
			break;
		case eAdc1:
			ADMUX = 0x41;
			break;
		case eAdc2:
			ADMUX = 0x42;
			break;
	}
#else
    qDebug() << __FUNCTION__ << " mux channel: " << mCurrentChannel;
#endif
}

#ifndef AVR_TEST
/*ISR(ADC_vect)
{
	// update the adc with value.
	adc.valueReady();
}*/
#endif


float Adc::getChannelReading(Adc::Channel aChannel)
{
    return mCurrentReading[aChannel];
}


bool Adc::isDataReady() const
{
	return mDataReady;
}
