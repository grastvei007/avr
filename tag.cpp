#include "tag.h"

#include <stdlib.h>

#include "message.h"
#include "usart.h"

namespace Tag
{

void createTag(const char *aName, bool aValue)
{
	setValue(aName, aValue);
}


void createTag(const char *aName, int aValue)
{
	setValue(aName, aValue);
}


void createTag(const char *aName, float aValue)
{
	setValue(aName, aValue);
}


void createTag(const char *aName,const  char *aValue)
{
	setValue(aName, aValue);
}


void setValue(const char *aTagName, bool aValue)
{
    Message msg;
    msg.init();
    msg.add(aTagName, aValue);
    msg.finnish();
    int size = msg.getSize() + 1;

    USART_putMessage(msg.getMessagePtr(), size);
    msg.destroy();
}


void setValue(const char *aTagName, int aValue)
{
    Message msg;
    msg.init();
    msg.add(aTagName, aValue);
    msg.finnish();
    int size = msg.getSize() + 1;

    USART_putMessage(msg.getMessagePtr(), size);
    msg.destroy();
}


void setValue(const char *aTagName, float aValue)
{
    Message msg;
    msg.init();
    msg.add(aTagName, aValue);
    msg.finnish();
    int size = msg.getSize() + 1;

    USART_putMessage(msg.getMessagePtr(), size);
    msg.destroy();
}


void setValue(const char *aTagName, const char *aValue)
{
    Message msg;
    msg.init();
    msg.add(aTagName, aValue);
    msg.finnish();
    int size = msg.getSize() + 1;

    USART_putMessage(msg.getMessagePtr(), size);
    msg.destroy();
}

}
