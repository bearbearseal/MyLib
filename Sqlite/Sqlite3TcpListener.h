#pragma once
#include "../TcpSocket/TcpListener.h"
#include "Sqlite3JsonTalker.h"
#include "Sqlite3.h"
#include <string>
#include <mutex>
#include <memory>

class Sqlite3TcpListener : public TcpListener {
private:
    const size_t MAX_LENGTH = 1024*4;
    const char MessageBreak = '\n';
public:
    Sqlite3TcpListener(uint16_t portNumber, std::shared_ptr<Sqlite3JsonTalker> _sqlite3Db);
    ~Sqlite3TcpListener();

private:
	void catch_message(std::string& data, size_t handle);

private:
    std::mutex dbMutex;
    std::shared_ptr<Sqlite3JsonTalker> sqlite3Db;
    std::string partialMessage;
};