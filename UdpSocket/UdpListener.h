#ifndef _UdpListener_H_
#define _UdpListener_H_
#include <stdint.h>
#include <string>

class UdpListener {
public:
    UdpListener();
    virtual ~UdpListener();

    void start();
    void stop();

    void add_listener(uint16_t portNumber);
    void write_message(const std::string& message);
};
#endif