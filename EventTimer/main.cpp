//Test program for SteadyEventTimer
#include "SteadyEventTimer.h"
#include <random>
#include <unordered_map>

using namespace std;

class TimedEventListener : public SteadyEventTimer::Listener
{
public:
    void catch_timed_event(uint64_t token)
    {
        uint64_t theMoment;
        {
            lock_guard<mutex> lockGuard(mapMutex);
            theMoment = token2TimeMap[token];
            token2TimeMap.erase(token);
        }
        auto now = chrono::time_point_cast<std::chrono::milliseconds>(chrono::system_clock::now()).time_since_epoch();
        auto sysTime = chrono::duration_cast<std::chrono::milliseconds>(now).count();
        printf("%lu, caught event timed at %lu\n", sysTime, theMoment);
    }

    void add_event_pair(uint64_t token, uint64_t millisecond)
    {
        lock_guard<mutex> lockGuard(mapMutex);
        token2TimeMap[token] = millisecond;
    }

private:
    mutex mapMutex;
    unordered_map<uint64_t, uint64_t> token2TimeMap;
};

int main()
{
    uint64_t sequenceNum = 0;
    SteadyEventTimer eventTimer;
    shared_ptr<TimedEventListener> timedEventListener = make_shared<TimedEventListener>();
    while (1)
    {
        uint64_t random = rand() % 10000;
        random += 2000;
        auto now = chrono::time_point_cast<std::chrono::milliseconds>(chrono::system_clock::now()).time_since_epoch();
        auto sysTime = chrono::duration_cast<std::chrono::milliseconds>(now).count();
        eventTimer.add_timed_event( random, timedEventListener, sequenceNum);
        printf("%lu, add event to be call in another %lu\n", sysTime, random);
        ++sequenceNum;
        this_thread::sleep_for(5s);
    }
}