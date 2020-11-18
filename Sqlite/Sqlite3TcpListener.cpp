#include "Sqlite3TcpListener.h"
#include "../../OtherLib/nlohmann/json.hpp"

using namespace std;

Sqlite3TcpListener::Sqlite3TcpListener(uint16_t portNumber, std::shared_ptr<Sqlite3JsonTalker> _sqlite3Db) : TcpListener(portNumber), sqlite3Db(_sqlite3Db) {

}

Sqlite3TcpListener::~Sqlite3TcpListener() {

}

void Sqlite3TcpListener::catch_message(std::string& data, size_t handle) {
	while (data.size()) {
		for (unsigned i = 0; i < data.size(); ++i) {
			if (data[i] == MessageBreak) {
				partialMessage += data.substr(0, i);
				data = data.substr(i + 1);
				if (partialMessage.size() >= MAX_LENGTH) {
					partialMessage.clear();
					break;
				}
                //process partial message
                write_message(handle, sqlite3Db->execute(partialMessage));
				partialMessage.clear();
			}
		}
		partialMessage += data;
		data.clear();
	}
}
