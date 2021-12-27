#include "SingletonTimer.h"
#include <map>
#include <vector>

using namespace std;

SingletonTimer::SingletonTimer()
{
    //Start thread in wait for event mode
}

SingletonTimer::~SingletonTimer()
{
    //Stop the thread
}

std::shared_ptr<SingletonTimer> SingletonTimer::get_singleton()
{

}

void SingletonTimer::time_changed()
{
}

void SingletonTimer::add_time_event(time_t eventTime, std::weak_ptr<Listener> listener, uint32_t token)
{
}

void SingletonTimer::handle_add_event(const Message& message)
{

}

void SingletonTimer::thread_process(SingletonTimer* me)
{

}
