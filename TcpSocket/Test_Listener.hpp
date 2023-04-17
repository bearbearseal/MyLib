#ifndef _TEST_LISTENER_HPP_
#define _TEST_LISTENER_HPP_
#include "TcpListener.h"
#include <thread>
#include <chrono>

using namespace std;

class DerivedTcpListener : public TcpListener{
public:
    DerivedTcpListener(uint16_t port) : TcpListener(port) {}
    void catch_message(const std::string& message, size_t handle) {
        string reply = "U sent: ";
        reply += message;
        write_message(handle, reply);
    }
};

namespace Test_Listener {

    void run() {
        DerivedTcpListener tcpListener(56789);
        tcpListener.start();

        while (1) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

}

#endif