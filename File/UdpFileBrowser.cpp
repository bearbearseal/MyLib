#include "UdpFileBrowser.h"

using namespace std;

UdpFileBrowser::UdpFileBrowser(uint16_t portNumber, const std::string& folderPath) : UdpListener(portNumber), fileBrowser(folderPath) {

}

UdpFileBrowser::~UdpFileBrowser() {

}

void UdpFileBrowser::catch_message(std::string& data, const UdpSocket::Address& hisAddress) {
    string reply = fileBrowser.parse_command(data);
    this->write_message(reply, hisAddress);
}
