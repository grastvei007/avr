/*This file is part of June.

June is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <https://www.gnu.org/licenses/>.*/

#include "message.h"

#include <string.h>
#include <stdlib.h>

Message::Message()
{
	init();
}


Message::Message(const char *aMsg, size_t aSize)
{
    mIsValid = validateMessage(aMsg, aSize);
}


void Message::init()
{
    mIsValid = true;
    mMessageSize = 7;
    mMessage[0] = '<';
    mMessage[1] = 'm';
    mMessage[2] = 's';
    mMessage[3] = 'g';
    mMessage[4] = '0';
    mMessage[5] = '0';
    mMessage[6] = '0'; 
    mMessage[7] = '0'; // with place for 4 byte int value in plain text.
    for(int i=8; i<100; ++i)
        mMessage[i] = '0';

}


void Message::destroy()
{

}


int Message::isValid()
{
    return mIsValid;
}

void Message::add(const char* aKey, double aValue)
{

}


void Message::add(const char *aKey, float aValue)
{
    size_t keySize = strlen(aKey);
    for(unsigned int i=0; i<keySize; ++i)
        mMessage[++mMessageSize] = aKey[i];

    mMessage[++mMessageSize] = ':';
    mMessage[++mMessageSize] = 'f';

    union U
    {
        float f;
        char bytes[4];
    }u;
    u.f = aValue;
    mMessage[++mMessageSize] = u.bytes[0];
    mMessage[++mMessageSize] = u.bytes[1];
    mMessage[++mMessageSize] = u.bytes[2];
    mMessage[++mMessageSize] = u.bytes[3];
}


void Message::add(const char* aKey, int aValue)
{
    size_t keySize = strlen(aKey);
    for(unsigned int i=0; i<keySize; ++i)
        mMessage[++mMessageSize] = aKey[i];

    mMessage[++mMessageSize] = ':';
    mMessage[++mMessageSize] = 'i';

    union U
    {
        int i;
        char bytes[4];
    }u;
    u.i = aValue;
    mMessage[++mMessageSize] = u.bytes[0];
    mMessage[++mMessageSize] = u.bytes[1];
    mMessage[++mMessageSize] = u.bytes[2];
    mMessage[++mMessageSize] = u.bytes[3];
}

/**
	the value is encoded to bX where x is 0 or 1
**/
void Message::add(const char* aKey, bool aValue)
{
    size_t keySize = strlen(aKey);

    for(unsigned int i=0; i<keySize; ++i)
        mMessage[++mMessageSize] = aKey[i];

    mMessage[++mMessageSize] = ':';
    mMessage[++mMessageSize] = 'b';
    mMessage[++mMessageSize] = (aValue) ? char(1) : char(0);

}


/** \brief Add key value to message,

	the value is marked with prefix cXX, wher XX is binary short that
	tells the size of the value text.

**/
void Message::add(const char* aKey, const char *aValue)
{
    size_t keySize = strlen(aKey);
    size_t valueSize = strlen(aValue);
	
	for(unsigned int i=0; i<keySize; ++i)
		mMessage[++mMessageSize] = aKey[i];

	mMessage[++mMessageSize] = ':';
	mMessage[++mMessageSize] = 'c';

	union shortBytes
	{
		unsigned short asShort;
		unsigned char asByte[2];
	}sb;
	sb.asShort = (short) valueSize; // n chars in value string.

	mMessage[++mMessageSize] = sb.asByte[0];
	mMessage[++mMessageSize] = sb.asByte[1];

	for(unsigned int i=0; i<valueSize; ++i)
		mMessage[++mMessageSize] = aValue[i];
}

void Message::finnish()
{
	mMessage[++mMessageSize] = '0';
	mMessage[++mMessageSize] = '>';

    char buffer[4] = {0};
    sprintf(buffer, "%i", (int)mMessageSize+1);
    int t = strlen(buffer);
    if(t == 1)
         mMessage[7] = buffer[0];
    else if(t == 2)
    {
        mMessage[6] = buffer[0];
        mMessage[7] = buffer[1];
    }
    else if( t == 3)
    {
        mMessage[5] = buffer[0];
        mMessage[6] = buffer[1];
        mMessage[7] = buffer[2];
    }
    else if(t == 4)
    {
        for(int i=0; i<t; ++i)
            mMessage[5+i] = buffer[i];
    }
    calcCheckcode();
}


int Message::getMessage(char *&rMessage)
{
    rMessage = (char*) malloc(mMessageSize);
    for(int i=0; i<mMessageSize; ++i)
        rMessage[i] = mMessage[i];
    //memcpy(rMessage, mMessage, mMessageSize);
    return mMessageSize;
}


char* Message::getMessagePtr()
{
	return mMessage;
}

void Message::setMessage(char *aMessage, size_t aMessageSize)
{
	mIsValid = validateMessage(aMessage, aMessageSize);
}


void Message::calcCheckcode()
{
    int n = 0;
    int r = 0;
    for(unsigned int i=0; i<mMessageSize+1; ++i)
    {
        if(i == mMessageSize -1)
            continue;

        n += (int)mMessage[i];
    }
    r = n % 255;
    mMessage[mMessageSize-1] = (char)r;
}


int Message::validateMessage(const char *aMsg, size_t aSize)
{
//	size_t size = strlen((const char*)aMsg);
    if(aSize < 8)
        return -1;
    if(aMsg[0] != '<' || aMsg[1] != 'm' || aMsg[2] != 's' || aMsg[3] != 'g')
        return -2;

    char buffer[4] = {0};
    buffer[0] = aMsg[4];
    buffer[1] = aMsg[5];
    buffer[2] = aMsg[6];
    buffer[3] = aMsg[7];

    int msgSize = atoi(buffer);

    int n = 0;
    for(unsigned int i=0; i<msgSize; ++i)
    {
        if(i != msgSize-2)
            n += aMsg[i];
    }

    int r = n % 255;
    int t = aMsg[msgSize-2];
    while(t<0)
        t += 256;
    if( r != t)
        return -3;

    mMessageSize = msgSize;
	for(unsigned int i=0; i<msgSize; ++i)
		mMessage[i] = aMsg[i];
    return 1;
}


void Message::print()
{
	printf("\nMessage size: %i\n", mMessageSize);
	for(unsigned int i=0; i<mMessageSize; ++i)
	{
		printf("%c", mMessage[i]);
	}
	printf("\nMessage End\n");
}

int Message::getSize()
{
    return mMessageSize;
}

void Message::getMessageData(char *msg)
{
    for(unsigned int i=0; i<mMessageSize+1; ++i)
        msg[i] = mMessage[i];
}
