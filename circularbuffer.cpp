//
// Created by steini on 1/30/21.
//

#include "circularbuffer.h"


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
    buffer->writePosition++;
    if(buffer->writePosition >= BUFFER_SIZE)
        buffer->writePosition = 0;
}

char buffer_read(CircularBuffer *buffer)
{
    char value = buffer->buffer[buffer->readPosition];
    buffer->readPosition++;
    if(buffer->readPosition >= BUFFER_SIZE)
        buffer->readPosition = 0;
    return value;
}

bool buffer_empty(CircularBuffer *buffer)
{
    return (buffer->readPosition == buffer->writePosition);
}