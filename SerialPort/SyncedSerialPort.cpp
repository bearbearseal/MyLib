#include "SyncedSerialPort.h"

using namespace std;

SyncedSerialPort::SyncedSerialPort() {

}

SyncedSerialPort::~SyncedSerialPort() {

}

void SyncedSerialPort::open(const std::string& portName, const SyncedSerialPort::Config& config) {
    //printf("Opening serial port %s\n", portName.c_str());
    serialPort.open(portName);
    SerialPort::Config _config;
    _config.baudrate = config.baudrate;
    _config.bitPerByte = config.bitPerByte;
    _config.blocking = false;
    _config.hardwareFlowControl = config.hardwareFlowControl;
    _config.parityBit = config.parityBit;
    _config.softwareFlowControl = config.softwareFlowControl;
    _config.stopBit = config.stopBit;
    _config.timeout = 0;
    serialPort.configure(_config);
}

void SyncedSerialPort::open(const std::string& portName, uint32_t baudrate) {
    serialPort.open(portName);
    SerialPort::Config _config;
    _config.baudrate = baudrate;
    serialPort.configure(_config);
}

std::string SyncedSerialPort::write_then_read(const std::string& writeData, size_t expectedReplyLength, std::chrono::milliseconds timeout) {
    lock_guard<mutex> lock(portMutex);
    //printf("Writing %zu bytes expects %zu bytes.\n", writeData.size(), expectedReplyLength);
    auto result = serialPort.write(writeData);
    std::string received;
    if(result.first && result.second == writeData.size()) {
        auto stop = std::chrono::steady_clock::now() + timeout;
        while(received.size() < expectedReplyLength && stop > std::chrono::steady_clock::now()) {
            this_thread::sleep_for(20ms);
            received += serialPort.read();
        }
    }
    this_thread::sleep_for(delay);
    //printf("Got %zu bytes reply.\n", received.size());
    return received;
}

void SyncedSerialPort::flush_read() {
    serialPort.read();
}
