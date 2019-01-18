#include "messagetranslationsenter.h"

#include <string.h>
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


void MessageTranslationSenter::translateMessage(Message *aMsg)
{
	if(aMsg == NULL)
	{
		fprintf(stderr, "No msg");
		return;
	}

	unsigned char *msg = NULL;
	int size = aMsg->getMessage(msg);
	fprintf(stderr, "s", msg);
	if(!msg || size < 0)
		return;
	
	int pos = Message::HeaderSize+4;
	for(;;)
	{
		if( (size-pos) <= 2)
			break;
		fprintf(stderr, "translatepair\n");
		pos += translateKeyValuePair(msg, pos);
	}
		
}

int MessageTranslationSenter::translateKeyValuePair(unsigned char *aMsg, int aStartPos)
{
	fprintf(stderr, "startpos, %i", aStartPos);
	unsigned char keyBuffer[Message::MaxKeySize] = {'\0'};
	int steps = 0;
	int startPosValue = 0;

	for(int i=aStartPos; ; i++)
	{
	//	fprintf(stderr, "%c%", aMsg[i]);
		if(aMsg[i] == ':')
		{
			steps++;
			break;
		}
		keyBuffer[steps++] = aMsg[i];
	};
	fprintf(stderr, "key %s", keyBuffer);
	unsigned char type = aMsg[aStartPos+steps];
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
		bool b = translateBool(aMsg + (aStartPos+steps));
		steps++;
		if(mCallbackBool && !cmd)
			mCallbackBool(keyBuffer, b);
	}
	else if(type == 'i')
	{
		int i = translateInt(aMsg+(aStartPos+steps));
		steps += 4;
		if(mCallbackInt && !cmd)
			mCallbackInt(keyBuffer, i);
	}
	else if(type == 'f')
	{
		float f = translateFloat(aMsg+(aStartPos+steps));
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


bool MessageTranslationSenter::translateBool(unsigned char *aBool)
{
	return (*aBool == '1') ? true : false;
}


int MessageTranslationSenter::translateInt(unsigned char *aInt)
{
	int r = aInt[0] + (aInt[1] << 8) + (aInt[2] << 16) + (aInt[3] << 24);
	return r;
}


float MessageTranslationSenter::translateFloat(unsigned char *aFloat)
{
	float f = aFloat[0] + (aFloat[1] << 8) + (aFloat[2] << 16) + (aFloat[3] << 24);
	return f;
}


short MessageTranslationSenter::translateShort(unsigned char *aShort)
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
