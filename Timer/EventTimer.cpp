#include "EventTimer.h"
#include "../../MyLib/ITC/ITC.h"
#include <map>
#include <vector>
#include <stdio.h>

using namespace std;

enum class Command
{
    AddEvent,
    TimeChanged,
    Terminate
};
struct ListenerData
{
    std::weak_ptr<EventTimer::Listener> listener;
    time_t interval;
    uint32_t token;
};
std::map<time_t, std::vector<ListenerData>> second2ListenerListMap;

struct Message
{
    Command command;
    time_t eventTime;
    std::weak_ptr<EventTimer::Listener> listener;
    time_t interval;
    uint32_t token;
};

std::unique_ptr<std::thread> theProcess;
ITC<Message> itc;
std::unique_ptr<ITC<Message>::FixedSocket> eventSocket;
std::unique_ptr<ITC<Message>::FixedSocket> threadSocket;
size_t processState = 0;
std::mutex startMutex;
bool started = false;

void thread_process();

void EventTimer::start()
{
    lock_guard<mutex> lock(startMutex);
    if (!started)
    {
        eventSocket = itc.create_fixed_socket(1, 2);
        threadSocket = itc.create_fixed_socket(2, 1);
        theProcess = make_unique<thread>(thread_process);
        started = true;
    }
}

void EventTimer::time_changed()
{
    Message message;
    message.command = Command::TimeChanged;
    eventSocket->send_message(message);
}

void EventTimer::add_time_event(time_t eventTime, std::weak_ptr<EventTimer::Listener> listener, uint32_t token, time_t interval)
{
    Message message;
    message.command = Command::AddEvent;
    message.eventTime = eventTime;
    message.listener = listener;
    message.token = token;
    message.interval = interval;
    eventSocket->send_message(message);
}

void EventTimer::terminate()
{
    Message message;
    message.command = Command::Terminate;
    eventSocket->send_message(message);
    theProcess->join();
}

void thread_process()
{
    while (1)
    {
        switch (processState)
        {
        case 0: // Initial State
            threadSocket->wait_message();
            processState = 10;
            break;

        case 10: // Read message
        {
            // time_t secondNow = chrono::system_clock::to_time_t(chrono::system_clock::now());
            while (threadSocket->has_message())
            {
                auto message = threadSocket->get_message();
                switch (message.message.command)
                {
                case Command::AddEvent:
                    printf("Adding event at %ld\n", message.message.eventTime);
                    second2ListenerListMap[message.message.eventTime].push_back({message.message.listener, message.message.interval, message.message.token});
                    break;
                case Command::TimeChanged:
                    break;
                case Command::Terminate:
                default:
                    return;
                }
            }
            printf("No more message.\n");
            processState = 20;
            break;
        }
        case 20: // Wait for event
        {
            printf("Wait for event state.\n");
            auto firstEntry = second2ListenerListMap.begin();
            if (firstEntry == second2ListenerListMap.end())
            {
                processState = 0; // wait for message
            }
            else
            {
                time_t secondNow = chrono::system_clock::to_time_t(chrono::system_clock::now());
                time_t eventSecond = firstEntry->first;
                if (eventSecond <= secondNow)
                {
                    for (size_t i = 0; i < firstEntry->second.size(); ++i)
                    {
                        ListenerData &currentListener = firstEntry->second[i];
                        auto shared = currentListener.listener.lock();
                        if (shared != nullptr)
                        {
                            shared->catch_time_event(eventSecond, currentListener.token);
                            if (currentListener.interval)
                            {
                                // add to list again
                                second2ListenerListMap[eventSecond + currentListener.interval].push_back({currentListener.listener, currentListener.interval, currentListener.token});
                            }
                        }
                    }
                    second2ListenerListMap.erase(firstEntry);
                }
                else
                {
                    // printf("Going to wait %lu seconds for event.\n", eventSecond - secondNow);
                    // printf("Event time %lu now %lu\n", eventSecond, secondNow);
                    threadSocket->wait_message(chrono::seconds(eventSecond - secondNow));
                    if (threadSocket->has_message())
                    {
                        processState = 10; // Get message
                    }
                }
            }
            break;
        }
        }
    }
}
