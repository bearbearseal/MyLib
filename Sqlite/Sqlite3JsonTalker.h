#pragma once
#include <string>
#include <mutex>
#include "Sqlite3.h"

class Sqlite3JsonTalker {
private:
    const std::string Command_Query = "Query";
    const std::string Command_Update = "Update";
    const std::string Property_Statement = "Statement";
    const std::string Property_Command = "Command";
public:
    Sqlite3JsonTalker(const std::string& dbName);
    ~Sqlite3JsonTalker();

    std::string execute(const std::string& input);

private:
    std::mutex dbMutex;
    Sqlite3 theDb;
};