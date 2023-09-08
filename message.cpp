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

#include "message.h"

#include <string.h>
#include <stdlib.h>


namespace message
{
	static void clear_buffer(Message *msg)
	{
		int i = 0;
		for(i=0; i<MAX_SIZE; ++i)
		{
			msg->buffer[i] = 0;
		}
		msg->size = 0;
	}

	static void write_header(Message *msg)
	{
		msg->buffer[msg->size++] = '<';
		msg->buffer[msg->size++] = 'm';
		msg->buffer[msg->size++] = 's';
		msg->buffer[msg->size++] = 'g';
	}

	static void write_device_name(Message *msg, const char* device_name)
	{
		msg->buffer[msg->size++] = ':';
		size_t len = strlen(device_name);
		for(int i=0; i<len; ++i)
		{
			msg->buffer[msg->size++] = device_name[i];
		}
		msg->buffer[msg->size++] = ':';
		msg->size_location = msg->size++;
	}

	static uint8_t calc_checksum(Message *msg)
	{
		int sum = 0;
		for(int i=0; i<msg->size+1; ++i)
		{
			sum += msg->buffer[i];
		}
		int r = sum % 256;
		return 256 - r;
	}


	void reset_message(Message *msg, const char* device)
	{
		clear_buffer(msg);
		write_header(msg);
		write_device_name(msg, device);
	}


	void finnish_message(Message *msg)
	{
		msg->buffer[msg->size++] = 'A'; // check sum
		msg->buffer[msg->size] = '>';
		msg->buffer[msg->size_location] = msg->size;

		msg->buffer[msg->size - 1] = calc_checksum(msg);
    }





    void add(Message *msg, const char *key, float value)
    {
        size_t keySize = strlen(key);
        for(unsigned int i=0; i<keySize; ++i)
            msg->buffer[msg->size++] = key[i];

        msg->buffer[msg->size++] = ':';
        msg->buffer[msg->size++] = 'f';

        union U
        {
            float f;
            uint8_t bytes[4];
        }u;
        u.f = value;
        msg->buffer[msg->size++] = u.bytes[0];
        msg->buffer[msg->size++] = u.bytes[1];
        msg->buffer[msg->size++] = u.bytes[2];
        msg->buffer[msg->size++] = u.bytes[3];
    }

    void add(Message *msg, const char* key, int value)
    {
        size_t keySize = strlen(key);
        for(unsigned int i=0; i<keySize; ++i)
            msg->buffer[msg->size++] = key[i];

        msg->buffer[msg->size++] = ':';
        msg->buffer[msg->size++] = 'i';

        union U
        {
            int i;
            uint8_t bytes[4];
        }u;

        u.i = value;
        msg->buffer[msg->size++] = u.bytes[0];
        msg->buffer[msg->size++] = u.bytes[1];
        msg->buffer[msg->size++] = u.bytes[2];
        msg->buffer[msg->size++] = u.bytes[3];
    }

    /**
        the value is encoded to bX where x is 0 or 1
    **/

    void add(Message *msg, const char* key, bool value)
    {
        size_t keySize = strlen(key);

        for(unsigned int i=0; i<keySize; ++i)
            msg->buffer[msg->size++] = key[i];

        msg->buffer[msg->size++] = ':';
        msg->buffer[msg->size++] = 'b';
        msg->buffer[msg->size++] = (value) ? uint8_t(1) : uint8_t(0);

    }


    /** \brief Add key value to message,

        the value is marked with prefix cXX, wher XX is binary short that
        tells the size of the value text.

    **/
    void add(Message *msg, const char* key, const char *value)
    {
        size_t keySize = strlen(key);
        size_t valueSize = strlen(value);

        for(unsigned int i=0; i<keySize; ++i)
            msg->buffer[msg->size++] = key[i];

        msg->buffer[msg->size++] = ':';
        msg->buffer[msg->size++] = 'c';

        union shortBytes
        {
            unsigned short asShort;
            uint8_t asByte[2];
        }sb;
        sb.asShort = (short) valueSize; // n chars in value string.

        msg->buffer[msg->size++] = sb.asByte[0];
        msg->buffer[msg->size++] = sb.asByte[1];

        for(unsigned int i=0; i<valueSize; ++i)
            msg->buffer[msg->size++] = value[i];
    }

}//end namespace
