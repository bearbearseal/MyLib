#include "VariableSqlite3.h"

using namespace std;

VariableSqlite3::Sqlite3Variable::Sqlite3Variable(shared_ptr<Sqlite3> _parent, int64_t _rowid, const string& _column)
    : parent(_parent), rowid(_rowid), column(_column)
{

}

VariableSqlite3::Sqlite3Variable::~Sqlite3Variable()
{

}

bool VariableSqlite3::Sqlite3Variable::write_value(const Value& newValue)
{
    auto shared = parent.lock();
    if(shared != nullptr)
    {
        shared->execute_update()
    }
}

VariableSqlite3::VariableSqlite3(const std::string& databasename)
{
    sqlite3 = make_shared<Sqlite3>(databasename);
}

VariableSqlite3::~VariableSqlite3()
{

}

std::shared_ptr<VariableSqlite3::Sqlite3Variable> VariableSqlite3::get_variable(int64_t rowid, const std::string& column)
{
    return make_shared<Sqlite3Variable>(sqlite3, rowid, column);
}
