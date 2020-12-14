#include "FileIOer.h"
#include "FileBrowser.h"
#include "FileBrowserUdpListener.h"
#include <iostream>
#include <thread>

using namespace std;

namespace Test {
    void run_file_browser() {
        FileBrowser aFileBrowser("/var/InOutOpt");
        string result;
        result = aFileBrowser.parse_command("{\"Command\":\"CreateFolder\", \"Name\":\"Folder1\"}");
        cout<<result<<endl;
        result = aFileBrowser.parse_command("{\"Command\":\"CreateFile\", \"Name\":\"File1.txt\"}");
        cout<<result<<endl;
        result = aFileBrowser.parse_command("{\"Command\":\"ToParentFolder\"}");
        cout<<result<<endl;
        result = aFileBrowser.parse_command("{\"Command\":\"Show\"}");
        cout<<result<<endl;
        result = aFileBrowser.parse_command("{\"Command\":\"ReadFile_UTF-8\", \"Name\":\"dsf.txt\"}");
        cout<<result<<endl;
        result = aFileBrowser.parse_command("{\"Command\":\"ReadFile_UTF-8\", \"Name\":\"aFile.txt\"}");
        cout<<result<<endl;
        result = aFileBrowser.parse_command("{\"Command\":\"WriteFile_UTF-8\", \"Name\":\"aFile.txt\", \"Content\":\"dfsdfsf\\nretert gtgf挺\\n悲催fg\"}");
        cout<<result<<endl;
        result = aFileBrowser.parse_command("{\"Command\":\"ReadFile_UTF-8\", \"Name\":\"aFile.txt\"}");
        cout<<result<<endl;
        result = aFileBrowser.parse_command("{\"Command\":\"OpenFolder\", \"Name\":\"TestFold\"}");
        cout<<result<<endl;
        result = aFileBrowser.parse_command("{\"Command\":\"OpenFolder\", \"Name\":\"SubFolder\"}");
        cout<<result<<endl;
        result = aFileBrowser.parse_command("{\"Command\":\"OpenFolder\", \"Name\":\"TestFolder\"}");
        cout<<result<<endl;
        result = aFileBrowser.parse_command("{\"Command\":\"OpenFolder\", \"Name\":\"SubFolder\"}");
        cout<<result<<endl;
        result = aFileBrowser.parse_command("{\"Command\":\"ToParentFolder\"}");
        cout<<result<<endl;
        result = aFileBrowser.parse_command("{\"Command\":\"Show\"}");
        cout<<result<<endl;
        //cout<<"Grand End"<<endl;
    }

    void run_udp_file_browser() {
        FileBrowserUdpListener udpFileBrowser(12345, "/var/InOutOpt");
        udpFileBrowser.start();
        while(1) {
            this_thread::sleep_for(1s);
        }
    }
}