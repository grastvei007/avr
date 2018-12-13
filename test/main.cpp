#include "../messagehandler.h"

#include <iostream>

void callback(Message *aMsg)
{
	std::cout << "Callback" << std::endl;

}


int main(int argc, char *argv[])
{

	MessageHandler *msg = new MessageHandler(10);
	msg->setCallback(callback);
	for(int i=0; i< 15; ++i)
	{
		msg->printBuffer();
		msg->insertChar(77+i);

		std::cout << "    " << msg->size()  <<  "\n";
	}
	std::cout << "\n";


	msg->insertChar('m');
	msg->insertChar('s');
	msg->insertChar('g');


	msg->printBuffer();
	std::cout << std::endl;

	int pos = msg->find("msg");
	std::cout << pos << std::endl;

	return 0;
}
