#ifndef MESSAGETRANSLATIONSENTER_H
#define MESSAGETRANSLATIONSENTER_H

#include "message.h"


typedef void (*funcB)(unsigned char*, bool);
typedef void (*funcI)(unsigned char*, int);
typedef void (*funcF)(unsigned char*, float);

/** \brief setup tags this device is listen at.
	The input tags.
	Called when a message recieved conains the key "createTags", any type.
**/
typedef void (*createTagsFunc)();

/** \brief Transmit one message that contain the device name,
	and is used as base for tag names on remote server 
	Called when a message recived contains the key "deviceName", any type.
**/
typedef void (*deviceNameFunc)();


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

		void setCreateTagsFunc(createTagsFunc aFunc);
		void setDeviceNameFunc(deviceNameFunc aFunc);

	private:
		int translateKeyValuePair(unsigned char* aMsg, int aStartPos);

		bool translateBool(unsigned char *aBool);
		int translateInt(unsigned char *aInt);
		float translateFloat(unsigned char *aFloat);
		short translateShort(unsigned char *aShort);

	private:
		funcB mCallbackBool;
		funcI mCallbackInt;
		funcF mCallbackFloat;

		createTagsFunc mCreateTagsFunc;
		deviceNameFunc mDeviceNameFunc;
};


#endif

