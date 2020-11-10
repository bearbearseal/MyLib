#pragma once
#include "../TcpSocket/TcpListener.h"
#include "Sqlite3.h"
#include <string>
#include <shared_mutex>

class Sqlite3Listener : public TcpListener {
private:
    const size_t MAX_LENGTH = 1024*4;
    const char MessageBreak = '\n';
    const std::string Command_Query = "Query";
    const std::string Command_Update = "Update";
    const std::string Property_Statement = "Statement";
    const std::string Property_Command = "Command";
public:
    Sqlite3Listener(uint16_t portNumber, const std::string& dbName);
    ~Sqlite3Listener();

private:
	void catch_message(std::string& data, size_t handle);
    std::string process_message(const std::string& input);

private:
    std::mutex dbMutex;
    Sqlite3 theDb;
    std::string partialMessage;
};