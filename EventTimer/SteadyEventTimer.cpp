#include "SteadyEventTimer.h"
#include <chrono>

using namespace std;

SteadyEventTimer::SteadyEventTimer()
{
    eventSocket = itc.create_fixed_socket(1, 2);
    threadSocket = itc.create_fixed_socket(2, 1);
    theProcess = make_unique<thread>(thread_process, this);
}

SteadyEventTimer::~SteadyEventTimer()
{
    Message message;
    message.type = MessageType::Terminate;
    eventSocket->send_message(message);
    //wait for thread to terminate
    theProcess->join();
}

void SteadyEventTimer::add_timed_event(uint64_t millisecond, std::weak_ptr<Listener> listener, uint64_t token)
{
    Message aMessage;
    aMessage.type = MessageType::AddTimedEvent;
    auto theMoment = chrono::time_point_cast<std::chrono::milliseconds>(chrono::steady_clock::now()).time_since_epoch();
    aMessage.theMoment = chrono::duration_cast<std::chrono::milliseconds>(theMoment).count() + millisecond;
    aMessage.token = token;
    aMessage.listener = listener;
    eventSocket->send_message(move(aMessage));
}

void SteadyEventTimer::terminate()
{
    Message aMessage;
    aMessage.type = MessageType::Terminate;
    eventSocket->send_message(move(aMessage));
}

void SteadyEventTimer::thread_process(SteadyEventTimer *me)
{
    while (1)
    {
        switch (me->processState)
        {
        case 0: //Wait message
            me->threadSocket->wait_message();
            me->processState = 10;
        case 10: //Read message
            while (me->threadSocket->has_message())
            {
                auto message = me->threadSocket->get_message();
                Message &theMessage = message.message;
                switch (theMessage.type)
                {
                case MessageType::AddTimedEvent:
                    me->elapsedTime2ListenerListMap[theMessage.theMoment].push_back({theMessage.listener, theMessage.token});
                    break;
                case MessageType::Terminate:
                    return;
                }
            }
            me->processState = 20;
        case 20: //Process event
            if (me->elapsedTime2ListenerListMap.size())
            {
                auto theMoment = chrono::time_point_cast<std::chrono::milliseconds>(chrono::steady_clock::now()).time_since_epoch();
                uint64_t msTheMoment = chrono::duration_cast<std::chrono::milliseconds>(theMoment).count();
                if(me->elapsedTime2ListenerListMap.begin()->first <= msTheMoment) 
                {
                    //Time to fire all the events
                    vector<ListenerData>& listenerList = me->elapsedTime2ListenerListMap.begin()->second;
                    //All listener shall catch the event
                    for(size_t i=0; i<listenerList.size(); ++i)
                    {
                        auto shared = listenerList[i].listener.lock();
                        if(shared != nullptr)
                        {
                            shared->catch_timed_event(listenerList[i].token);
                        }
                    }
                    //Remove head after notifying all listener for that moment.
                    me->elapsedTime2ListenerListMap.erase(me->elapsedTime2ListenerListMap.begin());
                }
                else
                {
                    //The next event havent reach the time yet, delay a while then check if got new event.
                    this_thread::sleep_for(50ms);
                    me->processState = 10;  //Go check i
                }
            }
            else{
                //No more timed event, go to wait message.
                me->processState = 0;
            }
        }
    }
}
