#include <stdio.h>
#include "UdpSocket.h"

int main(){
	UdpSocket aSocket;
	if(!aSocket.listen(9989)){
		printf("Udp socket listen failed.\n");
		return 1;
	}
	std::string incoming;
	//std::string hisIpAddress;
	//uint16_t hisPort;
	uint16_t i = 1;
	UdpSocket::Address hisAddress;
	auto anotherAddress = UdpSocket::to_address("127.0.0.1", 65535);
	auto ori = UdpSocket::to_ip_and_port(anotherAddress);
		printf("Ori %s:%u\n", ori.first.c_str(), ori.second);
	aSocket.set_destination(anotherAddress);
	while(1){
		//incoming = aSocket.read(hisIpAddress, hisPort);
		incoming = aSocket.read(hisAddress);
		if(incoming.size()){
			printf("Data read: %s\n", incoming.c_str());
			auto result = UdpSocket::to_ip_and_port(hisAddress);
			printf("From %s:%u\n", result.first.c_str(), result.second);
			std::string message;
			message = "LianViqu ";
			message += std::to_string(i);
			message += "\n";
			++i;
			aSocket.write(message, hisAddress);
			aSocket.write(message, anotherAddress);
			aSocket.write(message);
		}
	}
	return 0;
}
