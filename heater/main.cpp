#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>
#include <string.h>

#include <messagehandler.h>
#include <messagetranslationsenter.h>
#include <message.h>
#include <usart.h>
#include <pwm.h>
#include <tag.h>

#include "pump.h"

#define F_CPU 12000000UL

MessageHandler mh;
MessageTranslationSenter mts;
Pwm pwm;
Pump pump;
volatile bool lock;

void init(); ///< init fan, and effect.

//ISR(PCINT0_vect);

enum State
{
	eInit,
	eStarting,
	eRuning,
	eStoping,
	eStoped
};

/**
	Setting for fan blowing air.
**/
struct Fan
{
	volatile int newLevel;
	volatile bool changed;
	volatile int currentLevel;
	int minLevel;
	int maxLevel;
	int pwmMin;
	int pwmMax;
}fan;

/**
	Settings for the heat, airinlet and pump speed.
**/
struct Effect
{
	volatile int newLevel;
	volatile bool changed;
	volatile int currentLevel;
	int minLevel;
	int maxLevel;
	int pwmMin;
	int pwmMax;
}effect;

State state = eInit;
int numPrePumps = 250; // 25 pumps 
volatile bool isBurning = false;

void initTimer();

void messageCallback(Message *&msg);
void requestDeviceName();
void requestCreateTags();

void onBoolValueChanged(char *aKey, bool aValue);
void onIntValueChanged(char *aKey, int aValue);

int main()
{
	lock = true;
//	cli();
	initTimer();
	USART_init();
	mh.init();
	mh.setCallback(messageCallback);

	mts.init();
	mts.setDeviceNameFunc(requestDeviceName);
	mts.setCreateTagsFunc(requestCreateTags);

	mts.setCallbackBoolValue(onBoolValueChanged);
	mts.setCallbackIntValue(onIntValueChanged);

	pwm.init();
	sei();
	pwm.enable(Pwm::eChanPb3);
	pwm.enable(Pwm::eChanPd3);
	pwm.setDutyCycle(Pwm::eChanPb3, 100);
	pwm.setDutyCycle(Pwm::eChanPd3, 100);

	lock = false;
	//ouput pins
	DDRB |= (1 << PB0);
	DDRB |= (1 << PB2);
//	DDRB |= (1 << PB3);
	DDRB |= (1 << PB4);
	DDRB |= (1 << PB5);

//	DDRD |= (1 << PD3);

	//input pins
	DDRB &= ~(1 << PB1);

	DDRD &= ~(1 << PD2);
	DDRD &= ~(1 << PD4);
	DDRD &= ~(1 << PD5);
	DDRD &= ~(1 << PD6);
	DDRD &= ~(1 << PD7);

//	PCMSK0 |= (1 << PCINT1);
//	PCICR |= (1 << PCIE0);

	pump.start();
	pump.setSpeed(255);

//	sei();

	while(true)
	{

	}

	return 0;
}


void onBoolValueChanged(char *aKey, bool aValue)
{
	if(strcmp(aKey, "on") == 0)
	{
		if(aValue)
			PORTB &= ~(1<<PB0);
		else
			PORTB |= (1<<PB0);
	}
	else if(strcmp(aKey, "burning") == 0)
	{
		isBurning = aValue;
		if(aValue)
			PORTB &= ~(1<<PB1);
		else
			PORTB |= (1 << PB1);
	}
}


void onIntValueChanged(char *aKey, int aValue)
{
	if(strcmp(aKey, "fanLevel") == 0)
	{
		fan.newLevel = aValue;
		fan.changed = true;
	}
	else if(strcmp(aKey, "effectLevel") == 0)
	{
		effect.newLevel = aValue;
		effect.changed = true;
	}
}


void messageCallback(Message *&msg)
{
	mts.translateMessage(msg);
	msg->destroy();
	free(msg);	
}


ISR(USART_RX_vect)
{
    while(!(UCSR0A&(1<<RXC0))){};
    char c = UDR0;
	mh.insertChar(c);
}


void initTimer()
{
    OCR1A = F_CPU / 256 / 10; // 10 Hz
    TCCR1B |= 1 << WGM12;
    TCCR1B |= 1 << CS12;
    TIMSK1 |= 1 << OCIE1A;
}


void requestDeviceName()
{
	Tag::createTag("deviceName", "heater");
}


void requestCreateTags()
{
	Tag::createTag("burning", false);
	Tag::createTag("fanLevel", fan.currentLevel);
	Tag::createTag("effectLevel", effect.currentLevel);
	Tag::createTag("on", false);
//	Tag::createTag("burning", false);
}


void init()
{
	fan.newLevel = 0;
	fan.changed = false;
	fan.currentLevel = 5;
	fan.minLevel = 0;
	fan.maxLevel = 10;
	fan.pwmMin = 10;
	fan.pwmMax = 255;

	effect.newLevel = 0;
	effect.changed = false;
	effect.currentLevel = 5;
	effect.minLevel = 0;
	effect.maxLevel = 10;
	effect.pwmMin = 100;
	effect.pwmMax = 255;
}


ISR(TIMER1_COMPA_vect)
{
	if(lock)
		return;
	lock = true;
	// run loop
	
	mh.run(); // run message handler

	pump.update(0.1);

	if(state == eInit)
	{
		numPrePumps--;
		if(numPrePumps <= 0)
		{
			onBoolValueChanged("burning", false);
			state = eStarting;
		}
	}
	else if(state == eStarting)
	{

		if(isBurning)
			state = eRuning;

	}
	else if(state == eRuning)
	{

	if(fan.changed)
	{
			fan.currentLevel = fan.newLevel;

			uint8_t pwmValue = 100 + (fan.currentLevel*15);
			pwm.setDutyCycle(Pwm::eChanPb3, pwmValue);;
			fan.changed = false;

			pump.setSpeed(25*fan.currentLevel); //TODO: move to effect.
		}
		if(effect.changed)
		{
			effect.currentLevel = effect.newLevel;
			uint8_t pwmValue = 100 + (effect.currentLevel*15);
			pwm.setDutyCycle(Pwm::eChanPd3, pwmValue);
			effect.changed = false;
		}
	}
	else if(state == eStoping)
	{

	}
	else if(state == eStoped)
	{

	}

	lock = false;
}


ISR(PCINT0_vect)
{
	if(!PINB & (1 << PINB1))
	{
		onBoolValueChanged("on", true);
	}

}

