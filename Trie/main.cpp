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

    string string1("ghtr");
    printf("Search %s: %s\n", string1.c_str(), aTrie.search(string1) ? "found" : "not found");

    string string2("Kuala Kurau");
    printf("Search %s: %s\n", string2.c_str(), aTrie.search(string2) ? "found" : "not found");

    string string3("Kuala");
    printf("Search %s: %s\n", string3.c_str(), aTrie.search(string3) ? "found" : "not found");
    
    string string4("Kuala Lumpur");
    printf("Search %s: %s\n", string4.c_str(), aTrie.search(string4) ? "found" : "not found");
    
    string string5("Kuala Ku");
    printf("Search %s: %s\n", string5.c_str(), aTrie.search(string5) ? "found" : "not found");
    
    string string6("Kuala Lipis");
    printf("Search %s: %s\n", string6.c_str(), aTrie.search(string6) ? "found" : "not found");
    
    string string7("Kuala Lipis Mu");
    printf("Search %s: %s\n", string7.c_str(), aTrie.search(string7) ? "found" : "not found");

    string string8("Pantai Re");
    printf("Search %s: %s\n", string8.c_str(), aTrie.search(string8) ? "found" : "not found");
    
    string string9("Pantai Remis");
    printf("Search %s: %s\n", string9.c_str(), aTrie.search(string9) ? "found" : "not found");
    
}
