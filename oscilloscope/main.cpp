#include "src/usart.h"
#include "src/adc.h"
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>


int main()
{
    usart_init(9600);
    adc::init();
    sei();
    adc::enable(adc::Adc0);
    usart_transmit_string("usart started");

    int current_channel = 0;
    _delay_ms(1000);

    while(true)
    {
        if(adc::enabled_channels[current_channel])
        {
            int value = adc::channel_value[current_channel] * 100;

            unsigned char buffer[7];
            char vb[4];
            char c[1];
            sprintf(c, "%d", current_channel);
            buffer[0] = c[0];
            buffer[1] = ':';

            int n = sprintf(vb, "%d", value);
            for(int i = 0; i<n; ++i)
                buffer[2 + i] = vb[i];
            buffer[2 + n] = ':';
            usart_transmit_bytes(buffer, 3 + n);


        }
        // next channel
        current_channel = (current_channel + 1) % 6;
        _delay_ms(10);
    }

    return 0;
}
