#include "../messagehandler.h"
#include "../message.h"

#include <iostream>
#include <string>

void callback(Message *aMsg)
{
	std::cout << "Callback" << std::endl;

}


int main(int argc, char *argv[])
{

	MessageHandler *msg = new MessageHandler(30);
	msg->setCallback(callback);
	for(int i=0; i< 55; ++i)
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

	std::string str = "<msg0019key:f33SAn>";

	for(int i=0; i<str.size(); ++i)
	{
		msg->insertChar(str.at(i));
	}

/*	Message msg2;
	msg2.init();
	float val = 13.2;
	msg2.add("key", val);
	msg2.finnish();
	unsigned char *m;
	msg2.getMessage(m);
	std::cout << m << std::endl;*/

	return 0;
}
