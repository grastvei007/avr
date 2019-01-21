#include "adc.h"

#ifndef ADC_TEST

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#endif

#include <stdlib.h>

Adc::Adc()
{
	init();
}


Adc::~Adc()
{


}


void Adc::init()
{

#ifndef ADC_TEST
	for(int i=0; i<5; ++i)
	{
		mCallbackFunc[i] = NULL;
		mChannel[i] = false;
		mCurrentReading[i] = 0.0;
	}
	

    ADCSRA = 0;
    ADCSRA |= ((1<<ADPS2)|(1<<ADPS1));// |(1<<ADPS0));    //16Mhz/128 = 125Khz the ADC reference clock
    ADMUX |= (1<<REFS0);                //Voltage reference from Avcc (5v)

//    ADCSRA |= (1<<ADIE); // enable interupt
    ADCSRA |= 1<<ADEN;
  //  sei();

    // ADMUX setup is 0b01000000 hex 40

    ADCSRA |= (1<<ADSC);                //Do an initial conversion because this one is the slowest and to ensure that everything is up and running

//	ADCSRA |= (1<<ADIE); // enable interupt

#endif

}


void Adc::enable()
{
#ifndef ADC_TEST
	ADCSRA |= (1<<ADIE); // enable interupt
#endif
}


void Adc::disable()
{

}


void Adc::enableChannel(Channel aChannel)
{
	mChannel[aChannel] = true;
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
	mCurrentReading[aChannel] = aValue;
    if(mCallback)
        mCallback(aChannel);
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
#ifndef ADC_TEST
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
	}
	else
		ready++;

	// do a reading!
	ADCSRA |= 1<<ADSC;
#endif
}


void Adc::setAdmux()
{
#ifndef ADC_TEST
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
#endif
}

#ifndef ADC_TEST
ISR(ADC_vect)
{
	// update the adc with value.
	adc.valueReady();
}
#endif
