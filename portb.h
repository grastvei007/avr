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

#ifndef PORTB_H
#define PORT_B

#include <stdlib.h>
#include <stdint.h>

typedef void (*funcPtr)(int, bool);


class PortB
{
public:
	enum Pin{
		Pin0 = 0,
		Pin1,
		Pin2,
		Pin3,
		Pin4,
		Pin5,
		Pin6,
		Pin7
	};
	PortB();
	void init();

	void updatePort();

	void portChanged(); ///< call this from an ISR.

	void setPinTriggeredFunc(funcPtr aFuncPtr);
private:
	uint8_t mPortHistory;
	bool mChangedFlag;

	funcPtr *mFuncPtr;
};

#endif

