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
}


void MessageHandler::run()
{
    int startPos = find("<msg");
    if( startPos < 0)
        return;
	//USART_putstring(" msg found ");
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

/** \brief Get the char from the buffer.

	searches the buffer from begining to end.

**/
char MessageHandler::getChar(int aIdx)
{
	int pos = (mBufferEnd + 1) % mBufferSize;
	return mBuffer[pos];
}


int MessageHandler::find(char *aStr)
{
	int len = strlen(aStr);
	if(len <= 0)
		return -1;

	int startPos = (mBufferEnd + 1) % bufferSize();
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

