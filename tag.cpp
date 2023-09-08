#include "tag.h"

#include <stdlib.h>

#include "message.h"

namespace tag
{

void createTag(const char *device, char *aName, bool aValue)
{
    setValue(device, aName, aValue);
}


void createTag(const char *device, char *aName, int aValue)
{
    setValue(device, aName, aValue);
}


void createTag(const char *device, char *aName, float aValue)
{
    setValue(device, aName, aValue);
}


void createTag(const char *device, char *aName, char *aValue)
{
    setValue(device, aName, aValue);
}


void setValue(const char *device, char *tagName, bool value)
{
    message::reset_message(&tagMessageBuffer , device);
    message::add(&tagMessageBuffer, tagName, value);
    message::finnish_message(&tagMessageBuffer);
}


void setValue(const char *device, char *tagName, int value)
{
    message::reset_message(&tagMessageBuffer , device);
    message::add(&tagMessageBuffer, tagName, value);
    message::finnish_message(&tagMessageBuffer);
}


void setValue(const char *device, char *tagName, float value)
{
    message::reset_message(&tagMessageBuffer , device);
    message::add(&tagMessageBuffer, tagName, value);
    message::finnish_message(&tagMessageBuffer);
}


void setValue(const char *device, char *tagName, char *value)
{
    message::reset_message(&tagMessageBuffer , device);
    message::add(&tagMessageBuffer, tagName, value);
    message::finnish_message(&tagMessageBuffer);
}

}
