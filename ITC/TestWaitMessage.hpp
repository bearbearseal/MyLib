/*********************************************************************************
 * Test and demo 1 thread waiting for messages from another.					*
 * The waiting thread would be put to sleep until signaled (messages come in).	*
 * The message is of structure type.											*
*********************************************************************************/
#include "ITC.h"
#include <thread>

using namespace std;

namespace TestWaitMessage {

	struct MessageStruct {
		double theNumber;
		string theString;
	};
	
	//Wait for message then output the message which includes a double and a string.
	//If the string equals "x", print a good bye message.
	//Run until the ITC that created the socket is no longer valid
	void wait_message(shared_ptr<ITC<MessageStruct>::Socket>* _socket) {
		//quickly make a copy of the _socket, hopefully it is not freed yet.
		//This thread would be golden once it secured a shared_ptr to *_socket;
		shared_ptr<ITC<MessageStruct>::Socket> socket = *_socket;
		while(socket->wait_message()) {
			auto message = socket->get_message();
			if(!message.message.theString.compare("x")){
				printf("Good bye.\n");
			}
			printf("Got message, number value: %f, string value: %s\n", message.message.theNumber, message.message.theString.c_str());
		}
		printf("Wait message failed, ITC hub is no longer valid.\n");
	}

	//Wait for user input, send the input together with a double to the other thread
	//If user input equals "x", quit.
	void run(){
		double theNumber = 123456789;
		//shared_ptr<ITC<MessageStruct>::Socket> getSocket;
		//shared_ptr<ITC<MessageStruct>::FixedSocket> sendSocket;
		thread* theThread;
		{
			ITC<MessageStruct> itc;
			auto getSocket = itc.create_socket(1);
			auto sendSocket = itc.create_fixed_socket(2, 1);
			theThread = new thread(wait_message, &getSocket);
			while (1) {
				printf("Type 'x' to quit and anything else to send message.\n");
				string theString;
				char theChar = getchar();
				while ('\n' != theChar) {
					theString += theChar;
					theChar = getchar();
				}
				theNumber += 1;
				theNumber /= 2;
				MessageStruct message;
				message.theNumber = theNumber;
				message.theString = theString;
				sendSocket->send_message(message);
				printf("Message sent.\n");
				if (!theString.compare("x")) {
					printf("Main thread quiting.\n");
					break;
				}
				else {
					printf("Message: %s\n", theString.c_str());
				}
			}
		}
		printf("Waiting for thread to join.\n");
		theThread->join();
		delete theThread;
	}
}
