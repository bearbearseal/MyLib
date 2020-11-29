#include "FileBrowserUdpListener.h"

using namespace std;

FileBrowserUdpListener::FileBrowserUdpListener(uint16_t portNumber, const std::string& folderPath) : UdpListener(portNumber), fileBrowser(folderPath) {

}

FileBrowserUdpListener::~FileBrowserUdpListener() {

}

void FileBrowserUdpListener::catch_message(std::string& data, const UdpSocket::Address& hisAddress) {
    string reply = fileBrowser.parse_command(data);
    this->write_message(reply, hisAddress);
}
