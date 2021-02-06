#include "tag.h"

#include <stdlib.h>

#include "message.h"
#include "usart.h"

namespace Tag
{

void createTag(char *aName, bool aValue)
{
	setValue(aName, aValue);
}


void createTag(char *aName, int aValue)
{
	setValue(aName, aValue);
}


void createTag(char *aName, float aValue)
{
	setValue(aName, aValue);
}


void createTag(char *aName, char *aValue)
{
	setValue(aName, aValue);
}


void setValue(char *aTagName, bool aValue)
{
    Message msg;
    msg.init();
    msg.add(aTagName, aValue);
    msg.finnish();
    int size = msg.getSize() + 1;

    //USART_putMessage(msg.getMessagePtr(), size);
    USART_buffer_append(msg.getMessagePtr(), size);
    msg.destroy();
}


void setValue(char *aTagName, int aValue)
{
    Message msg;
    msg.init();
    msg.add(aTagName, aValue);
    msg.finnish();
    int size = msg.getSize() + 1;

    //USART_putMessage(msg.getMessagePtr(), size);
    USART_buffer_append(msg.getMessagePtr(), size);
    msg.destroy();
}


void setValue(char *aTagName, float aValue)
{
    Message msg;
    msg.init();
    msg.add(aTagName, aValue);
    msg.finnish();
    int size = msg.getSize() + 1;

    //USART_putMessage(msg.getMessagePtr(), size);
    USART_buffer_append(msg.getMessagePtr(), size);
    msg.destroy();
}


void setValue(char *aTagName, char *aValue)
{
    Message msg;
    msg.init();
    msg.add(aTagName, aValue);
    msg.finnish();
    int size = msg.getSize() + 1;

    //USART_putMessage(msg.getMessagePtr(), size);
    USART_buffer_append(msg.getMessagePtr(), size);
    msg.destroy();
}

}
