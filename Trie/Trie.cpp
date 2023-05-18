#include "Trie.h"

using namespace std;

constexpr size_t TheSize = 29;
constexpr size_t AsciiAlphabetCount = 26;

size_t convert_character_to_index(char c)
{
    if (c >= 'a' && c <= 'z')
    {
        return c - 'a';
    }
    else if (c >= 'A' && c <= 'Z')
    {
        return c - 'A';
    }
    else if (c == ' ')
    {
        return AsciiAlphabetCount + 1;
    }
    else if (c == '-')
    {
        return AsciiAlphabetCount + 2;
    }
    return AsciiAlphabetCount + 3;
}

char convert_index_to_character(size_t index)
{
    if (index < 26)
    {
        return 'A' + index;
    }
    else if (index == 27)
    {
        return ' ';
    }
    else if (index == 28)
    {
        return '-';
    }
    return '*';
}

Trie::Trie() : children(TheSize)
{
}

Trie::~Trie()
{
}

bool Trie::search(const string &word, size_t distance)
{
    return true;
}

void Trie::insert(const string &word)
{
    if(!word.empty())
    {
        size_t index = convert_character_to_index(*word.begin());
        string_view toInsert = &word[1];
        children[index].insert_node(toInsert);
    }

}

bool Trie::remove(const string &word)
{
    return true;
}

Trie::Node::Node()
{
    type = Type::Empty;
}

void Trie::Node::insert_node(string_view &toAdd)
{
    if (children.empty())
    {
        children.resize(AsciiAlphabetCount + 3);
    }
    size_t theIndex = convert_character_to_index(*toAdd.begin());
    toAdd.remove_prefix(1);
    if (toAdd.empty())
    {
        type = Type::Word;
    }
    else
    {
        type = Type::Intermediate;
        children[theIndex].insert_node(toAdd);
    }
}

bool Trie::Node::remove_node(string_view &toRemove)
{
    //if this is Word Type
    //  if toRemove has more characters then return false
    //string_view step forward
    //if child search returns true
    //  Remove the child
    //  If got other child or this is a word
    //      Return false
    //  Else return true
    return true;

}

bool Trie::Node::search_node(string_view &toRemove, size_t /*distance*/)
{
    //if this is an Empty Type
    //  return false
    //if toRemove is empty
    //  if this is a Word Type
    //      return true
    //  else
    //      return false
    //return search_node of ++toRemove
    return true;
}
