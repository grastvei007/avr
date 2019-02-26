#include "messagehandler.h"
#include "usart.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

MessageHandler::MessageHandler(size_t aBufferSize)
{
	init(aBufferSize);
}


MessageHandler::~MessageHandler()
{
	free(mBuffer);
}


void MessageHandler::init(size_t aBufferSize)
{
	mBufferStart = 0;
	mBufferEnd = 0;
	mBufferSize = aBufferSize;
    mBuffer = (char*)malloc(aBufferSize);
    mCallback = NULL;
    for(size_t i=0; i<mBufferSize; ++i)
    {
        mBuffer[i] = '0';
    }

}

void MessageHandler::setCallback(funcPtr func)
{
	mCallback = func;	
}


void MessageHandler::insertChar(char c)
{
	
	mBuffer[mBufferEnd++] = c;
	if(mBufferEnd >= mBufferSize)
 		mBufferEnd = 0;


	if(mBufferEnd == mBufferStart)
		mBufferStart++;

	if(mBufferStart >= mBufferSize)
		mBufferStart = 0;

/*
	int startPos = find("<msg");
	if( startPos < 0)
		return;

	Message *msg;
	getMessage(msg, startPos);
	if(!msg)
		return;

	if(mCallback)
		mCallback(msg);
	else
		free(msg);*/
}


void MessageHandler::run()
{
    int startPos = find("<msg");
    if( startPos < 0)
        return;

    Message *msg;
    getMessage(msg, startPos);
    if(!msg)
        return;

    if(mCallback)
        mCallback(msg);
    else
        free(msg);
}


bool MessageHandler::hasMessage() const
{


}


size_t MessageHandler::bufferSize() const
{
	return mBufferSize;
}


size_t MessageHandler::size() const
{
	if(mBufferStart < mBufferEnd)
	{
		return mBufferEnd - mBufferStart;
	}
	else if(mBufferStart > mBufferEnd)
	{
		return (mBufferSize-mBufferStart)+mBufferEnd;
	}
	else
		return 0;
	
}


void MessageHandler::printBuffer() const
{
#ifndef AVR_TEST
	USART_putMessage(mBuffer, mBufferSize);
#else
	printf("MessageHandler buffer\n");
        for(size_t i=0; i<bufferSize(); ++i)
	{
		printf("%c", mBuffer[i]);
	}
	printf("\n");
#endif
}


void MessageHandler::getBuffer(char *aBuffer)
{
	for(unsigned int i=0; i<mBufferSize; ++i)
	{
		aBuffer[i] = mBuffer[i];
	}
}


char MessageHandler::getChar(int aIdx)
{
	int pos = (mBufferStart + aIdx) % mBufferSize;
	return mBuffer[pos];
}


int MessageHandler::find(char *aStr)
{
	int len = strlen(aStr);
	if(size() < len)
		return -1;

	if(len == 0)
		return -1;

	for(unsigned int i=0; i<bufferSize(); ++i)
	{
		if(mBuffer[i] == aStr[0])
		{
			bool found = true;
			for(int k = 1; k<len; ++k)
			{	
				if(mBuffer[i+k] != aStr[k])
				{
					found = false;
				}
			}
			if(found)
				return i;
		}
	}

	return -1;

// old
/*	int len = strlen(aStr);
	if(size() < len)
		return -1;

	if(len == 0)
		return -1;
	int k = 0;
	for(int i=0; i<size()+1; i++)
	{
		int pos = (i + mBufferStart) % mBufferSize;
		if(mBuffer[pos] == aStr[0])
		{
			int p = (pos+1);;
			for(int j=1; j<len; j++)
			{			
				if(p > mBufferSize-1)
					p = 0;
					
				if(mBuffer[p] == aStr[j])
				{
					k++;
					p++;
				}
				else
				{
					k=0;
					break;
				}
			}
			if(k != 0)
			{
				int posInArray = i;
				return (mBufferStart + posInArray) % mBufferSize;
			}
		}
	}
	return -1;*/
}


int MessageHandler::getMessage(Message *& rMessage, int aStartPosInBuffer)
{
	rMessage = NULL;


	char sizeBuffer[4];
	sizeBuffer[0] = mBuffer[(aStartPosInBuffer+4)%mBufferSize];
	sizeBuffer[1] = mBuffer[(aStartPosInBuffer+5)%mBufferSize];
	sizeBuffer[2] = mBuffer[(aStartPosInBuffer+6)%mBufferSize];
	sizeBuffer[3] = mBuffer[(aStartPosInBuffer+7)%mBufferSize];

	int size = atoi(sizeBuffer);
	if(size  <= 10)
		return -1;
	
    char msg[size];
	for(int i = 0; i<size; ++i)
	{
		msg[i] = mBuffer[(aStartPosInBuffer+i)%mBufferSize];
	}

	rMessage = (Message*)malloc(sizeof(Message));
	rMessage->init();
    rMessage->setMessage(msg, size);

	if(rMessage->isValid() != 1)
	{
		rMessage->destroy();
		free(rMessage);
		rMessage = NULL;
	}
	else
	{	
		for(int i=0; i<size; ++i)
		{
			int pos = (aStartPosInBuffer+i) % mBufferSize;
			mBuffer[pos] = '0'; // overwrite with something
		}
	}
    //free(msg);
}

