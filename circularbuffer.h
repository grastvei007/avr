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
void buffer_write_pos(CircularBuffer *buffer, int pos, char c);
char buffer_read(CircularBuffer *buffer);
char buffer_read_pos(CircularBuffer *buffer, int pos);
bool buffer_empty(CircularBuffer *buffer);
void buffer_erase(CircularBuffer *buffer, int startPos, int nElements);
int buffer_find(CircularBuffer *buffer, char *str, int startPos);
int buffer_next_pos(int pos);

#endif //AVR_CIRCULARBUFFER_H
