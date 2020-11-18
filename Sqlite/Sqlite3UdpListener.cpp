#include "Sqlite3UdpListener.h"
#include "../../OtherLib/nlohmann/json.hpp"

using namespace std;

Sqlite3UdpListener::Sqlite3UdpListener(uint16_t portNumber, std::shared_ptr<Sqlite3JsonTalker> _sqlite3Db) : UdpListener(portNumber), sqlite3Db(_sqlite3Db) {

}

Sqlite3UdpListener::~Sqlite3UdpListener() {

}

void Sqlite3UdpListener::catch_message(std::string& input, const UdpSocket::Address& hisAddress) {
    this->write_message(sqlite3Db->execute(input), hisAddress);
}
