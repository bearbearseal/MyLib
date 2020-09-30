#pragma once
#include "AsymmetricITC.h"
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

mutex printfLock;

namespace TestAsITC
{
	struct ThreadData1
	{
		AsymmetricITC<string, wstring>* itc;
		uint16_t port;
	};

	void thread_process1(ThreadData1* data)
	{
		chrono::milliseconds delay(rand() / 100);
		this_thread::sleep_for(delay);
		auto socket = data->itc->create_socket1(data->port);
		{
			lock_guard<mutex> lock(printfLock);
			printf("%u: %s\n", data->port, socket == nullptr ? "null" : "not null");
		}
		if (socket != nullptr)
		{
			if (socket->wait_message())
			{
				auto message = socket->get_message();
				if (message.first)
				{
					lock_guard<mutex> lock(printfLock);
					printf("%u->%u: %s\n", message.second.sourceSocketId, data->port, message.second.message.c_str());
				}
				else
				{
					lock_guard<mutex> lock(printfLock);
					printf("%u read failed.\n", message.second.sourceSocketId);
				}
				wstring reply = L"My reply";
				socket->message_to_socket(message.second.sourceSocketId, reply);
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
		AsymmetricITC<string, wstring>* itc;
		uint16_t myPort;
		uint16_t hisPort;
		uint32_t delay;
	};

	void thread_process2(ThreadData2* data)
	{
		auto socket = data->itc->create_socket2(data->myPort);
		chrono::milliseconds delay(rand() / 100);
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
			if (reply.first)
			{
				lock_guard<mutex> lock(printfLock);
				wprintf(L"%u->%u: %s.\n", data->hisPort, data->myPort, reply.second.message.c_str());
			}
			else
			{
				lock_guard<mutex> lock(printfLock);
				printf("I also dunno y said got message then no more, delete happened in between?\n");
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
		AsymmetricITC<string, wstring> aItc;
		ThreadData1 threadData1[100];
		thread* threads1[100];
		for (unsigned i = 0; i < 100; ++i)
		{
			threadData1[i].itc = &aItc;
			threadData1[i].port = i / 2;
			threads1[i] = new thread(thread_process1, &threadData1[i]);
		}
		ThreadData2 threadData2[50];
		thread* threads2[50];
		for (unsigned i = 0; i < 50; ++i)
		{
			threadData2[i].itc = &aItc;
			threadData2[i].hisPort = i;
			threadData2[i].myPort = 100 + i;
			threads2[i] = new thread(thread_process2, &threadData2[i]);
		}
		for (unsigned i = 0; i < 50; ++i)
		{
			threads2[i]->join();
			delete threads2[i];
			threads2[i] = new thread(thread_process2, &threadData2[i]);
		}
		for (unsigned i = 0; i < 50; ++i)
		{
			threads2[i]->join();
			delete threads2[i];
		}
		for (unsigned i = 0; i < 100; ++i)
		{
			threads1[i]->join();
			delete threads1[i];
		}
		system("PAUSE");
	}
}
