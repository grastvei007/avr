#include "messagehandler.h"

#include <stdlib.h>
#include <cstdio>
#include <string.h>

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


	if(mBufferEnd == mBufferStart)
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
		printf("%c ", mBuffer[i]);
	}
}

unsigned char MessageHandler::getChar(unsigned int aIdx)
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

	int k = 0;
	for(int i=0; i<size(); i++)
	{
		int pos = (i + mBufferStart) % mBufferSize;
		if(mBuffer[pos] == aStr[0])
		{
			printf("%s\n", "first-c");
			int p = pos+1;
			for(int j=1; j<len; j++)
			{			
				printf("%c\n", mBuffer[p]);	
				if(p > mBufferSize)
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
				printf("%s %i\n", "found start,", mBufferStart);
				int posInArray = i;
				if(mCallback)
					mCallback(NULL);
				return (mBufferStart + posInArray) % mBufferSize;
			}
		}
	}
	return -1;
}
