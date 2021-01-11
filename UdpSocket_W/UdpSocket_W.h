#pragma once
#ifndef _UdpSocket_W_H_
#define _UdpSocket_W_H_
#include <winsock2.h>
#include <ws2tcpip.h>
#include <utility>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

class UdpSocket {
public:
	class Address {
		friend class UdpSocket;
	public:
		Address();
		Address(const Address& theOther);
		~Address();
		std::pair<std::string, uint16_t> to_address_and_port() const;
		bool is_valid() { return isValid; }
	private:
		bool isValid;
		size_t length;
		struct sockaddr myAddress;
	};
	UdpSocket(bool blocking = true);
	~UdpSocket();
	static Address to_address(const std::string& hostAddr, uint16_t portNumber);
	static std::pair<std::string, uint16_t> to_ip_and_port(const Address& address);
	bool open(const Address& address);
	bool listen(uint16_t portNumber, bool useIpV6 = false);
	void close();
	std::string read();
	std::string read(Address& hisAddress);
	int32_t write(const std::string& message);
	int32_t write(const std::string& message, const Address& address);

private:
	SOCKET socketFd = INVALID_SOCKET;
	//struct sockaddr hisAddress;
	//int addressLength;
	bool blocking;
	WSADATA wsaData;
};

#endif
