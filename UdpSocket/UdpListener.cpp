#include <iostream>
#include "UdpListener.h"

using namespace std;

UdpListener::UdpListener(uint16_t portNumber) : socket(true){
    socket.listen(portNumber);
}

UdpListener::~UdpListener() {
    stop();
}

void UdpListener::start() {
    if(theProcess == nullptr) {
        keepRunning = true;
        theProcess = new thread(thread_process, this);
    }
}

void UdpListener::stop() {
    if(theProcess != nullptr) {
        keepRunning = false;
        socket.close();
        theProcess->join();
        delete theProcess;
        theProcess = nullptr;
    }
}

void UdpListener::write_message(const std::string& message, const UdpSocket::Address& hisAddress) {
    socket.write(message, hisAddress);
}

void UdpListener::catch_message(std::string& data, const UdpSocket::Address& hisAddress) {
    auto addressPort = UdpSocket::to_ip_and_port(hisAddress);
    cout<<"Got Data: "<<data<<" from "<<addressPort.first<<" "<<addressPort.second<<endl;
}

void UdpListener::thread_process(UdpListener* me) {
    while(1) {
        UdpSocket::Address hisAddress;
        string input = me->socket.read(hisAddress);
        if(input.size()) {
            me->catch_message(input, hisAddress);
        }
    }
}
