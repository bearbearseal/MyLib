#ifndef _Trie_H_
#define _Trie_H_

#include <vector>
#include <string>
#include <memory>
#include <string_view>

class Trie {
private:
    enum class Type{
        Empty,
        Intermediate,
        Word  
    };

    class Node {
    public:
        Node();
        void insert_node(std::string_view& toAdd);
        bool remove_node(std::string_view& toRemove);
        bool search_node(std::string_view& toRemove, size_t distance);
    private:
        Type type;
        std::vector<Node> children; 
    };

public:
    Trie();
    ~Trie();

    bool search(const std::string& word, size_t distance = 0);
    void insert(const std::string& word);
    bool remove(const std::string& word);

private:
    std::vector<Node> children;
};

#endif
