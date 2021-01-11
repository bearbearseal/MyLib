#include "UdpSocket_W.h"
#include <iostream>
#include <thread>

using namespace std;

namespace Test {
	void test_my_socket() {
		UdpSocket aSocket;
		if (!aSocket.listen(8888)) {
			printf("Udp socket listen failed.\n");
			return;
		}
		std::string incoming;
		uint16_t i = 1;
		UdpSocket::Address anotherAddress;
		auto destinationAddress = UdpSocket::to_address("192.168.56.101", 9989);
		auto resultSet = UdpSocket::to_ip_and_port(destinationAddress);
		printf("From %s:%u\n", resultSet.first.c_str(), resultSet.second);
		//aSocket.open(destinationAddress);
		//auto anotherAddress = UdpSocket::to_address("fe80::4d29:59c3:693:540e", 8888);
		while (1) {
			//printf("Waiting.\n");
			incoming = aSocket.read(anotherAddress);
			//printf("Received.\n");
			if (incoming.size()) {
				printf("Data read: %s\n", incoming.c_str());
				auto result = UdpSocket::to_ip_and_port(anotherAddress);
				printf("From %s:%u\n", result.first.c_str(), result.second);
				std::string message;
				message = "Reply ";
				message += std::to_string(i);
				message += "\n";
				++i;
				printf("Sending 1.\n");
				aSocket.write(message);
				printf("Sending 2.\n");
				aSocket.write(message, destinationAddress);
			}
			this_thread::sleep_for(1s);
		}
	}

	void test_socket_as_client() {
		UdpSocket aSocket;
		auto destAddr = UdpSocket::to_address("192.168.56.101", 9989);
		aSocket.open(destAddr);
		aSocket.write("Hello");
		UdpSocket::Address replyAddress;
		string incoming = aSocket.read(replyAddress);
		printf("Reply: %s\n", incoming.c_str());
		auto decodedAddr = replyAddress.to_address_and_port();
		printf("Address=>%s:%u\n", decodedAddr.first.c_str(), decodedAddr.second);
	}
}
