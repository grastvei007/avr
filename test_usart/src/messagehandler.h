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

#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H


#define BUFFER_SIZE 128 ///< default size of buffer

typedef void (*func_device_name)();

extern func_device_name callback_device_name;


typedef struct _BUFFER
{
    unsigned int head;
    unsigned int tail;
    unsigned char buffer[BUFFER_SIZE];
}MESSAGE_BUFFER;

typedef struct _MESSAGE
{
    int start_pos_in_buffer;
    unsigned int message_size;
    unsigned char message[BUFFER_SIZE];
}MESSAGE;

extern MESSAGE_BUFFER message_buffer;
extern MESSAGE current_message;

void message_handler_init();
void message_handler_clear_message_buffer();
void message_handler_insert_char(unsigned char c);

void parse_message();
void clear_message();

// look for message in the buffer
bool message_handler_scan();

int find_string(const char *token);


//typedef void (*funcPtr)(Message*&);



#endif
