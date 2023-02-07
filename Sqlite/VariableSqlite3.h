#ifndef _VariableSqlite3_H_
#define _VariableSqlite3_H_
#include "Sqlite3.h"
#include "../Basic/Variable.h"

class VariableSqlite3
{
private:
    class Sqlite3Variable : public Variable
    {
    public:
        Sqlite3Variable(std::unique_ptr<Sqlite3::PreparedStatement<std::string>>& preparedStatement);
        ~Sqlite3Variable();
        bool write_value(const Value& newValue);
    private:
        std::unique_ptr<Sqlite3::PreparedStatement<std::string>> preparedStatement;
    };
public:
    VariableSqlite3(const std::string& databasename);
    ~VariableSqlite3();
    std::shared_ptr<Sqlite3Variable> get_variable(const std::string& tablename, int64_t rowid, const std::string& column);

private:
    std::unique_ptr<Sqlite3> theSqlite3;
};

#endif