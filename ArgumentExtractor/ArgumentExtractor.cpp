#include "ArgumentExtractor.h"
#include <string.h>

using namespace std;

ArgumentExtractor::ArgumentExtractor(int argc, char** argv)
{
    auto group = &arguments[""];
    for(size_t i=1; i<size_t(argc); ++i)
    {
        if(argv[i][0] == GROUPER && strlen(argv[i])>1)
        {
            group = &arguments[string(argv[i] + 1)];
        }
        else
        {
            group->insert(argv[i]);
        }
    }
}

ArgumentExtractor::~ArgumentExtractor()
{

}

vector<string> ArgumentExtractor::get_groups()
{
    vector<string> retVal;
    for(auto i=arguments.begin(); i!=arguments.end(); ++i) {
        retVal.push_back(i->first);
    }
    return retVal;
}

vector<string> ArgumentExtractor::get_group_argument(const string& groupName)
{
    vector<string> retVal;
    auto theGroup = arguments.find(groupName);
    if(theGroup != arguments.end())
    {
        for(auto i=theGroup->second.begin(); i!=theGroup->second.end(); ++i) {
            retVal.push_back(*i);
        }
    }
    return retVal;
}

bool ArgumentExtractor::has_group(const std::string& groupName)
{
    return (bool) arguments.count(groupName);
}

bool ArgumentExtractor::group_has_argument(const std::string& groupName, const std::string& argumentName)
{
    auto theGroup = arguments.find(groupName);
    if(theGroup!=arguments.end())
    {
        return theGroup->second.count(argumentName);
    }
    return false;
}

/*
size_t ArgumentExtractor::size()
{
    return arguments.size();
}

const std::string& ArgumentExtractor::at(size_t index)
{
    static string empty;
    if(index<arguments.size())
    {
        return arguments[index];
    }
    return empty;
}

const std::vector<std::string>& ArgumentExtractor::get_argument()
{
    return arguments;
}
*/