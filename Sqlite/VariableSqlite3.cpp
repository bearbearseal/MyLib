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
    auto result = theSqlite3->execute_query("Select " + column + " from " + tablename + " WHERE rowid=" + to_string(rowid));
    if(result == nullptr)
        return nullptr;
    auto currentValue = result->get_string(0, 0);
    if(!currentValue.has_value())
        return make_shared<Sqlite3Variable>("", preparedStatement);
    return make_shared<Sqlite3Variable>(currentValue.value(), preparedStatement);
}

shared_ptr<VariableSqlite3::Sqlite3Variable> VariableSqlite3::get_variable_integer(const std::string& tablename, int64_t rowid, const std::string& column)
{
    if(theSqlite3 == nullptr)
        return nullptr;
    auto preparedStatement = theSqlite3->create_prepared_statement<string>("Update " + tablename + " SET " + column + "=? WHERE rowid=" + to_string(rowid));
    if(preparedStatement == nullptr)
        return nullptr;
    auto result = theSqlite3->execute_query("Select " + column + " from " + tablename + " WHERE rowid=" + to_string(rowid));
    if(result == nullptr)
        return nullptr;
    auto currentValue = result->get_string(0, 0);
    if(!currentValue.has_value())
        return make_shared<Sqlite3Variable>(0, preparedStatement);
    return make_shared<Sqlite3Variable>(stol(currentValue.value()), preparedStatement);
}

shared_ptr<VariableSqlite3::Sqlite3Variable> VariableSqlite3::get_variable_float(const std::string& tablename, int64_t rowid, const std::string& column)
{
    if(theSqlite3 == nullptr)
        return nullptr;
    auto preparedStatement = theSqlite3->create_prepared_statement<string>("Update " + tablename + " SET " + column + "=? WHERE rowid=" + to_string(rowid));
    if(preparedStatement == nullptr)
        return nullptr;
    auto result = theSqlite3->execute_query("Select " + column + " from " + tablename + " WHERE rowid=" + to_string(rowid));
    if(result == nullptr)
        return nullptr;
    auto currentValue = result->get_string(0, 0);
    if(!currentValue.has_value())
        return make_shared<Sqlite3Variable>(std::numeric_limits<double>::quiet_NaN(), preparedStatement);
    return make_shared<Sqlite3Variable>(stod(currentValue.value()), preparedStatement);
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
    if(preparedStatement->quick_update(newValue.get_string()))
    {
        this->update_value_to_cache(newValue);
        return true;
    }
    return false;
}
