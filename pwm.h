#ifndef PWM_H
#define PWM_H

#include <stdlib.h>
#include <stdint.h>

/** \brief
	Pwm control for Atmega328
**/
class Pwm
{
	public:
		enum Channel
		{
			eChan1 = 1,
			eChan2 = 1 << 1,
			eChan3 = 1 << 2,
			eChan4 = 1 << 3,

			eChanPd6 = 1,
			eChanPd5 = 1 << 1,
			eChanPb3 = 1 << 2,
			eChanPd3 = 1 << 3
		};
		Pwm();
		~Pwm();
		void init();

		void enable(Channel aChannel);
		void disable(Channel aChannel);
		void setDutyCycle(Channel aChannel, uint8_t aDutyCycle);
};

//static Pwm pwm;


#endif

