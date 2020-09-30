/*
 * main.cpp
 *
 *  Created on: Jun 27, 2019
 *      Author: lian
 */

#include "TcpSocket.h"
#include <unistd.h>
#include <chrono>
#include <thread>

void test_connect(TcpSocket& tcpSocket, const std::string& host, uint16_t portNumber){
	tcpSocket.open(host.c_str(), portNumber);
	printf("Connecting to %s port %u.\n", tcpSocket.get_his_ip().c_str(), tcpSocket.get_his_port());
	while(!tcpSocket.connection_established())
	{
		printf("Trying to connect\n");
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	printf("Connection established his address: %s:%u.\n", tcpSocket.get_his_ip().c_str(), tcpSocket.get_his_port());
	tcpSocket.write("U had accepted my connection request.\n");
	std::pair<bool, std::string> reply;
	do{
		printf("Trying to read.\n");
		reply = tcpSocket.read();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}while(reply.first && !reply.second.size());
	if(reply.second.size()){
		printf("His reply: %s", reply.second.c_str());
	}
	else{
		printf("Connection closed.\n");
	}
	tcpSocket.close();
}

void test_connect(TcpSocket& tcpSocket){
	tcpSocket.open();
	printf("Connecting to %s port %u.\n", tcpSocket.get_his_ip().c_str(), tcpSocket.get_his_port());
	while(!tcpSocket.connection_established())
	{
		printf("Trying to connect.\n");
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	printf("Connection established his address: %s:%u.\n", tcpSocket.get_his_ip().c_str(), tcpSocket.get_his_port());
	tcpSocket.write("U had accepted my connection request.\n");
	std::pair<bool, std::string> reply;
	do
	{
		printf("Trying to read.\n");
		reply = tcpSocket.read();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}while(reply.second.size() == 0 && reply.first);
	if(reply.second.size())
	{
		printf("His reply: %s", reply.second.c_str());
	}
	else
	{
		printf("Connection closed.\n");
	}
	tcpSocket.close();
}

void test_listen(TcpSocket& tcpSocket, uint16_t port){
	tcpSocket.listen(port);
	TcpSocket newSocket;
	printf("Listening to port %u.\n", port);
	while(!tcpSocket.accept(newSocket))
	{
		printf("Listening.\n");
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	printf("Connection established his address: %s:%u.\n", newSocket.get_his_ip().c_str(), newSocket.get_his_port());
	newSocket.write("Your connection request is accepted.\n");
	std::pair<bool, std::string> reply;
	do
	{
		printf("Trying to read.\n");
		reply = newSocket.read();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}while(reply.second.size() == 0 && reply.first);
	if(reply.second.size())
	{
		printf("His reply: %s", reply.second.c_str());
	}
	else
	{
		printf("Connection closed.\n");
	}
	newSocket.close();
	printf("Listening to port %u.\n", port);
	while(!tcpSocket.accept())
	{
		printf("Listening.\n");
		usleep(1000000);
	}
	printf("Connection established his address: %s:%u.\n", tcpSocket.get_his_ip().c_str(), tcpSocket.get_his_port());
	tcpSocket.write("Your connection request is accepted.\n");
	do
	{
		printf("Trying to read.\n");
		reply = tcpSocket.read();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}while(reply.second.size() == 0 && reply.first);
	if(reply.second.size())
	{
		printf("His reply: %s", reply.second.c_str());
	}
	else
	{
		printf("Connection closed.\n");
	}
	tcpSocket.close();
}

int main(int argc , char *argv[])
{
	TcpSocket tcpSocket(true);
	test_connect(tcpSocket, "127.0.0.1", 7777);
	test_connect(tcpSocket, "192.168.137.1", 502);
	tcpSocket.set_host("::1", 7777);
	test_connect(tcpSocket);
	test_listen(tcpSocket, 6666);
}


