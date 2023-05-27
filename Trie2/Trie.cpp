#include "Trie.h"

using namespace std;

Trie::Trie()
{
}

Trie::~Trie()
{
}

bool Trie::search(const std::string &word, size_t index)
{
    return true;
}

void Trie::insert(string_view &word, const string_view &newValue)
{
    if(word.empty())
    {
        value = newValue;
    }
    else
    {
        auto theChild = children.find(*word.begin());
        if (theChild == children.end())
        {
            //printf("Add node of %c as child\n", word.at(index));
            theChild = children.emplace(*word.begin(), new Trie()).first;
        }
        //printf("Insert child add %s\n", &word[index+1]);
        word.remove_prefix(1);
        theChild->second->insert(word, newValue);
    }
}

void Trie::insert(const std::string &word, const std::string& theValue, size_t index)
{
    if (index >= word.size())
    {
        //printf("Done, index: %zu word size: %zu\n", index, word.size());
        value = theValue;
    }
    else
    {
        auto theChild = children.find(word.at(index));
        if (theChild == children.end())
        {
            //printf("Add node of %c as child\n", word.at(index));
            theChild = children.emplace(word.at(index), new Trie()).first;
        }
        //printf("Insert child add %s\n", &word[index+1]);
        theChild->second->insert(word, theValue, index + 1);
    }
}

// bigger number at pair.first is the better
void combine_best_to_result1(pair<size_t, unordered_set<const string*>> &result1, pair<size_t, unordered_set<const string*>> &result2)
{
    if (result1.second.empty())
    {
        if (!result2.second.empty())
        {
            result1 = move(result2);
        }
    }
    else if (!result1.second.empty() && !result2.second.empty())
    {
        if (result1.first == result2.first)
        {
            result1.second.insert(result2.second.begin(), result2.second.end());
        }
        else if (result2.first > result1.first)
        {
            result1 = move(result2);
        }
    }
}

pair<size_t, unordered_set<const string*>> Trie::find_string(const string &word, size_t distance, size_t index)
{
    // Already check to the last character of the word
    if (index >= word.size())
    {
        if (!value.empty())
        {
            return {distance, {&value}};
        }
        else if (distance == 0)
        {
            // if distance is 0 then no need to go further
            return {0, {}};
        }
        // this value is empty, it would definitely have children
        // get the closet match of each child and put them into a list
        // return the list
        pair<size_t, unordered_set<const string*>> result;
        for (auto i = children.begin(); i != children.end(); ++i)
        {
            auto singleResult = i->second->find_string(word, distance - 1, index);
            combine_best_to_result1(result, singleResult);
        }
        return result;
    }
    pair<size_t, unordered_set<const string*>> result;
    pair<size_t, unordered_set<const string*>> singleResult;
    auto theChild = children.find(word.at(index));
    if (theChild != children.end())
    {
        singleResult = theChild->second->find_string(word, distance, index+1);
        combine_best_to_result1(result, singleResult);
        //distance = distance - singleResult.first;
    }
    if (distance > 0)
    {
        // Find result from if next character is a wrong
        singleResult = find_string_this_character_is_wrong(word, distance, index); 
        combine_best_to_result1(result, singleResult);
        distance = distance - singleResult.first;
    }
    if (distance > 0)
    {
        // Find result from if this character is an xtra
        singleResult = find_string_this_character_is_xtra(word, distance, index);
        combine_best_to_result1(result, singleResult);
        distance = distance - singleResult.first;
    }
    if (distance > 0)
    {
        // Find result from if next character is missing
        singleResult = find_string_this_character_is_missing(word, distance, index);
        combine_best_to_result1(result, singleResult);
        distance = distance - singleResult.first;
    }
    if (distance > 0)
    {

        // Find result from if next and next next characters switch position
        singleResult = find_string_this_character_is_switched(word, distance, index);
        combine_best_to_result1(result, singleResult);
        distance = distance - singleResult.first;
    }
    return result;
}

pair<size_t, unordered_set<const string*>> Trie::find_string_this_character_is_wrong(const std::string &word, size_t distance, size_t index)
{
    ++index;
    --distance;
    // All children find string and combine best
    pair<size_t, unordered_set<const string*>> result;
    for (auto i = children.begin(); i != children.end(); ++i)
    {
        auto singleResult = i->second->find_string(word, distance, index);
        combine_best_to_result1(result, singleResult);
    }
    return result;
}

pair<size_t, unordered_set<const std::string*>> Trie::find_string_this_character_is_xtra(const std::string &word, size_t distance, size_t index)
{
    ++index;
    --distance;
    return find_string(word, distance, index);
}

pair<size_t, unordered_set<const std::string*>> Trie::find_string_this_character_is_missing(const std::string &word, size_t distance, size_t index)
{
    --distance;
    // All children find string and combine best
    pair<size_t, unordered_set<const std::string*>> result;
    for (auto i = children.begin(); i != children.end(); ++i)
    {
        auto singleResult = i->second->find_string(word, distance, index);
        combine_best_to_result1(result, singleResult);
    }
    return result;
}

pair<size_t, unordered_set<const string*>> Trie::find_string_this_character_is_switched(const std::string &word, size_t distance, size_t index)
{
    if (word.size() - index < 2)
    {
        return {0, {}};
    }
    auto nextChar = word[index];
    auto nexNextChar = word[index + 1];
    // find next node with next char
    auto nextChild = children.find(nexNextChar);
    // find next node gain with previous char
    if (nextChild == children.end())
    {
        return {0, {}};
    }
    auto nextNextChild = nextChild->second->children.find(nextChar);
    if(nextNextChild == nextChild->second->children.end())
    {
        return {0, {}};
    }
    --distance;
    index+=2;
    return nextNextChild->second->find_string(word, distance, index);
}
