/* This file is part of June.

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

#include "usart.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/setbaud.h>
#include <string.h>

USART_BUFFER usart_rx_buffer = {{0}, 0, 0};
USART_BUFFER usart_tx_buffer = {{0}, 0, 0};


void usart_transmit(uint8_t data)
{
    unsigned int next_head = (usart_tx_buffer.head + 1) % USART_BUFFER_SIZE;

    //wait
//    while(next_head == usart_tx_buffer.tail);
    
    usart_tx_buffer.buffer[usart_tx_buffer.head] = data;
    usart_tx_buffer.head = next_head;

    // enable TX interupt
    USART0_CONTROL |= (1 << UDRIE0);

//    while(!(UCSR0A & (1 << UDRE0)));
//    UDR0 = data;
}

void usart_transmit_bytes(unsigned char bytes[], unsigned int size)
{
    unsigned char c;
    for(unsigned int i=0; i < size; ++i)
    {
        c = bytes[i];
        usart_transmit(c);
    }
}

void usart_transmit_string(char *data)
{
/*    while(data)
    {
        usart_transmit(*(++data));
    }*/

  unsigned char c = *data;
  while(c)
  {
 //   while(!(UCSR0A & (1 << UDRE0)));
   // UDR0 = c;
    usart_transmit(c);
    c = *(++data);
  }
}

void usart_init(unsigned int baud)
{
    UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
    UBRR0L = (uint8_t)(BAUD_PRESCALLER);
//    UDBR0 = USART_BAUDRATE(baud);

    // enable rx and tx
//    USART0_CONTROL |= (1 << RXEN0) | (1 << TXEN0);

    // enable rx interupt
//    USART0_CONTROL |= (1 << RXCIE0);

    // Set frame format: 8data, 1 stop bit
//    USART0_STATUS = (1 << UCSZ01) | (1 << UCSZ00);

    // enable transmit and receive
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);

    UCSR0B |= (1 << RXCIE0);

    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
     
    // enable global interupt
//    sei();
}

void usart_put_char_in_buffer(unsigned char c)
{
    unsigned int i = (unsigned int)(usart_rx_buffer.head + 1) % USART_BUFFER_SIZE;

    if(i != usart_rx_buffer.tail)
    {
        usart_rx_buffer.buffer[usart_rx_buffer.head] = c;
        usart_rx_buffer.head = i;
    }
}

void usart_clear_buffer()
{
  usart_rx_buffer.head = 0;
  usart_rx_buffer.tail = 0;
  usart_tx_buffer.head = 0;
  usart_tx_buffer.tail = 0;
}

uint8_t usart_data_available()
{
    return (uint8_t)(USART_BUFFER_SIZE + usart_rx_buffer.head - usart_rx_buffer.tail) % USART_BUFFER_SIZE;
}

uint8_t usart_read_data()
{
    if(usart_rx_buffer.head == usart_rx_buffer.tail)
        return -1;
    else
    {
        uint8_t data = usart_rx_buffer.buffer[usart_rx_buffer.tail];
        usart_rx_buffer.tail = (unsigned int)(usart_rx_buffer.tail + 1) % USART_BUFFER_SIZE;
        return data;
    }
}

ISR(USART_RX_vect)
{
    unsigned char data = UDR0;
    usart_put_char_in_buffer(data);
}

ISR(USART_UDRE_vect)
{
    if(usart_tx_buffer.head != usart_tx_buffer.tail)
    {
        UDR0 = usart_tx_buffer.buffer[usart_tx_buffer.tail];
        usart_tx_buffer.tail = (usart_tx_buffer.tail + 1) % USART_BUFFER_SIZE;
    }
    else
    {
        USART0_CONTROL &= ~(1 << UDRIE0);
    }
}

