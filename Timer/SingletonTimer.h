/*
This class time is based on system clock.
Add an event (with listener and token) to timer and tell it the second it should be invoked
The event would be invoked when time is >= the intended time and be removed.
*/

#ifndef _SingletonTimer_H_
#define _SingletonTimer_H_
#include "Timer.h"

namespace SingletonTimer
{
    static std::shared_ptr<Timer> get_singleton();
    static void time_changed();
    static void add_time_event(time_t eventTime, std::weak_ptr<Timer::Listener> listener, uint32_t token);
    static void start();
    static void terminate();
};

#endif