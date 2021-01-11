#include "UdpSocket_W.h"

using namespace std;

UdpSocket::Address::Address() {
	isValid = false;
}

UdpSocket::Address::Address(const Address& theOther) {
	memcpy(&this->myAddress, &theOther.myAddress, sizeof(myAddress));
	this->length = theOther.length;
	this->isValid = theOther.isValid;
}

UdpSocket::Address::~Address() {

}

std::pair<std::string, uint16_t> UdpSocket::Address::to_address_and_port() const {
	return UdpSocket::to_ip_and_port(*this);
}

UdpSocket::UdpSocket(bool _blocking) {
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		printf("WSAStartup failed with error %d\n", result);
	}
	blocking = _blocking;
}

UdpSocket::~UdpSocket() {
	close();
	WSACleanup();
}

UdpSocket::Address UdpSocket::to_address(const std::string& hostAddr, uint16_t portNumber) {
	Address retVal;

	ADDRINFO hints, *addrInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	int result = getaddrinfo(hostAddr.c_str(), to_string(portNumber).c_str(), &hints, &addrInfo);
	if (result != 0) {
		printf("Cannot resolve address, error %d, wsa %d\n", result, WSAGetLastError());
	}
	else {
		retVal.length = addrInfo->ai_addrlen;
		memcpy(&retVal.myAddress, addrInfo->ai_addr, sizeof(retVal.myAddress));
		retVal.isValid = true;
		freeaddrinfo(addrInfo);
	}
	return retVal;
}

std::pair<std::string, uint16_t> UdpSocket::to_ip_and_port(const Address& address) {
	string sourceIp;
	uint16_t sourcePortNumber;
	if (address.myAddress.sa_family == AF_INET6) {
		char ipAddress[64];
		struct sockaddr_in6* ipV6 = (struct sockaddr_in6*)&address.myAddress;
		inet_ntop(AF_INET6, &ipV6->sin6_addr, ipAddress, sizeof(ipAddress));
		sourceIp = ipAddress;
		sourcePortNumber = ntohs(ipV6->sin6_port);
	}
	else {
		char ipAddress[32];
		struct sockaddr_in* ipV4 = (struct sockaddr_in*)&address.myAddress;
		inet_ntop(AF_INET, &ipV4->sin_addr, ipAddress, sizeof(ipAddress));
		sourceIp = ipAddress;
		sourcePortNumber = ntohs(ipV4->sin_port);
	}
	return { sourceIp, sourcePortNumber };
}

bool UdpSocket::open(const Address& address) {
	if (socketFd != INVALID_SOCKET) {
		closesocket(socketFd);
		socketFd = INVALID_SOCKET;
	}
	socketFd = socket(address.myAddress.sa_family, SOCK_DGRAM, IPPROTO_UDP);
	if (socketFd == INVALID_SOCKET) {
		printf("socket() failed with error %d\n", WSAGetLastError());
		return false;
	}
	if (!blocking) {
		u_long iMode = 1;
		int result = ioctlsocket(socketFd, FIONBIO, &iMode);
		if (result) {
			printf("ioctlsocket() failed with error %d\n", WSAGetLastError());
			return false;
		}
	}
	if (connect(socketFd, &address.myAddress, int(address.length)))
	{
		printf("connect() failed with error %d\n", WSAGetLastError());
		return false;
	}
	return true;
}

bool UdpSocket::listen(uint16_t portNumber, bool useIpV6) {
	if (socketFd != INVALID_SOCKET) {
		closesocket(socketFd);
		socketFd = INVALID_SOCKET;
	}
	ADDRINFO hints, * addrInfo;
	memset(&hints, 0, sizeof(hints));
	if (useIpV6) {
		socketFd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
		hints.ai_family = AF_INET6;
	}
	else {
		socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		hints.ai_family = AF_INET;
	}
	if (socketFd == INVALID_SOCKET) {
		printf("socket() failed with error %d\n", WSAGetLastError());
		return false;
	}
	if (!blocking) {
		u_long iMode = 1;
		int result = ioctlsocket(socketFd, FIONBIO, &iMode);
		if (result) {
			printf("ioctlsocket() failed with error %d\n", WSAGetLastError());
			return false;
		}
	}
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_NUMERICHOST | AI_PASSIVE;
	auto result = getaddrinfo(nullptr, to_string(portNumber).c_str(), &hints, &addrInfo);
	if (result != 0) {
		printf("getaddrinfo() fail with error %d\n", result);
		return false;
	}
	if (bind(socketFd, addrInfo->ai_addr, int(addrInfo->ai_addrlen)) == SOCKET_ERROR) {
		printf("bind() failed with error %d\n", WSAGetLastError());
		freeaddrinfo(addrInfo);
		return false;
	}
	freeaddrinfo(addrInfo);
	return true;
	/*
	struct sockaddr hisAddress;
	memset(&hisAddress, 0, sizeof(hisAddress));
	//hisAddress.sa_family = AF_INET6;
	struct sockaddr_in6* ipV6 = (struct sockaddr_in6*)&hisAddress;
	ipV6->sin6_family = AF_INET6;
	ipV6->sin6_port = htons(portNumber);
	if (bind(socketFd, &hisAddress, sizeof(in6addr_any))<0) {
		printf("bind() failed with error %d\n", WSAGetLastError());
		return false;
	}
	return true;
	*/
}

void UdpSocket::close() {
	if (socketFd != INVALID_SOCKET) {
		::closesocket(socketFd);
		socketFd = INVALID_SOCKET;
	}
}

std::string UdpSocket::read() {
	std::string retVal;
	sockaddr sourceAddress;
	int addressSize = sizeof(sourceAddress);
	int length;
	char recvBuffer[64 * 1024];
	length = recvfrom(socketFd, recvBuffer, sizeof(recvBuffer) - 1, 0, &sourceAddress, &addressSize);
	if (length != -1) {
		retVal.assign(recvBuffer, length);
	}
	return retVal;
}

std::string UdpSocket::read(Address& hisAddress) {
	std::string retVal;
	int addressSize = sizeof(hisAddress.myAddress);
	int length;
	char recvBuffer[16 * 1024];
	length = recvfrom(socketFd, recvBuffer, sizeof(recvBuffer) - 1, 0, &hisAddress.myAddress, &addressSize);
	printf("His address length: %d\n", addressSize);
	if (length != -1) {
		retVal.assign(recvBuffer, length);
	}
	else {
		printf("recvfrom() failed with error %d\n", WSAGetLastError());
	}
	return retVal;
}

int32_t UdpSocket::write(const std::string& message) {
	int result = send(socketFd, message.c_str(), int(message.size()), 0);
	if (result == SOCKET_ERROR) {
		printf("send() failed with error %d\n", WSAGetLastError());
		return -1;
	}
	return result;
}

int32_t UdpSocket::write(const std::string& message, const Address& address) {
	int result = sendto(socketFd, message.c_str(), int(message.size()), 0, &address.myAddress, int(address.length));
	if (result == SOCKET_ERROR) {
		printf("sendto() failed with error %d\n", WSAGetLastError());
		return -1;
	}
	return result;
}
