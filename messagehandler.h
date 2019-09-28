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

#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "message.h"

#define BUFFER_SIZE 128 ///< default size of buffer



/** /brief class MessageHandler

	Handler uart rx, recieved chars are stored in a circular buffer,
	and automaticaly look for messages within the buffer.

	Set a callback function to get notifications every time a new message is detected.

*/

typedef void (*funcPtr)(Message*&);

class MessageHandler
{
	public:
		MessageHandler();
		~MessageHandler();
		void init();
		void run(); // check if there is a message in buffer to process.

                void insertChar(char c);

		void setCallback(funcPtr func);

		int find(char *aStr);
        char getChar(int aOdx);

		//get
		size_t bufferSize() const; ///< return the buffer size.
		int getMessage(Message *&rMsg, int aStartPosInBuffer);

		//debug
		void printBuffer() const;
		void getBuffer(char *aBuffer);
	private:
		bool hasMessage() const;
	private:
        volatile char mBuffer[BUFFER_SIZE];
		size_t mBufferSize;
	//	int mBufferStart;
		int mBufferEnd;

		funcPtr mCallback;;
};


#endif
