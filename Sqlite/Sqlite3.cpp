#include <stdarg.h>
#include <string.h>
#include <utility>
#include "Sqlite3.h"

using namespace std;

// const Sqlite3::Exception Sqlite3::SqliteError = {Sqlite3::ErrorCode::SqliteError, "Sqlite Error"};
const Sqlite3::Exception Sqlite3::OpenFailed = {Sqlite3::ErrorCode::OpenFailed, "Open Failed"};
const Sqlite3::Exception Sqlite3::InvalidRow = {Sqlite3::ErrorCode::InvalidRow, "Invalid Row"};
const Sqlite3::Exception Sqlite3::InvalidColumn = {Sqlite3::ErrorCode::InvalidColumn, "Invalid Column"};
const string emptyString("");

Sqlite3::Sqlite3(const std::string &name)
{
    db = nullptr;
    int result = sqlite3_open(name.c_str(), &db);
    if (result)
    {
        throw OpenFailed;
    }
}

Sqlite3::~Sqlite3()
{
    if (db != nullptr)
    {
        sqlite3_close(db);
    }
}

int select_call_back(void *parameter, int argc, char **argv, char **azColName)
{
    tuple<vector<vector<pair<bool, string>>>, unordered_map<string, size_t>, vector<string>> *retVal = (tuple<vector<vector<pair<bool, string>>>, unordered_map<string, size_t>, vector<string>> *)parameter;
    unordered_map<string, size_t> &name2Index = get<1>(*retVal);
    vector<string> &columnName = get<2>(*retVal);
    if (!name2Index.size())
    {
        for (int i = 0; i < argc; ++i)
        {
            name2Index[azColName[i]] = i;
            columnName.push_back(azColName[i]);
        }
    }
    vector<pair<bool, string>> row;
    for (int i = 0; i < argc; ++i)
    {
        if (argv[i] != NULL)
        {
            row.push_back({true, argv[i]});
        }
        else
        {
            row.push_back({false, string()});
        }
    }
    vector<vector<pair<bool, string>>> &rowData = get<0>(*retVal);
    rowData.push_back(move(row));
    return 0;
}

std::unique_ptr<Sqlite3::ResultSet> Sqlite3::execute_query(const string &query, ...) const
{
    char tempBuffer[4096];
    va_list parameters;
    va_start(parameters, query);
    vsprintf(tempBuffer, query.c_str(), parameters);
    va_end(parameters);
    tuple<vector<vector<pair<bool, string>>>, unordered_map<string, size_t>, vector<string>> retVal;
    char *zErrMessage = nullptr;
    int result = sqlite3_exec(db, tempBuffer, select_call_back, &retVal, &zErrMessage);
    if (result != SQLITE_OK)
    {
        printf("Error code: %d\nMessage: %s\n", result, zErrMessage);
        // throw SqliteError;
        // throw Exception(Sqlite3::ErrorCode::SqliteError, zErrMessage, result);
        sqlite3_free(zErrMessage);
        return nullptr;
    }
    return make_unique<ResultSet>(get<0>(retVal), get<1>(retVal), get<2>(retVal));
}

bool Sqlite3::execute_update(const string &update, ...)
{
    char tempBuffer[4096];
    va_list parameters;
    va_start(parameters, update);
    vsprintf(tempBuffer, update.c_str(), parameters);
    va_end(parameters);
    char *zErrMessage = nullptr;
    int result = sqlite3_exec(db, tempBuffer, nullptr, nullptr, &zErrMessage);
    if (result != SQLITE_OK)
    {
        printf("Error code: %d\nMessage: %s\n", result, zErrMessage);
        // throw SqliteError;
        // throw Exception(Sqlite3::ErrorCode::SqliteError, zErrMessage, result);
        sqlite3_free(zErrMessage);
        return false;
    }
    return true;
}

bool Sqlite3::execute_atomic_update(const std::string &update, ...)
{
}

bool Sqlite3::execute_atomic_update(const std::vector<std::string> &update)
{
    int rc = sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);
    char *zErrMessage = nullptr;
    for (size_t i = 0; i < update.size(); ++i)
    {
        rc = sqlite3_exec(db, update[i].c_str(), NULL, NULL, &zErrMessage);
        if (rc != SQLITE_OK)
        {
            printf("Error code: %d\nMessage: %s\n", rc, zErrMessage);
            sqlite3_free(zErrMessage);
            return false;
        }
    }
    if(sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL) == SQLITE_OK)
    {
        return true;
    }
    return false;
}

optional<uint64_t> Sqlite3::execute_insert(const string &update, ...)
{
    char tempBuffer[4096];
    va_list parameters;
    va_start(parameters, update);
    vsprintf(tempBuffer, update.c_str(), parameters);
    va_end(parameters);
    char *zErrMessage = nullptr;
    int result = sqlite3_exec(db, tempBuffer, nullptr, nullptr, &zErrMessage);
    if (result != SQLITE_OK)
    {
        printf("Error code: %d\nMessage: %s\n", result, zErrMessage);
        // throw SqliteError;
        // throw Exception(Sqlite3::ErrorCode::SqliteError, zErrMessage, result);
        sqlite3_free(zErrMessage);
        return {};
    }
    return sqlite3_last_insert_rowid(db);
}

bool do_sql_bind(sqlite3_stmt *stmt, variant<int64_t, double, string> parameter, size_t index)
{
    int result;
    switch (parameter.index())
    {
    case 0:
        printf("Binding as int64_t\n");
        result = sqlite3_bind_int(stmt, index, get<0>(parameter));
        break;
    case 1:
        printf("Binding as double.\n");
        result = sqlite3_bind_double(stmt, index, get<1>(parameter));
        break;
    case 2:
        printf("Beinding as text.\n");
        result = sqlite3_bind_text(stmt, index, get<2>(parameter).c_str(), get<2>(parameter).size(), NULL);
        break;
    }
    if (SQLITE_OK == result)
    {
        return true;
    }
    printf("Binding failed.\n");
    return false;
}

Sqlite3::ResultSet::ResultSet(vector<vector<pair<bool, std::string>>> &_data, unordered_map<string, size_t> &_names, vector<string> &_columnNames)
{
    data = move(_data);
    names = move(_names);
    columnNames = move(_columnNames);
}

Sqlite3::ResultSet::~ResultSet()
{
}

const string &Sqlite3::ResultSet::get_column_name(size_t index) const
{
    if (index >= columnNames.size())
    {
        return emptyString;
    }
    return columnNames[index];
}

pair<bool, const string &> Sqlite3::ResultSet::get_string(size_t row, size_t column) const
{
    if (row > data.size())
    {
        throw InvalidRow;
    }
    if (column > names.size())
    {
        throw InvalidColumn;
    }
    return {data[row][column].first, data[row][column].second};
}

pair<bool, const string &> Sqlite3::ResultSet::get_string(size_t row, const std::string &column) const
{
    if (row > data.size())
    {
        throw InvalidRow;
    }
    auto i = names.find(column);
    if (i == names.end())
    {
        throw InvalidColumn;
    }
    return {data[row][i->second].first, data[row][i->second].second};
}

pair<bool, int64_t> Sqlite3::ResultSet::get_integer(size_t row, size_t column) const
{
    if (row > data.size())
    {
        throw InvalidRow;
    }
    if (column > names.size())
    {
        throw InvalidColumn;
    }
    if (!data[row][column].second.size())
    {
        return {false, 0};
    }
    return {data[row][column].first, stoll(data[row][column].second)};
}

pair<bool, int64_t> Sqlite3::ResultSet::get_integer(size_t row, const std::string &column) const
{
    if (row > data.size())
    {
        throw InvalidRow;
    }
    auto i = names.find(column);
    if (i == names.end())
    {
        throw InvalidColumn;
    }
    if (!data[row][i->second].second.size())
    {
        return {false, 0};
    }
    return {data[row][i->second].first, stoll(data[row][i->second].second)};
}

pair<bool, double> Sqlite3::ResultSet::get_float(size_t row, size_t column) const
{
    if (row > data.size())
    {
        throw InvalidRow;
    }
    if (column > names.size())
    {
        throw InvalidColumn;
    }
    if (!data[row][column].second.size())
    {
        return {false, std::numeric_limits<double>::quiet_NaN()};
    }
    return {data[row][column].first, stod(data[row][column].second)};
}

pair<bool, double> Sqlite3::ResultSet::get_float(size_t row, const std::string &column) const
{
    if (row > data.size())
    {
        throw InvalidRow;
    }
    auto i = names.find(column);
    if (i == names.end())
    {
        throw InvalidColumn;
    }
    if (!data[row][i->second].second.size())
    {
        return {false, std::numeric_limits<double>::quiet_NaN()};
    }
    return {data[row][i->second].first, stod(data[row][i->second].second)};
}
