#include "Timer.h"

using namespace std;

class EventCatcher : public Timer::EventCatcher
{
public:
    void catch_timed_event(uint32_t eventId, std::chrono::time_point<std::chrono::steady_clock> eventTime)
    {
        caughtTime.push_back(chrono::steady_clock::now());
    }
    vector<chrono::time_point<chrono::steady_clock>> caughtTime;
};

class DummyEventCatcher : public Timer::EventCatcher
{
    void catch_timed_event(uint32_t eventId, std::chrono::time_point<std::chrono::steady_clock> eventTime) {}
};

int main()
{
    Timer aTimer;
    aTimer.start();
    // Test 1 event repeat 10 times, remove the event after 10th time.
    {
        // Setup
        shared_ptr<EventCatcher> aCatcher = make_shared<EventCatcher>();
        auto interval = 950ms;
        auto startTime = chrono::steady_clock::now() + interval;
        auto theId = aTimer.add_event(aCatcher, startTime, interval);

        // Wait done
        this_thread::sleep_for(10s);
        aTimer.remove_event(theId);
        this_thread::sleep_for(1s);

        // Test Result
        bool passed = true;
        if (aCatcher->caughtTime.size() > 10)
        {
            printf("Failed, Event count %zu\n", aCatcher->caughtTime.size());
            passed = false;
        }
        for (size_t i = 0; i < aCatcher->caughtTime.size(); ++i)
        {
            int64_t diff = chrono::duration_cast<chrono::duration<int64_t, milli>>(aCatcher->caughtTime[i] - startTime - i * interval).count();
            if (diff > 10 || diff < -10)
            {
                printf("Failed, diff: %ld.\n", diff);
                passed = false;
            }
        }
        printf("1 Event 10 Times Test %s\n", passed ? "Passed" : "Failed");
    }

    // Test 10 events run once.
    {
        // Setup
        shared_ptr<EventCatcher> aCatcher = make_shared<EventCatcher>();
        auto interval = 970ms;
        auto startTime = chrono::steady_clock::now() + interval;
        for (size_t i = 0; i < 10; ++i)
        {
            aTimer.add_event(aCatcher, startTime + (i / 2) * interval);
        }

        // Wait done
        this_thread::sleep_for(7s);

        // Test Result
        bool passed = true;
        if (aCatcher->caughtTime.size() > 10)
        {
            printf("Failed, Event count %zu\n", aCatcher->caughtTime.size());
            passed = false;
        }
        for (size_t i = 0; i < aCatcher->caughtTime.size(); ++i)
        {
            int64_t diff = chrono::duration_cast<chrono::duration<int64_t, milli>>(aCatcher->caughtTime[i] - startTime - (i / 2) * interval).count();
            if (diff > 10 || diff < -10)
            {
                printf("Failed, diff: %ld.\n", diff);
                passed = false;
            }
        }
        printf("10 Event 1 Time Test %s\n", passed ? "Passed" : "Failed");
    }

    // Test 1 event repeat 10 times then 10 event run once.
    {
        // Setup
        shared_ptr<EventCatcher> aCatcher = make_shared<EventCatcher>();
        auto interval = 1050ms;
        auto startTime = chrono::steady_clock::now() + interval;
        auto theId = aTimer.add_event(aCatcher, startTime, interval);
        for (size_t i = 0; i < 10; ++i)
        {
            aTimer.add_event(aCatcher, startTime + i * interval);
        }

        // Wait done
        this_thread::sleep_for(11s);
        aTimer.remove_event(theId);
        this_thread::sleep_for(1100ms);

        // Test Result
        bool passed = true;
        if (aCatcher->caughtTime.size() > 20)
        {
            printf("Failed, Event count %zu\n", aCatcher->caughtTime.size());
            passed = false;
        }
        for (size_t i = 0; i < aCatcher->caughtTime.size(); ++i)
        {
            int64_t diff = chrono::duration_cast<chrono::duration<int64_t, milli>>(aCatcher->caughtTime[i] - startTime - (i / 2) * interval).count();
            if (diff > 10 || diff < -10)
            {
                printf("Failed, diff: %ld.\n", diff);
                passed = false;
            }
        }
        printf("1 Event 10 Times + 10 Events 1 Time Test %s\n", passed ? "Passed" : "Failed");
    }

    // Sanity Test
    {
        // Setup
        shared_ptr<DummyEventCatcher> aCatcher = make_shared<DummyEventCatcher>();
        auto interval1 = 61ms;
        auto interval2 = 93ms;
        auto interval3 = 87ms;
        auto startTime = chrono::steady_clock::now() + 1s;
        aTimer.add_event(aCatcher, startTime, interval1);
        auto theId = aTimer.add_event(aCatcher, startTime, interval1);
        vector<Timer::EventId> eventID;

        // Setup and run at the same time
        for (size_t i = 0; i < 200; ++i)
        {
            eventID.emplace_back(aTimer.add_event(aCatcher, startTime + i * interval2));
            this_thread::sleep_for(interval3);
            switch (i % 5)
            {
            case 1:
                aTimer.remove_event(theId);
                break;
            case 3:
                theId = aTimer.add_event(aCatcher, startTime, interval1);
                break;
            default:
                break;
            }
        }
        this_thread::sleep_for(5s);
        aTimer.remove_event(theId);
        // No crash or deadlock!
        printf("Sanity Test passed.\n");
    }
    aTimer.stop();
}