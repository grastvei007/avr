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

#include "usart.h"
#include "circularbuffer.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define ASYNCHRONOUS (0<<UMSEL00) // USART Mode Selection

#define DISABLED    (0<<UPM00)
#define EVEN_PARITY (2<<UPM00)
#define ODD_PARITY  (3<<UPM00)
#define PARITY_MODE  DISABLED // USART Parity Bit Selection

#define ONE_BIT (0<<USBS0)
#define TWO_BIT (1<<USBS0)
#define STOP_BIT ONE_BIT      // USART Stop Bit Selection

#define FIVE_BIT  (0<<UCSZ00)
#define SIX_BIT   (1<<UCSZ00)
#define SEVEN_BIT (2<<UCSZ00)
#define EIGHT_BIT (3<<UCSZ00)
#define DATA_BIT   EIGHT_BIT  // USART Data Bit Selection

#define RX_COMPLETE_INTERRUPT         (1<<RXCIE0)
#define DATA_REGISTER_EMPTY_INTERRUPT (1<<UDRIE0)

volatile uint8_t USART_send_value;
CircularBuffer transmitBuffer = buffer_create();


ISR(USART_UDRE_vect)
{
    UDR0 = USART_send_value;
    UCSR0B &= ~DATA_REGISTER_EMPTY_INTERRUPT; // Disables the Interrupt, uncomment for one time transmission of data
}

void USART_send(char data)
{
    while(!(UCSR0A & (1<<UDRE0)));
        UDR0 = data;
}

void USART_putstring(char* StringPtr)
{
    while(*StringPtr != 0x00){
         USART_send(*StringPtr);
        StringPtr++;
    }
}

void USART_putMessage(char *aMsg, int size)
{
    for(int i=0; i<size; ++i)
    {
        USART_send(aMsg[i]);
    }
}


void USART_init()
{
    UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
    UBRR0L = (uint8_t)(BAUD_PRESCALLER);

   //  UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
    UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;
    //enable reception and RC complete interrupt
     UCSR0B |= (1<<TXEN0) | (1<<RXEN0)|(1<<RXCIE0);
     
    sei();
}


void USART_buffer_append(char *msg, unsigned int size)
{
    for(unsigned int i=0; i<size; ++i)
    {
        buffer_write(&transmitBuffer, msg[i]);
    }
}


void USART_buffer_send()
{
    if(!buffer_empty(&transmitBuffer))
    {
        USART_send_value = buffer_read(&transmitBuffer);
        UCSR0B |= DATA_REGISTER_EMPTY_INTERRUPT;
    }
}

