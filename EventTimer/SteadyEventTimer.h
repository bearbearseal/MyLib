#ifndef _EventTimer_H_
#define _EventTimer_H_
#include <time.h>
#include <stdint.h>
#include <map>
#include <memory>
#include <vector>
#include <thread>
#include "../../MyLib/ITC/ITC.h"

class SteadyEventTimer
{
public:
    class Listener
    {
    public:
        Listener() {}
        virtual ~Listener() {}

        virtual void catch_timed_event(uint64_t token) {}
    };
    SteadyEventTimer();
    virtual ~SteadyEventTimer();

    void terminate();
    void add_timed_event(uint64_t millisecond, std::weak_ptr<Listener> listener, uint64_t token);

private:
    enum class MessageType
    {
        AddTimedEvent,
        Terminate
    };
    struct ListenerData 
    {
        std::weak_ptr<Listener> listener;
        uint64_t token;
    };
    struct Message {
        MessageType type;
        uint64_t theMoment = 0;
        std::weak_ptr<Listener> listener;
        uint64_t token = 0;
    };
    std::unique_ptr<std::thread> theProcess;
    ITC<Message> itc;
    std::unique_ptr<ITC<Message>::FixedSocket> eventSocket;
    std::unique_ptr<ITC<Message>::FixedSocket> threadSocket;
    size_t processState = 0;
    std::map<uint64_t, std::vector<ListenerData>> elapsedTime2ListenerListMap;

    static void thread_process(SteadyEventTimer* me);
    void handle_message(const Message& message);
};

#endif