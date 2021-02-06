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

#ifndef MESSAGETRANSLATIONSENTER_H
#define MESSAGETRANSLATIONSENTER_H

#include "message.h"

/** Debug function, can be used for debug over usart.
**/
typedef void (*debugFunc)(char*);


typedef void (*funcB)(char*, bool);
typedef void (*funcI)(char*, int);
typedef void (*funcF)(char*, float);

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
		void setDebugFunc(debugFunc aFunc);


		void setCreateTagsFunc(createTagsFunc aFunc);
		void setDeviceNameFunc(deviceNameFunc aFunc);

	private:
		int translateKeyValuePair(char* aMsg, int aStartPos);

		bool translateBool(char aBool);
		int translateInt(char *aInt);
		float translateFloat(char *aFloat);
		short translateShort(char *aShort);

	private:
		funcB mCallbackBool;
		funcI mCallbackInt;
		funcF mCallbackFloat;
		debugFunc mDebug; ///< callback function for debuging.

		createTagsFunc mCreateTagsFunc;
		deviceNameFunc mDeviceNameFunc;
};


#endif

