#pragma once
#include "../UdpSocket/UdpListener.h"
#include "Sqlite3JsonTalker.h"
#include "Sqlite3.h"
#include <string>
#include <mutex>
#include <memory>

class Sqlite3UdpListener : public UdpListener {
private:
    const std::string Command_Query = "Query";
    const std::string Command_Update = "Update";
    const std::string Property_Statement = "Statement";
    const std::string Property_Command = "Command";
public:
    Sqlite3UdpListener(uint16_t portNumber, std::shared_ptr<Sqlite3JsonTalker> _sqlite3Db);
    ~Sqlite3UdpListener();

private:
    void catch_message(std::string& data, const UdpSocket::Address& hisAddress);

private:
    std::shared_ptr<Sqlite3JsonTalker> sqlite3Db;
};