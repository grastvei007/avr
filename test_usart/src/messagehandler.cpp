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

#include "messagehandler.h"
#include "usart.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

MESSAGE_BUFFER message_buffer = {0,0, {0}};
MESSAGE current_message = {-1, 0, {0}};

void message_handler_init()
{
    usart_init(9600);
}

void message_handler_clear_message_buffer()
{
    message_buffer.head = 0;
    message_buffer.tail = 0;
    memset(&message_buffer.buffer, '0', BUFFER_SIZE);
}

void message_handler_insert_char(unsigned char c)
{
    unsigned char next_head = (message_buffer.head + 1) % BUFFER_SIZE;

    message_buffer.buffer[message_buffer.head] = c;
    message_buffer.head = next_head;
}



bool message_handler_scan()
{
    if(current_message.start_pos_in_buffer < 0)
    {
        int start_pos = find_string("<msg");
        if( start_pos < 0)
            return false;
        current_message.start_pos_in_buffer = start_pos;
    }
    // message has a start position in receive buffer
    // try to find a complete message from this position.
    int pos = current_message.start_pos_in_buffer;

    char buffer[2] = {0};
    buffer[0] = message_buffer.buffer[(pos + 4) % BUFFER_SIZE];
    buffer[1] = message_buffer.buffer[(pos + 5) % BUFFER_SIZE];

    int msg_size = atoi(buffer);

    int sum = 0;
    for(int i = pos; i < msg_size; ++i)
    {
        sum += (int)message_buffer.buffer[i % BUFFER_SIZE];
    }

    if(sum % 256 != 0)
        return false;


    // indicate to look for new message
    current_message.start_pos_in_buffer = -1;

    message_buffer.tail = (message_buffer.tail + msg_size-1) % BUFFER_SIZE;

    current_message.message_size = 0;
    // copy message to current message
    for(int i = pos; i < msg_size; ++i)
    {
        current_message.message[current_message.message_size++] = message_buffer.buffer[i % BUFFER_SIZE];
    }
    usart_transmit_bytes(current_message.message, current_message.message_size);
    return true;
}

int find_string(const char *token)
{
    int length = strlen(token);
    if(length <= 0)
        return -1;

    for(unsigned int i = message_buffer.tail; ;)
//    for(unsigned int i = 0; i < BUFFER_SIZE; ++i)
    {
        // match first character
        if(message_buffer.buffer[i] == token[0])
        {
            int token_pos = 0;
            bool found = false;
            for(unsigned int pos = i; pos < i + length; ++pos)
            {
                if(message_buffer.buffer[pos % BUFFER_SIZE] == token[token_pos++])
                    found = true;
                else
                {
                    found = false;
                    break;
                }
            }
            if(found)
                return i;
        }

        if(i == (message_buffer.head) % BUFFER_SIZE)
            break;

        i = (i + 1) % BUFFER_SIZE;
    }
    return -1;
}

bool is_key_device_name(char key[])
{
    char compare[] = {'d','e','v','i','c','e','N','a','m','e'};
    for(int i = 0; i < 10; ++i)
    {
        if(key[i] != compare[i])
            return false;
    }
    return true;
}


void parse_message()
{
    if(current_message.message_size < 2)
        return;

    // key starts after heder + 2 bytes representing the message size
    int index = 6;
    char key[15] = {0};
    int key_index = 0;
    for(int i = index; i<current_message.message_size - index; ++i)
    {
        unsigned char c = current_message.message[i];
        if(c == ':')
        {
            //usart_transmit_bytes(key, 10);
            if(is_key_device_name(key))
                callback_device_name();
            return;
        }            

        key[key_index++] = c;
    }

}

void clear_message()
{
    current_message.message_size = 0;
    memset(&current_message.message, 0, BUFFER_SIZE);
}
