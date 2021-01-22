#include <string>
#include <filesystem>
#include "../UdpSocket/UdpListener.h"
#include "FileBrowser.h"

class UdpFileBrowser : public UdpListener {
public:
    UdpFileBrowser(uint16_t portNumber, const std::string& folderPath);
    ~UdpFileBrowser();

private:
    void catch_message(std::string& data, const UdpSocket::Address& hisAddress);

private:
    FileBrowser fileBrowser;    
};