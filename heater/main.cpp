#include "main.h"



int main()
{
	lock = true;
    init();

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
	//lock = false;
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
		on = aValue;
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

/**
 * CS12    CS11    CS10      Prescaler
  0       0       0       Timer off/no clock source
  0       0       1       1:1 prescaler/this is the same as not having prescaler
  0       1       0       1:8 prescaler
  0       1       1       1:64 prescaler
  1       0       0       1:256 prescaler
  1       0       1       1:1024 prescaler

Timer resolution = (Prescaler / Input frequency)
Prescaler values can be  1, 8, 64, 256 or 1024
And the input frequency is 12Mhz.

Prescaler valuer | Timer resolution 16MHz       | Timer resolution 12 MHz
1                | 0,0000000625s = 62,5 nS      | 0.000000083333 = 83.33ns
8                | 0,0000005s = 0,5uS           | 0.00000066667  = 0.6667 us
64               | 0,000004s = 4uS              | 0.0000053333   = 5.3333 us
256              | 0,000016s = 16us             | 0.000021333    = 21.333 us
1024             | 0,000064s = 64uS             | 0.000085333    = 85.333 us

 I want 50Hz then:
 50 Hz target delay 20ms

 prescale   |   value 12Mhz
 1          | 240000.96
 8          | 29999.85
 64         | 3750
 256        | 937.51
 1024       | 234.38
 */
void initTimer()
{
    TCCR1B = (1 << WGM12) | (1<<CS11) | (1<<CS10); // CTC mode and presale 64
    OCR1A = 3750;
    TIMSK1 = 1 << OCIE1A; // enable interrupt
}


void requestDeviceName()
{
	Tag::createTag("deviceName", "heater");
}


void requestCreateTags()
{
	returnState = state;
	tagNumber = 0;
	nextState = eSendTags;
	lock = false;
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

/**
 * Interrupt is called 50 times a second.
 */
ISR(TIMER1_COMPA_vect)
{
    USART_buffer_send();

    counter++;
    if(counter < 10)
        return;
    else
        counter = 0;
	// run loop
	mh.run(); // run message handler
    if(lock)
        return;

    pump.update(0.1);

	if(nextState != state)
    {
	    state = nextState;
	    writeStateTag();
    }

	if(state == eInit)
	{
		numPrePumps--;
		if(numPrePumps <= 0)
		{
//			pump.setSpeed(255);
			Tag::setValue("on", false);
			Tag::setValue("burning", false);
			//isBurning = false;
			//on = false;
			pwm.setDutyCycle(Pwm::eChanPd3, 250);
			effect.currentLevel = 9;
			effect.newLevel = 9;
			turnGlowPlugOn(true);
			nextState = eStarting;
		}
	}
	else if(state == eStarting)
	{
		if(isBurning)
		{
			//Tag::setValue("on", true);
			on = true;
			effect.currentLevel = 6;
			effect.newLevel = 6;
			effect.changed = true;
			fan.currentLevel = 6;
			fan.newLevel = 6;
			fan.changed = true;
			turnGlowPlugOn(false);
            Tag::setValue("statusFan", fan.currentLevel);
            Tag::setValue("statusHeat", effect.currentLevel);
			nextState = eRunning;
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
    		Tag::setValue("statusFan", fan.currentLevel);
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
    		Tag::setValue("statusHeat", effect.currentLevel);
        }
		if(!on)
		{
			pump.stop();
			nextState = eStopping;
		}
	}
	else if(state == eStopping)
	{
		if(!isBurning)
		{
			pwm.setDutyCycle(Pwm::eChanPb3, 0);
			pwm.setDutyCycle(Pwm::eChanPd3, 0);
			nextState = eStopped;
		}
	}
	else if(state == eStopped)
	{

	}
	else if(state == eSendTags)
	{
		if(tagNumber == 0)
		    Tag::createTag("burning", false);
		else if(tagNumber == 2)
		    Tag::createTag("fanLevel", fan.currentLevel);
		else if(tagNumber == 4)
	    	Tag::createTag("effectLevel", effect.currentLevel);
		else if(tagNumber == 6)
		    Tag::createTag("on", false);
		else if(tagNumber == 8)
			Tag::createTag("state", "init");
		else if(tagNumber == 10)
			Tag::createTag("adc0", adc0Value);
		else if(tagNumber == 12)
			Tag::createTag("statusHeat", effect.currentLevel);
		else if(tagNumber == 14)
			Tag::createTag("statusFan", fan.currentLevel);
		else if(tagNumber > 16)
			nextState = returnState;
		tagNumber++;
	}
}


/*ISR(PCINT0_vect)
{
	if(!PINB & (1 << PINB1))
	{
		onBoolValueChanged("on", true);
	}

}
*/

void writeStateTag()
{
    switch (state)
    {
        case eInit:
            Tag::setValue("state", "init");
            break;
        case eStarting:
            Tag::setValue("state", "starting");
            break;
        case eRunning:
            Tag::setValue("state", "running");
            break;
        case eStopping:
            Tag::setValue("state", "stopping");
            break;
        case eStopped:
            Tag::setValue("state", "stopped");
            break;
        case eSendTags:
            Tag::setValue("state", "sendTag");
            break;
        default:
            Tag::setValue("state", "unknown");
            break;
    }
}