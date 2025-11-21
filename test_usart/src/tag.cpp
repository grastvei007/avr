#include "tag.h"

#include <string.h>
#include <stdio.h>
#include "usart.h"

namespace tag
{
const int MESSAGE_SIZE = 128;
unsigned char message[MESSAGE_SIZE] = {0};
unsigned int message_size = 0;
unsigned int message_pos = 0;

void reset_buffer()
{
    message_pos = 0;
    message[message_pos++] = '<';
    message[message_pos++] = 'm';
    message[message_pos++] = 's';
    message[message_pos++] = 'g';
    message[message_pos++] = '0';
    message[message_pos++] = '0';
    for(int i = message_pos; i < MESSAGE_SIZE; ++i)
        message[i] = int(0);
}

void set_value(char *subsystem, char *name, int value)
{
    reset_buffer();
    for(unsigned int i=0; i<strlen(subsystem); ++i)
    {
        message[message_pos++] = subsystem[i];
    }
    message[message_pos++] = '.';

    for(unsigned int i = 0; i < strlen(name); ++i)
    {
        message[message_pos++] = name[i];
    }

    message[message_pos++] = ':';
    message[message_pos++] = 'i';

    union{
        unsigned char c[4];
        int i;
    }u;
    u.i = value;

    message[message_pos++] = u.c[0];
    message[message_pos++] = u.c[1];
    message[message_pos++] = u.c[2];
    message[message_pos++] = u.c[3];

    message[message_pos + 1] = '>';

    char buffer[2];
    sprintf(buffer, "%d", message_pos + 1);
    message[4] = buffer[0];
    message[5] = buffer[1];

    int sum = 0;
    for(unsigned int i=0; i<message_pos + 2; ++i)
        sum += (int)message[i];

    message[message_pos++] = sum % 256;
    usart_transmit_bytes(message, message_pos+1);
}

void set_value(char *subsystem, char *name, float value)
{
    reset_buffer();
    for(unsigned int i = 0; i < strlen(subsystem); ++i)
    {
        message[message_pos++] = subsystem[i];
    }
    message[message_pos++] = '.';

    for(unsigned int i = 0; i < strlen(name); ++i)
    {
        message[message_pos++] = name[i];
    }

    message[message_pos++] = ':';
    message[message_pos++] = 'f';

    union{
        unsigned char byte[4];
        float f;
    }v;
    v.f = value;

    message[message_pos++] = v.byte[0];
    message[message_pos++] = v.byte[1];
    message[message_pos++] = v.byte[2];
    message[message_pos++] = v.byte[3];

    message[message_pos + 1] = '>';

    char buffer[2];
    sprintf(buffer, "%d", message_pos + 1);
    message[4] = buffer[0];
    message[5] = buffer[1];

    int sum = 0;
    for(unsigned int i = 0; i < message_pos + 2; ++i)
        sum += (int)message[i];

    message[message_pos++] = sum % 256;
    usart_transmit_bytes(message, message_pos + 1);
}

void set_value(char *subsystem, char *name, bool value)
{
    reset_buffer();
    for(unsigned int i = 0; i < strlen(subsystem); ++i)
        message[message_pos++] = subsystem[i];
    message[message_pos++] = '.';
    for(unsigned int i = 0; i < strlen(name); ++i)
        message[message_pos++] = name[i];

    message[message_pos++] = ':';
    message[message_pos++] = 'b';

    message[message_pos++] = value ? int(1) : int(0);

    message[message_pos + 1] = '>';

    char buffer[2];
    sprintf(buffer, "%d", message_pos + 1);
    message[4] = buffer[0];
    message[5] = buffer[1];

    int sum = 0;
    for(unsigned int i = 0; i < message_pos + 2; ++i)
        sum += (int)message[i];

    message[message_pos++] = sum % 256;
    usart_transmit_bytes(message, message_pos + 1);
}

}
