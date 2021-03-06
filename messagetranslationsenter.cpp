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

#include "messagetranslationsenter.h"

#include <string.h>
#include <stdlib.h>
//#include <stdio.h>

MessageTranslationSenter::MessageTranslationSenter()
{
	init();
}


void MessageTranslationSenter::init()
{
	mCallbackBool = NULL;
	mCallbackInt = NULL;
	mCallbackFloat = NULL;
	mDeviceNameFunc = NULL;
	mCreateTagsFunc = NULL;
	mDebug = NULL;
}


void MessageTranslationSenter::setCallbackBoolValue(funcB aFunc)
{
	mCallbackBool = aFunc;
}


void MessageTranslationSenter::setCallbackIntValue(funcI aFunc)
{
	mCallbackInt = aFunc;
}


void MessageTranslationSenter::setCallbackFloatValue(funcF aFunc)
{
	mCallbackFloat = aFunc;
}


void MessageTranslationSenter::setDebugFunc(debugFunc aFunc)
{
	mDebug = aFunc;
}


void MessageTranslationSenter::translateMessage(Message *aMsg)
{
	if(aMsg == NULL)
	{
		return;
	}

	char msg[100];
	int size = aMsg->getSize() + 1;
	aMsg->getMessageData(msg);
	if(size < 0)
		return;
	
	int pos = Message::HeaderSize+4;
//	for(;;)
//	{
//		if( (size-pos) <= 2)
//			break;
		pos += translateKeyValuePair(msg, pos);
//	}
}

int MessageTranslationSenter::translateKeyValuePair(char *aMsg, int aStartPos)
{
	char keyBuffer[Message::MaxKeySize] = {'\0'};
	int steps = 0;

	for(int i=aStartPos; i<aStartPos+Message::MaxKeySize ; i++)
	{
	//	fprintf(stderr, "%c%", aMsg[i]);
		if(aMsg[i] == ':')
		{
			steps++;
			break;
		}
		keyBuffer[steps++] = aMsg[i];
	};
	//fprintf(stderr, "key %s", keyBuffer);
	char type = aMsg[aStartPos+steps];
	steps++;


	bool cmd = false;
	if(strcmp((char*)keyBuffer, "deviceName") == 0)
	{
		cmd = true;
		if(mDeviceNameFunc)
			mDeviceNameFunc();
	}
	if(strcmp((char*)keyBuffer, "createTags") == 0)
	{
		cmd = true;
		if(mCreateTagsFunc)
			mCreateTagsFunc();
	}

	if(type == 'b')
	{
		bool b = translateBool(aMsg[aStartPos + steps]);
		steps++;
		if(mCallbackBool && !cmd)
			mCallbackBool(keyBuffer, b);
	}
	else if(type == 'i')
	{
		int i = translateInt(aMsg+(aStartPos + steps));
		steps += 4;
		if(mCallbackInt && !cmd)
			mCallbackInt(keyBuffer, i);
	}
	else if(type == 'f')
	{
		float f = translateFloat(aMsg+(aStartPos + steps));
		steps += 4;
		if(mCallbackFloat && !cmd)
			mCallbackFloat(keyBuffer, f);
	}
	else if(type == 'c')
	{
		// skip string.
		short s = translateShort(aMsg+(aStartPos+steps));
		steps += (s + 2);
	}	
	
	return steps;	
}


bool MessageTranslationSenter::translateBool(char aBool)
{
	return ((uint8_t) aBool == 1);
}


int MessageTranslationSenter::translateInt(char *aInt)
{
	int r = aInt[0] + (aInt[1] << 8) + (aInt[2] << 16) + (aInt[3] << 24);
	return r;
}


float MessageTranslationSenter::translateFloat(char *aFloat)
{
	union U
	{
		float f;
		char bytes[4];
	}u;
	u.bytes[0] = aFloat[0];
	u.bytes[1] = aFloat[1];
	u.bytes[2] = aFloat[2];
	u.bytes[3] = aFloat[3];
	return u.f;
}


short MessageTranslationSenter::translateShort(char *aShort)
{
	short s = aShort[0] + (aShort[1] << 8);
	return s;
}

void MessageTranslationSenter::setCreateTagsFunc(createTagsFunc aFunc)
{
	mCreateTagsFunc = aFunc;
}


void MessageTranslationSenter::setDeviceNameFunc(deviceNameFunc aFunc)
{
	mDeviceNameFunc = aFunc;
}
