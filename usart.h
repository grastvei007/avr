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

#define F_CPU 12000000UL
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 12UL))) - 1)



void USART_init(void);
// UnBuffered sending
void USART_send(char data);
void USART_putstring(char* StringPtr);
void USART_putMessage(char *aMsg, int size);

//Buffered sending
void USART_buffer_append(char *msg, unsigned int size); ///< append string to buffer
void USART_buffer_send(); ///< send first char from buffer

#endif

