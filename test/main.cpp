#include "../messagehandler.h"

#include <iostream>

int main(int argc, char *argv[])
{

	MessageHandler *msg = new MessageHandler(10);

	for(int i=0; i< 20; ++i)
	{
		msg->printBuffer();
		msg->insertChar(77+i);

		std::cout << "\n";
	}
	std::cout << "\n";;

	return 0;
}
