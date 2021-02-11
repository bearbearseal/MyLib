#include "ITC.h"
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

mutex printfLock;

namespace TestITC
{
	struct ThreadData1
	{
		ITC<string>* itc;
		uint16_t port;
	};

	void thread_process1(ThreadData1* data)
	{
		chrono::milliseconds delay(rand() % 100);
		this_thread::sleep_for(delay);
		auto socket = data->itc->create_socket(data->port);
		{
			lock_guard<mutex> lock(printfLock);
			printf("%u: %s\n", data->port, socket == nullptr ? "null" : "not null");
		}
		if (socket != nullptr)
		{
			if (socket->wait_message())
			{
				auto message = socket->get_message();
				{
					lock_guard<mutex> lock(printfLock);
					printf("%lu->%u: %s\n", message.sourceSocketId, data->port, message.message.c_str());
				}
				string reply = "My reply";
				socket->message_to_socket(message.sourceSocketId, reply);
			}
			else
			{
				lock_guard<mutex> lock(printfLock);
				printf("socket's gone.\n");
			}
		}
	}

	struct ThreadData2
	{
		ITC<string>* itc;
		uint16_t myPort;
		uint16_t hisPort;
		uint32_t delay;
	};

	void thread_process2(ThreadData2* data)
	{
		auto socket = data->itc->create_socket(data->myPort);
		chrono::milliseconds delay(rand() % 100);
		this_thread::sleep_for(delay);
		string message = "A message for u.";
		if (!socket->message_to_socket(data->hisPort, message))
		{
			lock_guard<mutex> lock(printfLock);
			printf("%u->%u: socket's gone.\n", data->myPort, data->hisPort);
			return;
		}
		else
		{
			lock_guard<mutex> lock(printfLock);
			printf("%u waiting for reply.\n", data->myPort);
		}
		if (socket->wait_message())
		{
			auto reply = socket->get_message();
			{
				lock_guard<mutex> lock(printfLock);
				printf("%u->%u: %s.\n", data->hisPort, data->myPort, reply.message.c_str());
			}
		}
		else
		{
			lock_guard<mutex> lock(printfLock);
			printf("%u is gone.\n", data->myPort);
		}
	}

	void test_run()
	{
		ITC<string> aItc;
		ThreadData1 threadData1[100];
		thread* threads1[100];
		for (unsigned i = 0; i < 100; ++i)
		{
			//printf("Creating thread 1 %u.\n", i);
			threadData1[i].itc = &aItc;
			threadData1[i].port = i / 2;
			threads1[i] = new thread(thread_process1, &threadData1[i]);
		}
		ThreadData2 threadData2[50];
		thread* threads2[50];
		for (unsigned i = 0; i < 50; ++i)
		{
			//printf("Creating thread 2 %u\n", i);
			threadData2[i].itc = &aItc;
			threadData2[i].hisPort = i;
			threadData2[i].myPort = 100 + i;
			threads2[i] = new thread(thread_process2, &threadData2[i]);
		}
		for (unsigned i = 0; i < 50; ++i)
		{
			//printf("Thread 2 %u joining.\n", i);
			threads2[i]->join();
			delete threads2[i];
			threads2[i] = new thread(thread_process2, &threadData2[i]);
		}
		for (unsigned i = 0; i < 50; ++i)
		{
			//printf("Thread 2 %u joining.\n", i);
			threads2[i]->join();
			delete threads2[i];
		}
		for (unsigned i = 0; i < 100; ++i)
		{
			//printf("Thread 1 %u joining.\n", i);
			threads1[i]->join();
			delete threads1[i];
		}
	}
}
