
#include <util/delay.h>
#include <avr/io.h>

#include <i2c.h>
#include <ssd1306.h>
#include <usart.h>

enum states
{
	eInit,
	eStart,
	eNext
};


int main()
{
	DDRC = 0x00;
	states state = eInit;
	USART_init();

	USART_putstring("Test");

	while(true)
	{
		switch(state)
		{
			case  eInit:
			{
				USART_putstring("init\n");
				i2c_init();
				state = eStart;
			}
			case eStart:
				USART_putstring("start\n");
				_delay_ms(10);
				init_display();
				clear_display();
				setCursor(2,2);
				writeString("June");
				state = eNext;
				break;

			case eNext:
				break;
		}
	}

	return 0;
}
