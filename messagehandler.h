#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "message.h"

#define BUFFER_SIZE 128 ///< default size of buffer



/** /brief class MessageHandler

	Handler uart rx, recieved chars are stored in a circular buffer,
	and automaticaly look for messages within the buffer.

*/

typedef void (*funcPtr)(Message*);

class MessageHandler
{
	public:
		MessageHandler(size_t aBufferSize = BUFFER_SIZE);
		~MessageHandler();

		void insertChar(unsigned char c);

		void setCallback(funcPtr func);

		int find(char *aStr);
		unsigned char getChar(unsigned int aOdx);		

		//get
		size_t bufferSize() const; ///< return the buffer size.
		size_t size() const; ///< return the size of chars current in the buffer.
		int getMessage(Message *&rMsg, int aStartPosInBuffer);

		//debug
		void printBuffer() const;

	private:
		bool hasMessage() const;
	private:
		unsigned char* mBuffer;
		size_t mBufferSize;
		int mBufferStart;
		int mBufferEnd;

		funcPtr mCallback;;
};


#endif
