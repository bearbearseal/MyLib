#ifndef _ArgumentExtractor_H_
#define _ArgumentExtractor_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class ArgumentExtractor
{
public:
    const char GROUPER = '-';

    ArgumentExtractor(int argc, char** argv);
    virtual ~ArgumentExtractor();

    //size_t size();
    //const std::string& at(size_t index);
    std::vector<std::string> get_group_argument(const std::string& groupName);
    std::vector<std::string> get_groups();
    bool has_group(const std::string& groupName);
    bool group_has_argument(const std::string& groupName, const std::string& argumentName);

private:
    //std::vector<std::string> arguments;
    std::unordered_map<std::string, std::unordered_set<std::string>> arguments;
};

#endif