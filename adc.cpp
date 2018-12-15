#include "adc.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>

Adc::Adc()
{
	mCallbackAdc0 = NULL;


}


Adc::~Adc()
{


}


void Adc::init()
{
    ADCSRA = 0;
    ADCSRA |= ((1<<ADPS2)|(1<<ADPS1));// |(1<<ADPS0));    //16Mhz/128 = 125Khz the ADC reference clock
    ADMUX |= (1<<REFS0);                //Voltage reference from Avcc (5v)

//    ADCSRA |= (1<<ADIE); // enable interupt
    ADCSRA |= 1<<ADEN;
  //  sei();

    // ADMUX setup is 0b01000000 hex 40

    ADCSRA |= (1<<ADSC);                //Do an initial conversion because this one is the slowest and to ensure that everything is up and running

//	ADCSRA |= (1<<ADIE); // enable interupt

}


void Adc::enable()
{
	ADCSRA |= (1<<ADIE); // enable interupt
}


void Adc::disable()
{

}


ISR(ADC_vect)
{
	static int ready = 0;

    uint8_t low = ADCL;
    uint16_t value = ADCH<<8 | low;

    if(value < 0)
        value = 0;
    else if(value > 1024)
        value = 1024;

    // scale adc value to range [0, 5.0] volt
    double scale = 5.0 * (value/1024.0);

    switch(ADMUX)
    {
        case 0x40 :
			if(ready > 0)
			{
//	            value_adc0 = scale;
    	        ADMUX = 0x41;
				ready = 0;
			}
			else
				ready++;
            break;

        case 0x41 :
			if(ready > 0)
			{
  //          	value_adc1 = scale;
            	ADMUX = 0x42;
				ready = 0;
			}
			else
				ready++;
            break;

        case 0x42 :
			if(ready > 0)
			{
    //        	value_adc2 = scale;
            	ADMUX = 0x40;
				ready = 0;
			}
			else
				ready++;
            break;

    }

    ADCSRA |= 1<<ADSC;
}
