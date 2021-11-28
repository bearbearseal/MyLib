#ifndef _LogThreadWriter_H_
#define _LogThreadWriter_H_
#include "FileThreadWriter.h"

class LogThreadWriter : public FileThreadWriter
{
public:
    LogThreadWriter(const std::string& filename);
    ~LogThreadWriter();
    void add_log(const std::string& format, ...);
};

#endif