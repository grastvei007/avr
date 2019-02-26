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

