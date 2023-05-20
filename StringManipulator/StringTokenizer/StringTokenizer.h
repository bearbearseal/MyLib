#ifndef _StringTokenizer_H_
#define _StringTokenizer_H_
#include <string>
//#include <string_view>

class StringTokenizer {
public:
    struct StringToken {
        std::string value;
        std::string deliminator;
        std::string encloser[2];
    };
/*
    struct ViewToken {
        std::string_view value;

    };
*/
    StringTokenizer();
    virtual ~StringTokenizer();

    void add_deliminator(const std::string& deliminator);
    void add_encloser(const std::string& open, std::string& close);

};

#endif