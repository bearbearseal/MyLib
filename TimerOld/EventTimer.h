/*
This class time is based on system clock.
Add an event (with listener and token) to timer and tell it the second it should be invoked
The event would be invoked when time is >= the intended time and be removed.
*/

#ifndef _EventTimer_H_
#define _EventTimer_H_
#include <memory>

namespace EventTimer
{
    class Listener
    {
    public:
        Listener() {}
        virtual ~Listener() {}
        virtual void catch_time_event(time_t eventTime, uint32_t token) = 0;
    };
    void time_changed();
    void add_time_event(time_t eventTime, std::weak_ptr<Listener> listener, uint32_t token = 0, time_t interval = 0);
    void start();
    void terminate();
};

#endif