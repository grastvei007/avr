#include "src/usart.h"
#include "src/messagehandler.h"
#include "src/tag.h"
#include <avr/delay.h>
#include <avr/interrupt.h>

void set_device_name()
{
    usart_transmit_string("Hello");
}

func_device_name callback_device_name = &set_device_name;

int main()
{
    message_handler_init();
    sei();
    usart_transmit_string("usart started");

    while(true)
    {
        tag::set_value("ts", "test", 100);
        // update data
/*        while(usart_data_available())
        {
            unsigned char data = usart_read_data();
            message_handler_insert_char(data);
            if(message_handler_scan())
            {
                message_handler_clear_message_buffer();
                usart_transmit_string("message found");
                parse_message();
                clear_message();
                // handle message
                break;
            }
        }*/

        _delay_ms(1000);
//        usart_transmit_string("Hei");
    }

    return 0;
}
