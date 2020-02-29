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

MessageHandler::MessageHandler()
{
	init();
}


MessageHandler::~MessageHandler()
{
//	free(mBuffer);
}


void MessageHandler::init()
{
	mBufferEnd = BUFFER_SIZE - 1;
	mBufferSize = BUFFER_SIZE;
  //  mBuffer = (char*)malloc(aBufferSize);
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
	mBufferEnd++;
	if(mBufferEnd >= BUFFER_SIZE)
		mBufferEnd -= BUFFER_SIZE;
	
	mBuffer[mBufferEnd] = c;
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

    if(mCallback && msg)
        mCallback(msg);
    else
	{
		if(msg)
	        free(msg);
	}
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
//	USART_putMessage(mBuffer, mBufferSize);
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

	int startPos = mBufferEnd + 1;
	if(startPos >= BUFFER_SIZE)
		startPos = 0;

	for(unsigned int i=0; i<BUFFER_SIZE; ++i)
	{
		int index = startPos + i;
		if(index >= BUFFER_SIZE)
			index -= BUFFER_SIZE;

		if(mBuffer[index] == aStr[0])
		{
			bool found = true;
			for(int k=1; k<len; ++k)
			{
				int index2 = index + k;
				if(index2 >= BUFFER_SIZE)
					index2 -= BUFFER_SIZE;

				if(mBuffer[index2] != aStr[k])
				{
					found = false;
				}
			}
			if(found)
				return index;
		}
	}
	return -1;
}


int MessageHandler::getMessage(Message *& rMessage, int aStartPosInBuffer)
{
	rMessage = NULL;


	char sizeBuffer[4];
	sizeBuffer[0] = mBuffer[(aStartPosInBuffer+4)%BUFFER_SIZE];
	sizeBuffer[1] = mBuffer[(aStartPosInBuffer+5)%BUFFER_SIZE];
	sizeBuffer[2] = mBuffer[(aStartPosInBuffer+6)%BUFFER_SIZE];
	sizeBuffer[3] = mBuffer[(aStartPosInBuffer+7)%BUFFER_SIZE];

	int size = atoi(sizeBuffer);
	if(size  <= 10 || size >= BUFFER_SIZE)
		return -1;
	
    char msg[size];
	for(int i = 0; i<size; ++i)
	{
		msg[i] = mBuffer[(aStartPosInBuffer+i)%BUFFER_SIZE];
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
			int pos = (aStartPosInBuffer+i) % BUFFER_SIZE;
			mBuffer[pos] = '0'; // overwrite with something
		}
	}
    //free(msg);
}

