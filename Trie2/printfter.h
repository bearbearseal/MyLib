#ifndef _PRINTFTER_H_
#define _PRINTFTER_H_
#include <stdio.h>
#include <string>

class Printfter
{
public:
    void add_tab();
    void minus_tab();
    void clear_tab();
    void write(const char* format, ...);

private:
    std::string prefix;
};
#endif
