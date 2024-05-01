#include "printfter.h"
#include <stdarg.h>

using namespace std;

void Printfter::add_tab()
{
    prefix.append("\t");
}

void Printfter::minus_tab()
{
    prefix.pop_back();
}

void Printfter::clear_tab()
{
    prefix.clear();
}

void Printfter::write(const char* format, ...)
{
    printf("%s", prefix.c_str());
    va_list args;
    va_start(args, format);
    //vsnprintf(temp, sizeof(temp), format, args);
    vprintf(format, args);
    va_end(args);
}
