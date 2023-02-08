#include "VariableSqlite3.h"

using namespace std;

VariableSqlite3::VariableSqlite3(const string& databasename)
{
    theSqlite3 = Sqlite3::open_database(databasename);
}

VariableSqlite3::~VariableSqlite3()
{

}

shared_ptr<VariableSqlite3::Sqlite3Variable> VariableSqlite3::get_variable(const string& tablename, int64_t rowid, const string& column)
{
    if(theSqlite3 == nullptr)
        return nullptr;
    auto preparedStatement = theSqlite3->create_prepared_statement<string>("Update " + tablename + " SET " + column + "=? WHERE rowid=" + to_string(rowid));

    if(preparedStatement == nullptr)
        return nullptr;
    return make_shared<Sqlite3Variable>(0, preparedStatement);
}

VariableSqlite3::Sqlite3Variable::Sqlite3Variable(const Value& initValue, std::unique_ptr<Sqlite3::PreparedStatement<std::string>>& _preparedStatement)
{
    preparedStatement = move(_preparedStatement);
    this->update_value_to_cache(initValue);
}

VariableSqlite3::Sqlite3Variable::~Sqlite3Variable()
{

}

bool VariableSqlite3::Sqlite3Variable::write_value(const Value& newValue)
{
    return preparedStatement->quick_update(newValue.get_string());
}
