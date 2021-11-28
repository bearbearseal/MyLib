#ifndef _FileThreadWrite_H_
#define _FileThreadWrite_H_
#include <atomic>
#include <string>
#include <thread>
#include "../ITC/ITC.h"

class FileThreadWriter
{
public:
    FileThreadWriter(const std::string &_filename);
    virtual ~FileThreadWriter();
    bool clear();
    void append_data(const std::string &data);
    ssize_t get_last_error();

private:
    enum class Command
    {
        Append,
        Stop
    };
    enum Error
    {
        CannotOpenFile,
        WriteError
    };
    struct Message
    {
        Command command = Command::Append;
        std::string data;
    };
    ITC<Message> theItc;
    std::unique_ptr<ITC<Message>::FixedSocket> mainSocket;
    std::unique_ptr<ITC<Message>::FixedSocket> threadSocket;
    const std::string filename;
    std::atomic<ssize_t> lastError = 0;
    std::unique_ptr<std::thread> theProcess = nullptr;

    static void the_process(FileThreadWriter *me);
};

#endif