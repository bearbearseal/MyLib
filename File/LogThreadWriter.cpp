#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <stdarg.h>
#include "LogThreadWriter.h"

using namespace std;

LogThreadWriter::LogThreadWriter(const std::string& filename) : FileThreadWriter(filename)
{
}

LogThreadWriter::~LogThreadWriter()
{
}

void LogThreadWriter::add_log(const std::string& data, ...)
{
    //vsprintf
    va_list va;
    va_start(va, data);
    char buffer[8192];
    auto now = chrono::system_clock::now();
    auto secondTime = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&secondTime), "%Y-%m-%d %X: ");
    size_t pos = snprintf(buffer, sizeof(buffer), "%s", ss.str().c_str());
    pos += vsnprintf(&buffer[pos], sizeof(buffer)-pos, data.c_str(), va);
    buffer[pos] = '\n';
    this->append_data(buffer);
    va_end(va);
}
