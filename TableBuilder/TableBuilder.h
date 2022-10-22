#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class TableBuilder
{
public:
    TableBuilder();
    ~TableBuilder();
    void new_line();
    void add_column(const std::string& columnName, const std::string& value);
    std::string build_table(const std::string& columnDeliminator, const std::string& rowBreaker);

private:
    std::vector<std::unordered_map<std::string, std::string>> theRows;
    std::unordered_set<std::string> theColumns;
};