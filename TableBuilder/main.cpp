#include "../File/FileIOer.h"
#include "../ArgumentExtractor/ArgumentExtractor.h"
#include "../../OtherLib/nlohmann/json.hpp"
#include "TableBuilder.h"

using namespace std;

int main(int argc, char *argv[])
{
    ArgumentExtractor arge(argc, argv);
    auto args = arge.get_group_argument("");
    if (args.empty())
        return -1;
    FileIOer jsonFile(args[0]);
    string fileContent = jsonFile.read_data();
    nlohmann::json theJson;
    try
    {
        theJson = nlohmann::json::parse(fileContent);
    }
    catch (nlohmann::json::parse_error &error)
    {
        printf("Parse Failed:\n%s\n", fileContent.c_str());
        return -1;
    }
    if (!theJson.is_array())
    {
        printf("Json is Not an Array:\n%s\n", fileContent.c_str());
        return -1;
    }
    TableBuilder tableBuilder;
    for (size_t i = 0; i < theJson.size(); ++i)
    {
        nlohmann::json &element = theJson[i];
        if (!element.is_object())
        {
            printf("Line %zu is not an object:\n%s\n", i, fileContent.c_str());
            return -1;
        }
        tableBuilder.new_line();
        for (auto j = element.begin(); j != element.end(); ++j)
        {
            tableBuilder.add_column(j.key(), j.value());
        }
    }
    printf("Output:\n%s\n", tableBuilder.build_table("|", "\n").c_str());
}