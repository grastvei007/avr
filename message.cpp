#include "message.h"

#include <string.h>
#include <stdlib.h>

Message::Message() :
    mIsValid(true)
{
    mMessageSize = 8;
    mMessage = (unsigned char*)malloc(mMessageSize); // new unsigned char[mMessageSize];
    mMessage[0] = '<';
    mMessage[1] = 'm';
    mMessage[2] = 's';
    mMessage[3] = 'g';
    mMessage[4] = '0';
    mMessage[5] = '0';
    mMessage[6] = '0';
    mMessage[7] = '0'; // with place for 4 byte int value in plain text.

}


Message::Message(unsigned char *aMsg)
{
    mIsValid = validateMessage(aMsg);
}


int Message::isValid()
{
    return mIsValid;
}

void Message::add(char* aKey, double aValue)
{

}


void Message::add(char *aKey, float aValue)
{
    size_t keySize = strlen(aKey);
    char *pair = (char*) malloc(keySize+6);

    int t=0;
    for(int i=0; i<keySize; ++i)
    {
        pair[i] = aKey[i];
        t++;
    }
    pair[t++] = ':';
    pair[t++] = 'f'; // mark value as float.

    unsigned char bytes[4];
    memcpy(bytes, (unsigned char*) &aValue, 4);
    pair[t++] = bytes[0];
    pair[t++] = bytes[1];
    pair[t++] = bytes[2];
    pair[t++] = bytes[3];

    unsigned char *temp = (unsigned char*) malloc(mMessageSize+keySize+6);
    memcpy(temp, mMessage, mMessageSize);
    int k=0;
    for(int i=mMessageSize; i<(mMessageSize+keySize+6); ++i)
        temp[i] = pair[k++];
    mMessageSize += (keySize+6);
    free(mMessage);
	free(pair);
    mMessage = temp;
    temp = NULL;
	pair = NULL;
}


void Message::add(char* aKey, int aValue)
{
	size_t keySize = strlen(aKey);
	unsigned char* pair = (unsigned char*)malloc(keySize+6);
	memcpy(pair, aKey, keySize);

	pair[keySize+1] = ':';
	pair[keySize+2] = 'i';

	unsigned char bytes[4];
	memcpy(bytes, (unsigned char*)&aValue, 4);

	pair[keySize+3] = bytes[0];
	pair[keySize+4] = bytes[1];
	pair[keySize+5] = bytes[2];
	pair[keySize+6] = bytes[3];

	unsigned char* temp = (unsigned char*)malloc(mMessageSize+keySize+6);
	memcpy(temp+(mMessageSize+1), pair, keySize+6);
	
	free(mMessage);
	free(pair);
	mMessage = temp;
	temp = NULL;		
	pair = NULL;
}

/**
	the value is encoded to bX where x is 0 or 1
**/
void Message::add(char* aKey, bool aValue)
{
	size_t keySize = strlen(aKey);
	unsigned char* pair = (unsigned char*)malloc(keySize+3);
	memcpy(pair, aKey, keySize);

	pair[keySize+1] = ':';
	pair[keySize+2] = 'b';
	pair[keySize+3] = (aValue) ? '1' : '0';

	unsigned char* temp = (unsigned char*)malloc(mMessageSize+keySize+3);
	memcpy(temp, mMessage, mMessageSize);
	memcpy(temp+(mMessageSize+1), pair, keySize+3);

	free(mMessage);
	free(pair);
	mMessage = temp;
	temp = NULL;
	pair = NULL;
}


/** \brief Add key value to message,

	the value is marked with prefix cXX, wher XX is binary short that
	tells the size of the value text.

**/
void Message::add(char* aKey, char *aValue)
{
    size_t keySize = strlen(aKey);
    size_t valueSize = strlen(aValue);

    char *pair = (char*)malloc(keySize+valueSize+4);
    memcpy(pair, aKey, keySize);
    pair[keySize+1] = ':';
	pair[keySize+2] = 'c';
	short size = (short) valueSize;
	unsigned char bufferSize[2];
	memcpy(bufferSize, (unsigned char*)&size, 2);
	pair[keySize+3] = bufferSize[0];
	pair[keySize+4] = bufferSize[1];

	memcpy(pair+keySize+4, aValue, valueSize);

	unsigned char* temp = (unsigned char*)malloc(mMessageSize+keySize+valueSize+4);
	memcpy(temp, mMessage, mMessageSize);
	memcpy(temp+mMessageSize+1, (unsigned char*)pair, keySize+valueSize+4);
	free(mMessage);
	free(pair);
	pair = NULL;
	mMessage = NULL;
	mMessage = temp;
	mMessageSize += (keySize+valueSize+4); 
}

void Message::finnish()
{
    unsigned char *temp = (unsigned char*) malloc(mMessageSize+2);
    memcpy(temp, mMessage, mMessageSize);
    temp[mMessageSize] = '0';
    temp[mMessageSize+1] = '>';
    free(mMessage);
    mMessage = temp;
    temp = NULL;
    mMessageSize += 2;

    char buffer[4] = {0};
    sprintf(buffer, "%i", (int)mMessageSize);
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


int Message::getMessage(unsigned char *&rMessage)
{
    rMessage = (unsigned char*) malloc(mMessageSize);
    memcpy(rMessage, mMessage, mMessageSize);
    return mMessageSize;
}


void Message::calcCheckcode()
{
    int n = 0;
    for(int i=0; i<mMessageSize; ++i)
    {
        if(i != mMessageSize -2)
            n += (int)mMessage[i];
    }

    mMessage[mMessageSize-2] = (char)(n % 256);
}


int Message::validateMessage(const unsigned char *aMsg)
{
    int size = strlen((const char*)aMsg);
    if(size < 8)
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
    for(int i=0; i<msgSize; ++i)
    {
        if(i != msgSize-2)
            n += aMsg[i];
    }

    int r = n % 256;
    if( r != aMsg[msgSize-2])
        return -3;

    mMessageSize = msgSize;
    mMessage = (unsigned char*) malloc(msgSize);
    memcpy(mMessage, aMsg, msgSize);
    return 1;
}
