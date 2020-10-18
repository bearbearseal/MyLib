#include "PrioritizedValue.h"
#include "HashKey.h"
#include <iostream>
#include <unordered_set>

using namespace std;

int main() {
    /*
    PrioritizedValue pValue;
    pValue.set_value(1, 2);
    cout<<pValue.get_value().get_int()<<endl;
    pValue.set_value(2, 3);
    cout<<pValue.get_value().get_int()<<endl;
    pValue.set_value(3, 5);
    cout<<pValue.get_value().get_int()<<endl;
    pValue.set_value(1, 6);
    cout<<pValue.get_value().get_int()<<endl;
    pValue.unset_value(3);
    cout<<pValue.get_value().get_int()<<endl;
    pValue.unset_value(2);
    cout<<pValue.get_value().get_int()<<endl;
    pValue.set_value(3, 9);
    cout<<pValue.get_value().get_int()<<endl;
    pValue.trigger_value(20);
    cout<<pValue.get_value().get_int()<<endl;
    pValue.set_value(0, 9);
    cout<<pValue.get_value().get_int()<<endl;
    */
    unordered_set<HashKey::EitherKey, HashKey::EitherKeyHash> aSet;
    aSet.emplace(123);
    aSet.emplace("ABC");
    printf("%lu\n", aSet.count(1));
    printf("%lu\n", aSet.count(123));
    printf("%lu\n", aSet.count("54fd"));
    printf("%lu\n", aSet.count("ABC"));

    unordered_set<HashKey::DualKey, HashKey::DualKeyHash> set2;
    set2.emplace(123, "123");
    set2.emplace("123", "123");
    set2.emplace("abc", 888);
    printf("%lu\n", set2.count({123, "123"}));
    printf("%lu\n", set2.count({"123", 123}));
    printf("%lu\n", set2.count({"abc", 888}));
    printf("%lu\n", set2.count({123, 123}));    
    printf("%lu\n", set2.count({"123", "123"}));    
}