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
    Message(const char *aMsg, size_t aSize);

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

    void add(const char* aKey, double aValue);
    void add(const char *aKey, float aValue);
    void add(const char* aKey, int aValue);
	void add(const char* aKey, bool aValue);
	void add(const char* aKey, const char *aValue);

    void finnish(); ///< prepare meassege to be transmited.

    int getSize();
    void getMessageData(char *msg);

    int getMessage( char *&rMessage); ///< get the message
	/** brief Direct set the message buffer,
		@warning replaces existing message if it exist.
	**/
    void setMessage(char *aMsg, size_t aMessageSize);
	void print();
private:
    void calcCheckcode();
    int validateMessage(const char *aMsg, size_t aSize);

private:
    char mMessage[100];
    size_t mMessageSize;
    int mIsValid;

};

#endif // MESSAGE_H
