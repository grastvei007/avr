//
// Created by steini on 1/30/21.
//

#ifndef AVR_CIRCULARBUFFER_H
#define AVR_CIRCULARBUFFER_H

#define BUFFER_SIZE 128

struct CircularBuffer {
    int writePosition;
    int readPosition;
    char buffer[BUFFER_SIZE];

};

CircularBuffer buffer_create();
void buffer_write(CircularBuffer *buffer, char c);
char buffer_read(CircularBuffer *buffer);
bool buffer_empty(CircularBuffer *buffer);


#endif //AVR_CIRCULARBUFFER_H
