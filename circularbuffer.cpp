//
// Created by steini on 1/30/21.
//

#include "circularbuffer.h"
#include <string.h>

CircularBuffer buffer_create()
{
    CircularBuffer buffer;
    buffer.readPosition = 0;
    buffer.writePosition = 0;
    for(int i=0; i<BUFFER_SIZE; ++i)
        buffer.buffer[i] = char(0);
    return buffer;
}

void buffer_write(CircularBuffer *buffer, char c)
{
    buffer->buffer[buffer->writePosition] = c;
    buffer->writePosition = buffer_next_pos(buffer->writePosition);
}

void buffer_write_pos(CircularBuffer *buffer, int pos, char c)
{
    buffer->buffer[pos] = c;
}

char buffer_read(CircularBuffer *buffer)
{
    char value = buffer->buffer[buffer->readPosition];
    buffer->readPosition = buffer_next_pos(buffer->readPosition);
    return value;
}

char buffer_read_pos(CircularBuffer *buffer, int pos)
{
    return buffer->buffer[pos];
}

bool buffer_empty(CircularBuffer *buffer)
{
    return (buffer->readPosition == buffer->writePosition);
}

void buffer_erase(CircularBuffer *buffer, int startPos, int nElements)
{
    int pos = startPos;
    for(int i=0; i<nElements; ++i)
    {
        buffer_write_pos(buffer, pos, char(0));
        pos = buffer_next_pos(pos);
    }
}

int buffer_find(CircularBuffer *buffer, char *str, int startPos)
{
    int i = 0;
    int length = strlen(str);
    int pos = startPos;

    for(i; i<BUFFER_SIZE; ++i)
    {
        int findPos = pos;
        for(int k=0; k<length; ++k)
        {
            char findValue = buffer_read_pos(buffer, findPos);
            if(str[k] != findValue)
                break;

            if(k == length-1)
                return pos;

            findPos = buffer_next_pos(findPos);
        }

        pos = buffer_next_pos(pos);
    }
    return -1;
}

int buffer_next_pos(int pos)
{
    int nextPos = pos + 1;
    if(nextPos >= BUFFER_SIZE)
        nextPos = 0;
    return nextPos;
}
