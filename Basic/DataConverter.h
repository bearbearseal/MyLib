#ifndef _DataConverter_H_
#define _DataConverter_H_
#include <chrono>
#include <string>
#include <time.h>
#include <string.h>

namespace DataConverter {
    class ChronoSystemTime {
    public:
        ChronoSystemTime(std::chrono::time_point<std::chrono::system_clock> _theTime) {
            theTime = _theTime;
        }
        std::string to_string() const {
            auto milliSec = std::chrono::time_point_cast<std::chrono::milliseconds>(theTime);
            auto msPoint = milliSec.time_since_epoch();
            auto totalms = msPoint.count();
            auto second = totalms/1000;
            struct tm timeStruct = *(localtime(&second));
            char buffer[64];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeStruct);
            size_t length = strlen(buffer);
            sprintf(&buffer[length], ".%03ld", totalms%1000);
            return buffer;
        }
        uint64_t to_milli_second() const {
            auto milliSec = std::chrono::time_point_cast<std::chrono::milliseconds>(theTime);
            auto msPoint = milliSec.time_since_epoch();
            return msPoint.count();
        }
    private:
        std::chrono::time_point<std::chrono::system_clock> theTime;
    };
}

#endif