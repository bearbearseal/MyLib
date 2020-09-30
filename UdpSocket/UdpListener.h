#ifndef _UdpListener_H_
#define _UdpListener_H_
#include <stdint.h>
#include <string>
#include <thread>
#include "UdpSocket.h"

class UdpListener {
public:
    UdpListener(uint16_t portNumber);
    virtual ~UdpListener();

    void start();
    void stop();

    void write_message(const std::string& message, const UdpSocket::Address& hisAddress);

protected:
    virtual void catch_message(std::string& data, const UdpSocket::Address& hisAddress);

private:
    //Blocking socket
    bool keepRunning;
    UdpSocket socket;
    std::thread* theProcess = nullptr;

    static void thread_process(UdpListener* me);
};
#endif