#ifndef MESSAGETRANSLATIONSENTER_H
#define MESSAGETRANSLATIONSENTER_H

#include "message.h"


typedef void (*funcB)(unsigned char*, bool);
typedef void (*funcI)(unsigned char*, int);
typedef void (*funcF)(unsigned char*, float);


/** \brief Translate the key-values in the message, and pass them to the callback
			functions if they are set.

**/
class MessageTranslationSenter
{
	public:
		MessageTranslationSenter();
		void init();

		void translateMessage(Message *aMsg);

		void setCallbackBoolValue(funcB aFunc);
		void setCallbackIntValue(funcI aFunc);
		void setCallbackFloatValue(funcF aFunc);

	private:
		int translateKeyValuePair(unsigned char* aMsg, int aStartPos);

		bool translateBool(unsigned char *aBool);
		int translateInt(unsigned char *aInt);
		float translateFloat(unsigned char *aFloat);

	private:
		funcB mCallbackBool;
		funcI mCallbackInt;
		funcF mCallbackFloat;
};


#endif

