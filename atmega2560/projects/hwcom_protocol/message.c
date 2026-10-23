#include "message.h"


void calculateCheckSum(uint8_t *msg, size_t size)
{
    int sum = 0;
    for(unsigned i = 0; i < size; ++i)
    {
        if(i != size-2)
            sum += msg[i];
    }
    int r = sum % 255;
    msg[size-2] = r;
}
