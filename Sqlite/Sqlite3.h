#ifndef _SQLITE3_H_
#define _SQLITE3_H_

#include <sqlite3.h>
#include <variant>
#include <optional>
#include <memory>
#include <vector>
#include <unordered_map>

class Sqlite3
{
public:
    enum class ErrorCode
    {
        SqliteError = 1,
        OpenFailed = 2,
        InvalidRow = 3,
        InvalidColumn = 4
    };
    class Exception
    {
    public:
        Exception(ErrorCode _code, const std::string &_message, int _subCode = 0) : code(_code), subCode(_subCode), message(_message) {}
        const ErrorCode code;
        const int subCode;
        const std::string message;
    };
    static const Exception SqliteError;
    static const Exception OpenFailed;
    static const Exception InvalidRow;
    static const Exception InvalidColumn;

    class ResultSet
    {
    public:
        ResultSet(std::vector<std::vector<std::pair<bool, std::string>>> &data, std::unordered_map<std::string, size_t> &names, std::vector<std::string> &columnNames);
        ~ResultSet();
        std::pair<bool, const std::string &> get_string(size_t row, size_t column) const;
        std::pair<bool, const std::string &> get_string(size_t row, const std::string &column) const;
        std::pair<bool, int64_t> get_integer(size_t row, size_t column) const;
        std::pair<bool, int64_t> get_integer(size_t row, const std::string &column) const;
        std::pair<bool, double> get_float(size_t row, size_t column) const;
        std::pair<bool, double> get_float(size_t row, const std::string &column) const;
        inline size_t get_row_count() const { return data.size(); }
        inline size_t get_column_count() const { return names.size(); }
        const std::string &get_column_name(size_t index) const;

    private:
        std::vector<std::vector<std::pair<bool, std::string>>> data;
        std::unordered_map<std::string, size_t> names;
        std::vector<std::string> columnNames;
    };

    Sqlite3(const std::string &name);
    ~Sqlite3();
    std::unique_ptr<ResultSet> execute_query(const std::string &query, ...) const;
    bool execute_update(const std::string &update, ...);
    // returns last insert id.
    std::optional<uint64_t> execute_insert(const std::string &update, ...);
    std::optional<uint64_t> execute_bulk_insert(const std::string &statement, const std::vector<std::vector<std::variant<int64_t, double, std::string>>>& parameters);

private:
    sqlite3 *db;
};

#endif
