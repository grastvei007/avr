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

