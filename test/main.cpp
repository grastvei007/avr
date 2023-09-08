#include <iostream>
#include <string>

#include "../message.h"


Message msg;

void print_message(Message *msg);
bool isValid(Message *msg);

void test_valid()
{
    message::reset_message(&msg, "sensor");
    message::finnish_message(&msg);

    if(isValid(&msg))
        std::cout << "Valid message" << std::endl;
    else
        std::cout << "Message is not valid!" << std::endl;
    print_message(&msg);
}

void test_double_message()
{
    std::cout << "Test float message" << std::endl;
    float value = 10.5;

    message::reset_message(&msg, "doubleSensor");
    message::add(&msg, "temp", value);

    message::finnish_message(&msg);

    if(isValid(&msg))
        std::cout << "Valid message" << std::endl;
    else
        std::cout << "Message is not valid!" << std::endl;
    print_message(&msg);

}


int main(int argc, char *argv[])
{
    test_valid();

    test_double_message();
}




void print_message(Message *msg)
{
	std::cout << "Message:" << std::endl;
	std::cout << "size: " << (int)msg->size << std::endl;;
	std::cout << "size_location: " << (int)msg->size_location << std::endl;
	std::cout << "[";
	for(int i=0; i<=msg->size; ++i)
	{
		if( i == 0)
			std::cout << msg->buffer[i];
		else if(i == msg->size_location)
			std::cout << "(" << (int)msg->buffer[i] << ")";
		else if(i == msg->size-1)
			std::cout << "(" << (int)msg->buffer[i] << ")";
		else
			std::cout << msg->buffer[i];
	}
	std::cout << "]" <<  std::endl;
}

bool isValid(Message *msg)
{
	int sum = 0;
	for(int i=0; i <= msg->size; ++i)
		sum += msg->buffer[i];

	return sum % 256;
}
