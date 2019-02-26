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
		MessageHandler(size_t aBufferSize = BUFFER_SIZE);
		~MessageHandler();
		void init(size_t aBufferSize=BUFFER_SIZE);
		void run(); // check if there is a message in buffer to process.

                void insertChar(char c);

		void setCallback(funcPtr func);

		int find(char *aStr);
                char getChar(int aOdx);

		//get
		size_t bufferSize() const; ///< return the buffer size.
		size_t size() const; ///< return the size of chars current in the buffer.
		int getMessage(Message *&rMsg, int aStartPosInBuffer);

		//debug
		void printBuffer() const;
		void getBuffer(char *aBuffer);
	private:
		bool hasMessage() const;
	private:
                char* mBuffer;
		size_t mBufferSize;
		int mBufferStart;
		int mBufferEnd;

		funcPtr mCallback;;
};


#endif
