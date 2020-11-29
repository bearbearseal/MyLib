#include <string>
#include <filesystem>
#include "../UdpSocket/UdpListener.h"
#include "FileBrowser.h"

class FileBrowserUdpListener : public UdpListener {
public:
    FileBrowserUdpListener(uint16_t portNumber, const std::string& folderPath);
    ~FileBrowserUdpListener();

private:
    void catch_message(std::string& data, const UdpSocket::Address& hisAddress);

private:
    FileBrowser fileBrowser;    
};