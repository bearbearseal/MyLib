#include "../File/FileIOer.h"
#include "../ArgumentExtractor/ArgumentExtractor.h"
#include "../../OtherLib/nlohmann/json.hpp"

using namespace std;

class TableBuilder
{
public:
    TableBuilder();
    ~TableBuilder();
};

int main(int argc, char* argv[])
{
    ArgumentExtractor arge(argc, argv);
    auto args = arge.get_group_argument("");
    if(args.empty())
        return -1;
    FileIOer jsonFile(args[0]);
    string fileContent = jsonFile.read_data();
    nlohmann::json theJson;
	try
	{
		theJson = nlohmann::json::parse(fileContent);
        if(!theJson.is_array())
            printf("Json is Not an Array:\n%s\n", fileContent.c_str());
        for(size_t i=0; i<theJson.size(); ++i)
        {
            nlohmann::json& element = theJson[i];
            if(!element.is_object())
                printf("Line %zu is not an object:\n%s\n", i, fileContent.c_str());
        }
	}
	catch (nlohmann::json::parse_error& error)
    {
        printf("Parse Failed:\n%s\n", fileContent.c_str());
    }
}