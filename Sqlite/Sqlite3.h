#ifndef _SQLITE3_H_
#define _SQLITE3_H_

#include <sqlite3.h>
#include <variant>
#include <optional>
#include <tuple>
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

    template <typename T, typename... Args>
    class PreparedStatement
    {
        friend class Sqlite3;

    private:
        PreparedStatement() {}

    public:
        ~PreparedStatement() { sqlite3_finalize(stmt); }
        void add_line(T first, Args... rest) { lines.push_back(create_my_tuple(first, rest...)); }
        std::optional<int64_t> commit_insert()
        {
            int rc = sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);
            for (size_t i = 0; i < lines.size(); ++i)
            {
                commit_line(lines[i]);
                rc = sqlite3_step(stmt);
                if (rc != SQLITE_DONE)
                    goto failed_end;
                rc = sqlite3_reset(stmt);
                if (rc != SQLITE_OK)
                    goto failed_end;
            }
            rc = sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL);
            if (rc != SQLITE_OK)
                goto failed_end;
            lines.clear();
            return sqlite3_last_insert_rowid(db);
        failed_end:
            lines.clear();
            return {};
        }
        std::optional<int64_t> quick_update(T first, Args... rest)
        {
            int rc = sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);
            auto aLine = create_my_tuple(first, rest...);
            commit_line(aLine);
            rc = sqlite3_step(stmt);
            if(rc != SQLITE_DONE)
                return {};
            rc = sqlite3_reset(stmt);
            if(rc != SQLITE_OK)
                return {};
            rc = sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, NULL);
            if (rc != SQLITE_OK)
                return {};
            return sqlite3_last_insert_rowid(db);
        }
        inline size_t count_line() { return lines.size(); }

    private:
        template <std::size_t I = 0>
        inline typename std::enable_if<I >= sizeof...(Args) + 1, void>::type
        commit_line(std::tuple<T, Args...> &theTuple, size_t index = 0) {}
        template <std::size_t I = 0>
            inline typename std::enable_if < I<sizeof...(Args) + 1, void>::type
                                             commit_line(std::tuple<T, Args...> &theTuple, size_t index = 0)
        {
            // using type = std::tuple_element_t<I, std::tuple<T, Args...>>;
            do_bind(index + 1, std::get<I>(theTuple));
            commit_line<I + 1>(theTuple, index + 1);
        }
        bool do_bind(size_t index, int64_t item) { return sqlite3_bind_int(stmt, index, item) == SQLITE_OK; }
        bool do_bind(size_t index, double item) { return sqlite3_bind_double(stmt, index, item) == SQLITE_OK; }
        bool do_bind(size_t index, const std::string &item) { return sqlite3_bind_text(stmt, index, item.c_str(), item.size(), NULL) == SQLITE_OK; }
        bool create_prepared_statement(sqlite3 *_db, const std::string &statement)
        {
            db = _db;
            return sqlite3_prepare_v2(db, statement.c_str(), statement.size(), &stmt, NULL) == SQLITE_OK;
        }
        inline std::tuple<T, Args...> create_my_tuple(T first, Args... rest)
        {
            return tuple_cat(std::make_tuple(first), std::make_tuple(rest...));
        }
        sqlite3 *db;
        sqlite3_stmt *stmt;
        std::vector<std::tuple<T, Args...>> lines;
    };

    Sqlite3(const std::string &name);
    ~Sqlite3();
    std::unique_ptr<ResultSet> execute_query(const std::string &query, ...) const;
    bool execute_update(const std::string &update, ...);
    bool execute_atomic_update(const std::vector<std::string> &update);
    // returns last insert id.
    std::optional<uint64_t> execute_insert(const std::string &update, ...);
    template <typename T, typename... Args>
    std::unique_ptr<PreparedStatement<T, Args...>> create_prepared_statement(const std::string &statement)
    {
        std::unique_ptr<PreparedStatement<T, Args...>> retVal(new PreparedStatement<T, Args...>());
        if (retVal->create_prepared_statement(db, statement))
        {
            return retVal;
        }
        return nullptr;
    }

private:
    sqlite3 *db;
};

#endif
