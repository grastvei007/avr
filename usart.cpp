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

#include <avr/io.h>
#include <avr/interrupt.h>

void USART_send(char data){

 while(!(UCSR0A & (1<<UDRE0)));
 UDR0 = data;

}

void USART_putstring(char* StringPtr){

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


/*void USART_init(){
    UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
    UBRR0L = (uint8_t)(BAUD_PRESCALLER);

     UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
    //enable reception and RC complete interrupt
     UCSR0B |= (1<<TXEN0) | (1<<RXEN0)|(1<<RXCIE0);
}*/

