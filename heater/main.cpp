#include "main.h"



int main()
{
	_delay_ms(500);
	lock = true;
	USART_init();
	sei();
//	cli();
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

/*	adc.init();
	adc.setCallbackFunc(onAdcValueChanged);
	adc.enable();
	adc.enableChannel(Adc::eAdc0);
*/


	//ouput pins
//	DDRB |= (1 << PB0);
	DDRB |= (1 << PB1);

	pump.init();
	pump.start();
//	pump.setSpeed(255);

//	USART_init();
	initTimer();
	sei();
	init();
	lock = false;
	while(true)
	{

	}

	return 0;
}

void turnGlowPlugOn(bool aOn)
{
	if(aOn)
	{
		PORTB |= (1 << PB1);
	}
	else
	{
		PORTB &= ~(1 << PB1);
	}
}

void onAdcValueChanged(int aChannel)
{
	adc0Value = adc.getChannelReading(Adc::Channel(aChannel));
	adc.readNext();
}


void onBoolValueChanged(char *aKey, bool aValue)
{
	if(strcmp(aKey, "on") == 0)
	{
		on = !aValue;
	}
	else if(strcmp(aKey, "burning") == 0)
	{
		isBurning = aValue;
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

ISR(ADC_Vect)
{
	adc.valueReady();
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
	returnState = state;
	tagNumber = 0;
	state = eSendTags;
	isTagsRequested = true;
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
	if(countDown > 0)
	{
		countDown--;
		return;
	}
	
	if(lock)
		return;
	lock = true;
	// run loop
	
	mh.run(); // run message handler

	pump.update(0.1);

	if(state == eInit)
	{
	//	Tag::setValue("state", "init");
		numPrePumps--;
		if(numPrePumps <= 0)
		{
//			pump.start();
//			pump.setSpeed(255);
//			Tag::setValue("on", false);
//			Tag::setValue("burning", false);
			isBurning = false;
			on = false;
			state = eStarting;
			pwm.setDutyCycle(Pwm::eChanPd3, 250);
			effect.currentLevel = 9;
			effect.newLevel = 9;
			turnGlowPlugOn(true);
		//	Tag::setValue("state", "starting");
		}
	}
	else if(state == eStarting)
	{

		if(isBurning)
		{
			state = eRunning;
	//		Tag::setValue("on", true);
			on = true;
			effect.currentLevel = 6;
			effect.newLevel = 6;
			effect.changed = true;
			fan.currentLevel = 6;
			fan.newLevel = 6;
			fan.changed = true;
			turnGlowPlugOn(false);
		//	Tag::setValue("state", "running");
		}

	}
	else if(state == eRunning)
	{
        if(fan.changed)
        {
            fan.currentLevel = fan.newLevel;

            uint8_t pwmValue = 100 + (fan.currentLevel*15);
            if(fan.currentLevel > 10)
                    pwmValue = 255;
            pwm.setDutyCycle(Pwm::eChanPb3, pwmValue);;
            fan.changed = false;
    //		Tag::setValue("statusFan", fan.currentLevel);
        }
        if(effect.changed)
        {
            effect.currentLevel = effect.newLevel;
            uint8_t pwmValue = 100 + (effect.currentLevel*15);
            if(effect.currentLevel > 10)
            {
                pwmValue = 255;
                effect.currentLevel = 10;
            }
            pwm.setDutyCycle(Pwm::eChanPd3, pwmValue);
            effect.changed = false;
            pump.setSpeed(25*effect.currentLevel);
    //		Tag::setValue("statusHeat", effect.currentLevel);
        }
		if(!on)
		{
			pump.stop();
			state = eStopping;
			Tag::setValue("state", "Stopping");
		}
	}
	else if(state == eStopping)
	{
		if(!isBurning)
		{
			pwm.setDutyCycle(Pwm::eChanPb3, 0);
			pwm.setDutyCycle(Pwm::eChanPd3, 0);
			state = eStopped;
			Tag::setValue("state", "Stopped");
		}
	}
	else if(state == eStopped)
	{

	}
	else if(state == eSendTags)
	{
		if(tagNumber == 0)
		    Tag::createTag("burning", false);
		else if(tagNumber == 1)
		    Tag::createTag("fanLevel", fan.currentLevel);
		else if(tagNumber == 2)
	    	Tag::createTag("effectLevel", effect.currentLevel);
		else if(tagNumber == 3)
		    Tag::createTag("on", false);
		else if(tagNumber == 4)
			Tag::createTag("state", "init");
		else if(tagNumber == 5)
			Tag::createTag("adc0", adc0Value);
		else if(tagNumber == 6)
			Tag::createTag("statusHeat", effect.currentLevel);
		else if(tagNumber == 7)
			Tag::createTag("statusFan", fan.currentLevel);
		else
			state = returnState;
		tagNumber++;
	}

	USART_buffer_send(); // send one char from the buffer
	lock = false;
}


/*ISR(PCINT0_vect)
{
	if(!PINB & (1 << PINB1))
	{
		onBoolValueChanged("on", true);
	}

}
*/
