#include "../ArgumentExtractor.h"

using namespace std;

int main(int argc, char **argv)
{
    ArgumentExtractor extractor(argc, argv);
    auto groupList = extractor.get_groups();
    for (size_t i = 0; i < groupList.size(); ++i)
    {
        printf("Group: %s\n", groupList[i].c_str());
        auto argumentList = extractor.get_group_argument(groupList[i]);
        for(size_t j=0; j<argumentList.size(); ++j) {
            printf("\t%s\n", argumentList[j].c_str());
        }
    }
}