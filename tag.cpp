#include "tag.h"

#include <stdlib.h>

#include "message.h"
#include "usart.h"

namespace Tag
{

void createTag(char *aName, bool aValue)
{
    Message msg;
    msg.init();
    msg.add(aName, aValue);
    msg.finnish();
    int size = msg.getSize() + 1;
    char *data = (char*)malloc(size);
    msg.getMessageData(data);

    USART_putMessage(data, size);
    msg.destroy();
    free(data);
}


void createTag(char *aName, int aValue)
{
    Message msg;
    msg.init();
    msg.add(aName, aValue);
    msg.finnish();
    int size = msg.getSize() + 1;
    char *data = (char*)malloc(size);
    msg.getMessageData(data);
    
    USART_putMessage(data, size);
    msg.destroy();
    free(data);
}


void createTag(char *aName, float aValue)
{
    Message msg;
    msg.init();
    msg.add(aName, aValue);
    msg.finnish();
    int size = msg.getSize() + 1;
    char *data = (char*)malloc(size);
    msg.getMessageData(data);
    
    USART_putMessage(data, size);
    msg.destroy();
    free(data);
}


}
