#include "FileThreadWrite.h"

using namespace std;

FileThreadWrite::FileThreadWrite(const string &_filename) : filename(_filename)
{
    mainSocket = theItc.create_fixed_socket(1, 2);
    threadSocket = theItc.create_fixed_socket(2, 1);
    theProcess = make_unique<thread>(the_process, this);
}

FileThreadWrite::~FileThreadWrite()
{
    mainSocket->send_message({Command::Stop, ""});
    theProcess->join();
}

bool FileThreadWrite::clear()
{
    FILE *file;
#ifdef _WIN32
    fopen_s(&file, filename.c_str(), "w+b");
#else
    file = fopen(filename.c_str(), "w+b");
#endif
    if (file == NULL)
    {
        return false;
    }
    fclose(file);
    return true;
}

void FileThreadWrite::append_data(const string &data)
{
    mainSocket->send_message({Command::Append, data});
}

ssize_t FileThreadWrite::get_last_error()
{
    return lastError.load();
}

void FileThreadWrite::the_process(FileThreadWrite *me)
{
    while (1)
    {
        if (me->threadSocket->wait_message())
        {
            string writeData;
            bool end = false;
            do
            {
                Message message = me->threadSocket->get_message().message;
                switch (message.command)
                {
                case Command::Append:
                {
                    printf("going to append: %s\n", message.data.c_str());
                    writeData += message.data;
                }
                break;
                case Command::Stop:
                    end = true;
                    break;
                }
            } while (me->threadSocket->has_message());
            FILE *file = fopen(me->filename.c_str(), "a+b");
            if (file == NULL)
            {
                printf("Canot append file.\n");
                me->lastError = CannotOpenFile;
            }
            else
            {
                if (fwrite(writeData.c_str(), writeData.size(), 1, file) == writeData.size())
                {
                    me->lastError = WriteError;
                }
                fclose(file);
            }
            if(end)
            {
                printf("Ending.\n");
                return;
            }
        }
    }
}
