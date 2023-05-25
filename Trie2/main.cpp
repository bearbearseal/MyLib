#include "Trie.h"
#include <fstream>
#include <iostream>

using namespace std;

const pair<string_view, string_view>& break_into_pair(const std::string& input)
{
    static pair<string_view, string_view> retVal;
    retVal.first = input;
    size_t pos = input.find_first_of("=>");
    if(pos == string::npos)
    {
        retVal.second = input;
    }
    else
    {
        retVal.second = retVal.first.substr(pos+2);
        retVal.first = retVal.first.substr(0, pos);
    }
    return retVal;
}

int main(int argc, char* argv[])
{
    Trie aTrie;
    fstream theFile;
    string filename = "SelangorDistricts.txt";
    theFile.open(filename, ios::in);
    while(!theFile.is_open())
    {
        cout<<"Dictionary file name: "<<endl;
        cin>>filename;
        theFile.open(filename, ios::in);
    }
    string aLine;
    while(getline(theFile, aLine))
    {
        auto thePair = break_into_pair(aLine);
        cout<<thePair.first<<"=>"<<thePair.second<<endl;
        aTrie.insert(thePair.first, thePair.second);
    }
    string numberLine;
    int distance;
    while(1)
    {
        do {
            cout<<"Input search string: "<<endl;
            getline(cin,aLine);
        }while(aLine.empty());
        do {
            cout<<"Distance: "<<endl;
            getline(cin, numberLine);
            distance = atoi(numberLine.c_str());
        }while(distance<0 || distance>5);
        cout<<"Searching "<<aLine<<" Distance "<<distance<<endl;
        auto result = aTrie.find_string(aLine, distance);
        printf("Distance %zu: \n", distance - result.first);
        for (auto i = result.second.begin(); i != result.second.end(); ++i)
        {
            printf("\t%s\n", (*i)->c_str());
        }
        aLine.clear();
        numberLine.clear();
    }
}
