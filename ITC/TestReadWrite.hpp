/************************************************************************************************
* Test and demo 2 threads sending messages to 1 thread.											*
* The sending thread would send 2000 messages and expects to receive 1 reply for each message.	*
* The receiving thread would reply to every incoming message with 1 message.					*
************************************************************************************************/
#include "ITC.h"
#include <thread>
#include <string>
#include <memory>

using namespace std;

namespace TestReadWrite
{
	//Do the following for 2000 times:
	//{
	//	Write a message
	//	If already has reply, read it and print it out
	//}
	//Wait and read until a total of 2000 messages has arrived
	void write_message_thread(shared_ptr<ITC<string>::FixedSocket> socket) {
		unsigned replyCount = 0;
		for (unsigned i = 0; i < 2000; ++i) {
			string message = "message ";
			message += to_string(i);
			socket->send_message(message);
			while (socket->has_message()) {
				auto reply = socket->get_message();
				++replyCount;
				printf("Socket %lu: Got \"%s\" from socket %lu, replyCount %u\n", 
					socket->get_id(), reply.message.c_str(), reply.sourceSocketId, replyCount);
			}
		}
		if(replyCount >= 2000){
			return;
		}
		while (socket->wait_message()) {
			auto reply = socket->get_message();
			++replyCount;
			printf("Socket %lu: Got \"%s\" from socket %lu, replyCount %u\n", 
				socket->get_id(), reply.message.c_str(), reply.sourceSocketId, replyCount);
			if (replyCount >= 2000) {
				return;
			}
		}
	}

	//Wait for message
	//For each message arrived, send a reply back
	//Repeat until a message from socket 4 is received.
	void read_message_thread(shared_ptr<ITC<string>::Socket> socket) {
		ITC<string>::InData inMessage;
		while (1) {
			if (socket->wait_message()) {
				inMessage = socket->get_message();
				string reply = "Your message: ";
				reply += inMessage.message;
				socket->message_to_socket(inMessage.sourceSocketId, reply);
				if(inMessage.sourceSocketId == 4){
					return;
				}
			}
		}
	}

	//Creates 2 send threads
	//Create 1 receive thread
	void test_2_vs_1() {
		shared_ptr<ITC<string>> itc = make_shared<ITC<string>>();
		shared_ptr<ITC<string>::Socket> socket1 = itc->create_socket(1);
		shared_ptr<ITC<string>::FixedSocket> socket2 = itc->create_fixed_socket(2, 1);
		shared_ptr<ITC<string>::FixedSocket> socket3 = itc->create_fixed_socket(3, 1);
		shared_ptr<ITC<string>::FixedSocket> socket4 = itc->create_fixed_socket(4, 1);
		auto thread1 = new thread(read_message_thread, socket1);
		auto thread2 = new thread(write_message_thread, socket2);
		auto thread3 = new thread(write_message_thread, socket3);
		string aMessage = "bye";
		thread2->join();
		thread3->join();
		socket4->send_message(aMessage);
		thread1->join();
		delete thread1;
		delete thread2;
		delete thread3;
		printf("Press any key to quit.");
		getchar();
	}
}
