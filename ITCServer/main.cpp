#include "ItcServer.h"
#include <thread>
#include <string>
#include <random>

using namespace std;

void consumer_process(ItcClient<string, double> &itcClient)
{
    printf("b4 random number.\n");
    std::random_device rd;
    std::mt19937 gen(rd());                        // Mersenne Twister engine
    std::uniform_int_distribution<> dist(1, 100); // Range [1, 100]
    printf("After random number.\n");
    for (size_t i = 0; i < 100; ++i)
    {
        int randomNumber = dist(gen);
        this_thread::sleep_for(randomNumber * 1ms);
        printf("b4 client send.\n");
        itcClient.send_message("Halo " + to_string(i));
        printf("After client send.\n");
        bool gotMessage = itcClient.wait_message(100ms);
        if (!gotMessage)
        {
            continue;
        }
        auto message = itcClient.get_message();
        if (message.has_value())
        {
            printf("Message from Server %f\n", message.value());
        }
    }
}

void producer_process(ItcServer<string, double> &itcServer)
{
    printf("b4 random number.\n");
    std::random_device rd;
    std::mt19937 gen(rd());                        // Mersenne Twister engine
    std::uniform_int_distribution<> dist(10, 1000); // Range [1, 100]
    printf("After random number.\n");
    while (true)
    {
        int randomNumber = dist(gen);
        this_thread::sleep_for(randomNumber * 1ms);
        bool gotMessage = itcServer.wait_message(1000ms);
        if (!gotMessage)
        {
            break;
        }
        auto message = itcServer.get_message();
        do
        {
            printf("Message from Client: %s\n", message.value().message.c_str());
            itcServer.send_message(message.value().sourceId, static_cast<double>(randomNumber));
            message = itcServer.get_message();
        } while (message.has_value());
    }
}

int main()
{
    ItcServer<string, double> itcServer;
    auto client = itcServer.get_client();
    printf("Server has message: %s\n", itcServer.has_message() ? "Yes" : "No");
    auto toServerMessage = itcServer.get_message();
    printf("Server has message: %s\n", toServerMessage.has_value() ? "Yes" : "No");
    printf("Client has message: %s\n", client.has_message() ? "Yes" : "No");
    auto messageFromServer = client.get_message();
    printf("Client has message: %s\n", messageFromServer.has_value() ? "Yes" : "No");
    thread thread1(producer_process, ref(itcServer));
    thread thread2(consumer_process, ref(client));
    thread1.join();
    thread2.join();
}