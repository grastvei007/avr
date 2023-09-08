#ifndef TAG_H
#define TAG_H

#include "message.h"

namespace tag
{
    Message tagMessageBuffer;


	/** Create tags on the June server for values,
		that are input to the atmega. It means values sent
		to it over serial link.
	**/
    void createTag(const char *device, char *aName, bool aValue);
    void createTag(const char *device, char *aName, int aValue);
    void createTag(const char *device, char *aName, float aValue);
    void createTag(const char *device, char *aName, char *aValue);

	/** Set value on tags

	**/
    void setValue(const char *device, char *aTagName, bool aValue);
    void setValue(const char *device, char *aTagName, int aValue);
    void setValue(const char *device, char *aTagName, float aValue);
    void setValue(const char *device, char *aTagName, char *aValue);
};


#endif
