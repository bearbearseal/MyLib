#include "SyncedSerialPort.h"
#include <iostream>

using namespace std;

int main() {
    SyncedSerialPort serialPort;
    serialPort.open("/dev/ttyUSB0", 19200);
    std::string reply = serialPort.write_then_read("0123456789", 10, chrono::seconds(10));
    cout << reply << endl;
}