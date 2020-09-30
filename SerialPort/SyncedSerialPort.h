#ifndef _SyncedSerialPort_H_
#define _SyncedSerialPort_H_
#include "SerialPort.h"
#include "../ITC/ITC.h"
#include <string>
#include <chrono>
#include <mutex>

class SyncedSerialPort {
public:
    struct Config {
      bool parityBit = false;
      bool stopBit = false;
      uint8_t bitPerByte = 8;
      bool hardwareFlowControl = false;
      bool softwareFlowControl = false;
      uint32_t baudrate;
    };
    SyncedSerialPort();
    ~SyncedSerialPort();

    void open(const std::string& portName, const SyncedSerialPort::Config& config);
    void open(const std::string& portName, uint32_t baudrate);
    void set_delay(std::chrono::milliseconds delay) { this->delay = delay; }
    std::string write_then_read(const std::string& writeData, size_t expectedReplyLength, std::chrono::milliseconds timeout);
    void flush_read();

private:
    SerialPort serialPort;
    std::mutex portMutex;
    std::chrono::milliseconds delay;
};

#endif
