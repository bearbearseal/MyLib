/****************************************************************************************
* This is a test to verify if the socket would be signaled when the ITC is destructed.	*
****************************************************************************************/
#pragma once
#include "ITC.h"
#include <thread>
#include <memory>
#include <atomic>

using namespace std;

namespace TestDestroyItc {
	atomic<bool> terminate;

	void access_socket(shared_ptr<ITC<size_t>::Socket>* socket) {
		while (!terminate.load()) {
			(*socket)->get_message();
		}
	}

	void run() {
		shared_ptr<ITC<size_t>> anItc = make_shared<ITC<size_t>>();
		auto socket = anItc->create_socket(1);
		terminate.store(false);
		shared_ptr<thread> theThread = make_shared<thread>(access_socket, &socket);
		printf("Press anykey to kill destroy the itc.\n");
		getchar();
		//Now, delete the ITC before deleting the socket.
		anItc.reset();
		printf("U should c this line if the program hasnt crash.\n");
		terminate.store(true);
		printf("Press anykey to continue.\n");
		theThread->join();
		getchar();
	}
}
