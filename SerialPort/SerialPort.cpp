#include "SerialPort.h"
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int baudrate_number_to_defined(int number) {
	switch (number) {
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    case 460800:
        return B460800;
    case 500000:
        return B500000;
    case 576000:
        return B576000;
    case 921600:
        return B921600;
    case 1000000:
        return B1000000;
    case 1152000:
        return B1152000;
    case 1500000:
        return B1500000;
    case 2000000:
        return B2000000;
    case 2500000:
        return B2500000;
    case 3000000:
        return B3000000;
    case 3500000:
        return B3500000;
    case 4000000:
        return B4000000;
    default: 
        return -1;
    }
}

int baudrate_defined_to_number(int defined) {
	switch (defined) {
    case B9600:
        return 9600;
    case B19200:
        return 19200;
    case B38400:
        return 38400;
    case B57600:
        return 57600;
    case B115200:
        return 115200;
    case B230400:
        return 230400;
    case B460800:
        return 460800;
    case B500000:
        return 500000;
    case B576000:
        return 576000;
    case B921600:
        return 921600;
    case B1000000:
        return 1000000;
    case B1152000:
        return 1152000;
    case B1500000:
        return 1500000;
    case B2000000:
        return 2000000;
    case B2500000:
        return 2500000;
    case B3000000:
        return 3000000;
    case B3500000:
        return 3500000;
    case B4000000:
        return 4000000;
    default: 
        return -1;
    }
}

SerialPort::SerialPort()
{
	fileDescriptor = -1;	//Invalid file descriptor
}

SerialPort::~SerialPort()
{
	if(fileDescriptor != -1)
	{
		::close(fileDescriptor);
	}
}
	
bool SerialPort::open(const std::string& portName)
{
	fileDescriptor = ::open(portName.c_str(), O_RDWR);
	if(fileDescriptor < 0)
	{
		printf("Serial port open failed.\n");
		return false;
	}
	memset(&tty, 0, sizeof(tty));
	if(tcgetattr(fileDescriptor, &tty)!=0)
	{
		printf("Get serial port config failed: %i: %s\n", errno, strerror(errno));
		::close(fileDescriptor);
		fileDescriptor = -1;
		return false;
	}
	return true;
}

void SerialPort::close()
{
	::close(fileDescriptor);
}

bool SerialPort::configure(const Config& config)
{
	if(config.parityBit)
	{
		tty.c_cflag |= PARENB;
	}
	else
	{
		tty.c_cflag &= ~PARENB;
	}
	if(config.stopBit)
	{
		tty.c_cflag |= CSTOPB;
	}
	else
	{
		tty.c_cflag &= ~CSTOPB;
	}
	switch(config.bitPerByte)
	{
		case 5:
			tty.c_cflag |= CS5;
			break;
		case 6:
			tty.c_cflag |= CS6;
			break;
		case 7:
			tty.c_cflag |= CS7;
			break;
		default:
			tty.c_cflag |= CS8;
			break;
	}
	if(config.hardwareFlowControl)
	{
		tty.c_cflag&=~CRTSCTS;
	}
	else
	{
		tty.c_cflag |= CRTSCTS;
	}
	if(config.softwareFlowControl)
	{
		tty.c_iflag |= (IXON | IXOFF | IXANY);	//Turn off s/w flow ctrl
	}
	else
	{
		tty.c_iflag &= ~(IXON | IXOFF | IXANY);	//Turn off s/w flow ctrl
	}
	tty.c_cflag |= CREAD | CLOCAL;	//Turn on READ & ignore ctrl lines (CLOCAL = 1)
	tty.c_lflag &= ~ICANON;	//non-canonical mode
	tty.c_lflag &= ~ECHO;	//Disable echo
	tty.c_lflag &= ~ECHOE;	//Disable erasure
	tty.c_lflag &= ~ECHONL;	//Disable new-line echo
	tty.c_lflag &= ~ISIG;		//Disable interpretation of INTR, QUIT and SUSP
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); 
	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
	if(config.blocking)
	{
		tty.c_cc[VTIME] = config.timeout;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
		tty.c_cc[VMIN] = 1;
	}
	else
	{
		tty.c_cc[VTIME] = 0;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
		tty.c_cc[VMIN] = 0;
	}
	int baudrate = baudrate_number_to_defined(config.baudrate);
	cfsetispeed(&tty, baudrate);
	cfsetospeed(&tty, baudrate);
	if (tcsetattr(fileDescriptor, TCSANOW, &tty) != 0) 
	{
		printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
		return false;
	}
	return true;
}

std::pair<bool, uint16_t> SerialPort::write(const std::string& message)
{
	int written = ::write(fileDescriptor, message.c_str(), message.size());
	if(written < 0)
	{
		return {false, written};
	}
	else
	{
		return {true, written};
	}
}

std::string SerialPort::read()
{
	std::string retVal;
	char buffer[1024];
	int count;
	do
	{
		count = ::read(fileDescriptor, buffer, sizeof(buffer));
		if(count > 0)
		{
			retVal.append(buffer, count);
		}
		
	}while(count == sizeof(buffer));
	return retVal;
}










