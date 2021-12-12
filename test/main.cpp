#include "../circularbuffer.h"

#include <iostream>
#include <string>


bool circularbufferTests();
void printBuffer(CircularBuffer *buffer);


int main(int argc, char *argv[])
{

    if(!circularbufferTests())
        std::cerr << "circular buffer tests failed" << std::endl;
    else
        std::cerr << "circular buffer tests passed" << std::endl;

	return 0;
}


bool circularbufferTests()
{
    CircularBuffer buffer = buffer_create();
//    printBuffer(&buffer);

    int pos = 0;
    for(int i=0; i<BUFFER_SIZE+10; ++i)
    {
        std::cerr << pos << " ";
        pos = buffer_next_pos(pos);
    }
    std::cerr << std::endl;

    for(int i=0; i<29; ++i)
        buffer_write(&buffer, char(i));

    buffer_write(&buffer, '<');
    buffer_write(&buffer, 'm');
    buffer_write(&buffer, 's');
    buffer_write(&buffer, 'g');

    int found = buffer_find(&buffer, "<msg", 1);
    if(found)
        std::cerr << "found: " << found << std::endl;
    else
        std::cerr << "not found" << std::endl;

    std::cerr << "find pos: " << (int)buffer_find(&buffer, "<msg", 100) << std::endl;
    std::cerr << "find pos: " << (int)buffer_find(&buffer, "<msg", 31) << std::endl;

    buffer_erase(&buffer, found, 4);

//   printBuffer(&buffer);

    return true;
}


void printBuffer(CircularBuffer *buffer)
{
    for(int i=0; i<BUFFER_SIZE; ++i)
    {
        std::cerr << "[" << i << "," << (int)buffer_read_pos(buffer, i) << "] "; 
    }
    std::cerr << std::endl;
}
