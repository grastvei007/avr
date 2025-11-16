#include "usart.h"
#include <avr/delay.h>
#include <avr/interrupt.h>

int main()
{
//    _delay_ms(5000);
    usart_init(9600);
    sei();
    usart_transmit_string("usart started");

    while(true)
    {
        _delay_ms(100);
        usart_transmit_string("Hei");
    }

    return 0;
}
