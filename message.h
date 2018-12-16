#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdio.h>

/**
 * @brief The Message class
 *
 *
 * The message starts with <msg and ends with r> where r is a control byte.
 * the sum of all bytes in the message modulo 256 = r.
 * after <msg is 4 bytes that describes the size of the message, it is an int in
 * plain text.
 *
 */
class Message
{
public:
    Message();
    Message(unsigned char *aMsg, size_t aSize);

	static const int HeaderSize = 4;
	static const int MaxKeySize = 20;

	void init();
	void destroy(); ///< free message buffer.

    /**
     * @brief isValid
     *
     * error codes:
     *   1 - all good
     *  -1 - error length
     *  -2 - error in header
     *  -3 - checksum error
     *
     *
     * @return int an error code
     */
    int isValid();

    void add(char* aKey, double aValue);
    void add(char *aKey, float aValue);
    void add(char* aKey, int aValue);
	void add(char* aKey, bool aValue);
	void add(char* aKey, char *aValue);

    void finnish(); ///< prepare meassege to be transmited.

    int getMessage(unsigned char *&rMessage); ///< get the message
	/** brief Direct set the message buffer,
		@warning replaces existing message if it exist.
	**/
	void setMessage(unsigned char *&aMsg, size_t aMessageSize);
	void print();
private:
    void calcCheckcode();
    int validateMessage(const unsigned char *aMsg, size_t aSize);

private:
    unsigned char *mMessage;
    size_t mMessageSize;
    int mIsValid;

};

#endif // MESSAGE_H
