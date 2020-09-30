#pragma once
#include <termios.h>
#include <string>
#include <stdint.h>
#include <tuple>

class SerialPort
{
public:
	struct Config
	{
		bool parityBit = false;
		bool stopBit = false;
		uint8_t bitPerByte = 8;
		bool hardwareFlowControl = false;
		bool softwareFlowControl = false;
		bool blocking = false;
		uint16_t timeout;
		uint32_t baudrate;
	};
	SerialPort();
	~SerialPort();
	
	bool open(const std::string& portName);
	void close();
	
	bool configure(const Config& config);
	
	std::pair<bool, uint16_t> write(const std::string& message);
	std::string read();
	
private:
	int fileDescriptor;
	termios tty;
};
