#include "TableBuilder.h"

using namespace std;

TableBuilder::TableBuilder()
{
}

TableBuilder::~TableBuilder()
{

}

void TableBuilder::new_line()
{
    theRows.push_back(std::unordered_map<std::string, std::string>());
}

void TableBuilder::add_column(const std::string& columnName, const std::string& value)
{
    if(theRows.empty())
        theRows.push_back(std::unordered_map<std::string, std::string>());
    theColumns.emplace(columnName);
    theRows[theRows.size()-1][columnName] = value;
}

std::string TableBuilder::build_table(const std::string& columnDeliminator, const std::string& rowBreaker)
{
    string retVal;
    for(auto i=theColumns.begin(); i!=theColumns.end();++i)
    {
        retVal += *i;
        retVal += columnDeliminator;
    }
    retVal += rowBreaker;
    for(size_t i=0;i<theRows.size();++i)
    {
        for(auto j=theColumns.begin(); j!=theColumns.end();++j)
        {
            if(theRows[i].count(*j))
            {
                retVal += theRows[i][*j];
            }
            retVal += columnDeliminator;
        }
        retVal += rowBreaker;
    }
    return retVal;
}