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
#include <stdint.h>

/**
 * @brief The Message class
 *
 * <msg:devicename:size|name:type<value>|checksum|>
 *
 * 	size - 		uint8
 *	type - 		c<size> string
 *		   		f float
 *		   		i int
 *		   		b bool
 *	checksum -	uint8
 *
 *	sum of every byte equal 0
 */

const int MAX_SIZE = 100;

struct Message
{
	uint8_t size;
	uint8_t size_location;
	uint8_t buffer[MAX_SIZE];
};

namespace message
{
	void reset_message(Message *msg, const char* device);
    void finnish_message(Message *msg);


    bool isValid(Message *msg);

//    void add(Message *msg, const char* aKey, double aValue);
    void add(Message *msg, const char *aKey, float aValue);
    void add(Message *msg, const char* aKey, int aValue);
	void add(Message *msg, const char* aKey, bool aValue);
    void add(Message *msg, const char* aKey, const char *aValue);
}


#endif // MESSAGE_H
