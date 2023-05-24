#ifndef _Trie_H_
#define _Trie_H_

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <memory>
#include <string_view>

class Trie
{
public:
    Trie();
    ~Trie();

    bool search(const std::string &word, size_t index = 0);
    void insert(const std::string &word) { insert(word, word, 0); }
    void insert(const std::string &word, const std::string &value, size_t index = 0);
    std::pair<size_t, std::unordered_set<const std::string *>> find_string(const std::string &word, size_t distance, size_t index = 0);

private:
    std::pair<size_t, std::unordered_set<const std::string *>> find_string_this_character_is_wrong(const std::string &word, size_t distance, size_t index);
    std::pair<size_t, std::unordered_set<const std::string *>> find_string_this_character_is_xtra(const std::string &word, size_t distance, size_t index);
    std::pair<size_t, std::unordered_set<const std::string *>> find_string_this_character_is_missing(const std::string &word, size_t distance, size_t index);
    std::pair<size_t, std::unordered_set<const std::string *>> find_string_this_character_is_switched(const std::string &word, size_t distance, size_t index);

private:
    std::string value;
    std::unordered_map<char, Trie *> children;
};

#endif
