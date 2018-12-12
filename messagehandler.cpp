#include "messagehandler.h"

#include <stdlib.h>
#include <cstdio>

MessageHandler::MessageHandler(size_t aBufferSize) :
	mBufferStart(0),
	mBufferEnd(0),
	mBufferSize(aBufferSize)
{
	mBuffer = (unsigned char*)malloc(aBufferSize);
	mCallback = NULL;
	for(int i=0; i<mBufferSize; ++i)
	{
		mBuffer[i] = '0';
	}
}


MessageHandler::~MessageHandler()
{
	free(mBuffer);
}


void MessageHandler::setCallback(funcPtr func)
{
	mCallback = func;	
}


void MessageHandler::insertChar(unsigned char c)
{
	
	mBuffer[mBufferEnd++] = c;
	if(mBufferEnd >= mBufferSize)
 		mBufferEnd = 0;


	if(mBufferEnd >= mBufferStart)
		mBufferStart++;

	if(mBufferStart >= mBufferSize)
		mBufferStart = 0;
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
	for(int i=0; i<bufferSize(); ++i)
	{
		printf("%c", mBuffer[i]);
	}
}
