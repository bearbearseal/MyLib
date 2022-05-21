#include "ServerClientItc.h"
#include <string>
#include <thread>

using namespace std;

void thread_process_1(ServerClientItc<int, string>* itc)
{
    auto serverSocket = itc->get_server_socket();
    while(1)
    {
        this_thread::sleep_for(200ms);
        chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds >(chrono::system_clock::now().time_since_epoch());
        printf("Server socket wait message at %zu.\n", ms.count());
        serverSocket->wait_message(300ms);
        ms = chrono::duration_cast<chrono::milliseconds >(chrono::system_clock::now().time_since_epoch());
        printf("Server socket check message at %zu.\n", ms.count());
        auto messagePair = serverSocket->get_message();
        if(messagePair.has_value())
        {
            printf("Server socket message: %d, Source: %zu\n", messagePair.value().message, messagePair.value().sourceId.id);
            this_thread::sleep_for(500ms);
            ms = chrono::duration_cast<chrono::milliseconds >(chrono::system_clock::now().time_since_epoch());
            printf("Server socket send message at %zu\n", ms.count());
            if(messagePair.value().message > 0)
            {
                serverSocket->send_message(messagePair.value().sourceId, "Positive value.\n");
            }
            else
            {
                serverSocket->send_message(messagePair.value().sourceId, "Negative value.\n");
            }
        }
        else
        {
            printf("Server socket time out.\n");
        }
    }
}

void thread_process_2(ServerClientItc<int, string>* itc)
{
    size_t anInt = 10;
    while(1)
    {
        auto clientSocket = itc->get_client_socket();
        ++anInt;
        this_thread::sleep_for(2s);
        chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds >(chrono::system_clock::now().time_since_epoch());
        printf("Client socket 1 send message at %zu.\n", ms.count());
        clientSocket->send_message(anInt);
        printf("Client socket 1 wait message.\n");
        clientSocket->wait_message();
        ms = chrono::duration_cast<chrono::milliseconds >(chrono::system_clock::now().time_since_epoch());
        printf("Client socket 1 got message at %zu.\n", ms.count());
        auto message = clientSocket->get_message();
        if(message.has_value())
        {
            printf("Client socket 1 message: %s\n", message.value().c_str());
        }
    }
}

void thread_process_3(ServerClientItc<int, string>* itc)
{
    int anInt = -10;
    while(1)
    {
        auto clientSocket = itc->get_client_socket();
        --anInt;
        this_thread::sleep_for(3s);
        chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds >(chrono::system_clock::now().time_since_epoch());
        printf("Client socket 2 send message at %zu.\n", ms.count());
        clientSocket->send_message(anInt);
        printf("Client socket 2 wait message.\n");
        clientSocket->wait_message();
        ms = chrono::duration_cast<chrono::milliseconds >(chrono::system_clock::now().time_since_epoch());
        printf("Client socket 2 got message at %zu.\n", ms.count());
        auto message = clientSocket->get_message();
        if(message.has_value())
        {
            printf("Client socket 2 message: %s\n", message.value().c_str());
        }
    }
}

int main()
{
    ServerClientItc<int, string> itc;
    //auto something = ServerClientItc<int, int>::ClientData();
    //itc.clientsData.clientData.push_back();
    thread thread1(thread_process_1, &itc);
    thread thread2(thread_process_2, &itc);
    thread thread3(thread_process_3, &itc);
    while(1)
    {
        this_thread::sleep_for(1s);
    }
}
