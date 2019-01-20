#include "message.h"

#include <string.h>
#include <stdlib.h>

Message::Message()
{
	init();
}


Message::Message(char *aMsg, size_t aSize)
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

void Message::add(char* aKey, double aValue)
{

}


void Message::add(char *aKey, float aValue)
{
/*    size_t keySize = strlen(aKey);
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
	pair = NULL;*/
}


void Message::add(char* aKey, int aValue)
{
/*	size_t keySize = strlen(aKey);
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
	pair = NULL;*/
}

/**
	the value is encoded to bX where x is 0 or 1
**/
void Message::add(char* aKey, bool aValue)
{
/*	size_t keySize = strlen(aKey);
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
	pair = NULL;*/
}


/** \brief Add key value to message,

	the value is marked with prefix cXX, wher XX is binary short that
	tells the size of the value text.

**/
void Message::add(char* aKey, char *aValue)
{
    size_t keySize = strlen(aKey);
    size_t valueSize = strlen(aValue);
	
	for(int i=0; i<keySize; ++i)
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

	for(int i=0; i<valueSize; ++i)
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


void Message::setMessage(char *&aMessage, size_t aMessageSize)
{
	mIsValid = validateMessage(aMessage, aMessageSize);
}


void Message::calcCheckcode()
{
    printf("calc check code\n");
    int n = 0;
    int r = 0;
    for(int i=0; i<mMessageSize+1; ++i)
    {
        if(i == mMessageSize -1)
            continue;

        n += (int)mMessage[i];
        printf("c(%i) %c\n", mMessage[i], mMessage[i]);
    }
    printf("\ndone\n");
    r = n % 255;
    printf("sum: %i, r=%i\n", n,r);
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
    for(int i=0; i<msgSize; ++i)
    {
        if(i != msgSize-2)
            n += aMsg[i];
    }

    int r = n % 256;
    if( r != aMsg[msgSize-2])
        return -3;

    mMessageSize = msgSize;
	for(int i=0; i<msgSize; ++i)
		mMessage[i] = aMsg[i];
    return 1;
}


void Message::print()
{
	printf("\nMessage size: %i\n", mMessageSize);
	for(int i=0; i<mMessageSize; ++i)
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
    for(int i=0; i<mMessageSize+1; ++i)
        msg[i] = mMessage[i];
}
