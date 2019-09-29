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

/**
	Setting for fan blowing air.
**/
struct Fan
{
	volatile int newLevel;
	volatile bool changed;
	int currentLevel;
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
	int currentLevel;
	int minLevel;
	int maxLevel;
	int pwmMin;
	int pwmMax;
}effect;



void initTimer();

void messageCallback(Message *&msg);
void requestDeviceName();
void requestCreateTags();

void onBoolValueChanged(char *aKey, bool aValue);
void onIntValueChanged(char *aKey, int aValue);

int main()
{
	lock = true;
	cli();
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
	pwm.enable(Pwm::eChanPb3);
	pwm.enable(Pwm::eChanPd3);
	pwm.setDutyCycle(Pwm::eChanPb3, 0);
	pwm.setDutyCycle(Pwm::eChanPd3, 0);

	lock = false;
	sei();

	DDRB |= (1<<PB0);
	DDRB |= (1<<PB2);

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
	//Tag::createTag("pb0", false);
	Tag::createTag("fanLevel", fan.currentLevel);
	Tag::createTag("effectLevel", effect.currentLevel);
	Tag::createTag("on", false);
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
	effect.currentLevel = 0;
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

	if(fan.changed)
	{
		fan.currentLevel = fan.newLevel;
//		Tag::setValue("fanLevel", fan.currentLevel);

		double step = (fan.pwmMax - fan.pwmMin)/(double)fan.maxLevel;
		uint8_t pwmValue = fan.pwmMin + (step*fan.currentLevel);
		pwm.setDutyCycle(Pwm::eChanPb3, fan.currentLevel);//  pwmValue);
		fan.changed = false;

		pump.setSpeed(25*fan.currentLevel); //TODO: move to effect.
	}
	if(effect.changed)
	{
		effect.currentLevel = effect.newLevel;
//		Tag::setValue("effectLevel", effect.currentLevel);

		double step = (effect.pwmMax - effect.pwmMin)/(double)effect.maxLevel;
		uint8_t pwmValue = effect.pwmMin + (step*effect.currentLevel);
		pwm.setDutyCycle(Pwm::eChanPd3, effect.currentLevel);//pwmValue);
		effect.changed = false;
	}

	lock = false;
}
