#include "Trie.h"

using namespace std;

int main()
{
    Trie aTrie;
    aTrie.insert("Kuala Lumpur");
    aTrie.insert("Kuala Selangor");
    aTrie.insert("Kuala Kurau");
    aTrie.insert("Kuala Ku");
    aTrie.insert("Kuala Lipis");
    aTrie.insert("Kuala Lipis mu");
    aTrie.insert("Pantai Remis");
    aTrie.insert("Pantai Redang");
    aTrie.insert("Kluang");
    aTrie.insert("Klang");
    aTrie.insert("Qooala Loompoor");

    {
        auto result = aTrie.find_string("Kuala Se langor", 1);
        printf("%zu: \n", result.first);
        for (auto i = result.second.begin(); i != result.second.end(); ++i)
        {
            printf("\t%s\n", (*i)->c_str());
        }
    }
    {
        auto result = aTrie.find_string("Kuala K", 1);
        printf("%zu: \n", result.first);
        for (auto i = result.second.begin(); i != result.second.end(); ++i)
        {
            printf("\t%s\n", (*i)->c_str());
        }
    }
    {
        auto result = aTrie.find_string("Luala Kumpur", 2);
        printf("%zu: \n", result.first);
        for (auto i = result.second.begin(); i != result.second.end(); ++i)
        {
            printf("\t%s\n", (*i)->c_str());
        }
    }
    {
        auto result = aTrie.find_string("PantaiR emis", 1);
        printf("%zu: \n", result.first);
        for (auto i = result.second.begin(); i != result.second.end(); ++i)
        {
            printf("\t%s\n", (*i)->c_str());
        }
    }
    {
        auto result = aTrie.find_string("Clung", 2);
        printf("%zu: \n", result.first);
        for (auto i = result.second.begin(); i != result.second.end(); ++i)
        {
            printf("\t%s\n", (*i)->c_str());
        }
    }
    {
        auto result = aTrie.find_string("Quala Loompur", 3);
        printf("%zu: \n", result.first);
        for (auto i = result.second.begin(); i != result.second.end(); ++i)
        {
            printf("\t%s\n", (*i)->c_str());
        }
    }
}
