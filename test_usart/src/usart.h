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

#ifndef USART_H
#define USART_H

// Allow F_CPU and BAUD to be set from Makefile,
// but use some defaults if not.
#ifndef F_CPU
  #define F_CPU 12000000UL
#endif
#ifndef BAUD
  #define BAUD 9600
#endif
#define BAUD_PRESCALLER (((F_CPU / (BAUD * 12UL))) - 1)

#define USART0_CONTROL UCSR0B
#define USART0_STATUS UCSR0C

typedef unsigned char uint8_t;

#define USART_BUFFER_SIZE 128

typedef volatile struct
{
  unsigned char buffer[USART_BUFFER_SIZE];
  unsigned int head;
  unsigned int tail;
}USART_BUFFER;

extern USART_BUFFER usart_rx_buffer;
extern USART_BUFFER usart_tx_buffer;


void usart_init(unsigned int baud);
void usart_transmit(uint8_t data);
void usart_transmit_bytes(unsigned char data[], unsigned int size);
void usart_transmit_string(char *data);

void usart_put_char_in_buffer(unsigned char c);
void usart_clear_buffer();
uint8_t usart_data_available();

uint8_t usart_read_data();
#endif
