#pragma once
#ifndef _ASYMMETRICITC_H_
#define _ASYMMETRICITC_H_
#include <unordered_map>
#include <memory>
#include <tuple>
#include <string>
#include <mutex>
#include <shared_mutex>

//socket1 get T1 send T2
//socket2 get T2 send T1
template <typename T1, typename T2>
class AsymmetricITC
{
	friend class Socket1;
	friend class Socket2;
public:
	struct InData1
	{
		uint16_t sourceSocketId;
		//std::string message;
		T1 message;
	};
	struct InData2
	{
		uint16_t sourceSocketId;
		//std::string message;
		T2 message;
	};
	class Socket1
	{
		friend class AsymmetricITC;
	public:
		Socket1(uint16_t socketId, AsymmetricITC& _master) : master(_master) { myId = socketId; }
		~Socket1() { master.delete_socket1(myId); }

		bool message_to_socket(uint16_t socketId, T2& message)
		{
			return master.socket1_send_message(socketId, message, myId);
		}
		std::pair<bool, InData1> get_message()
		{
			return master.socket1_get_message(myId);
		}
		bool wait_message()
		{
			return master.socket1_wait_message(myId);
		}

	private:
		AsymmetricITC& master;
		uint16_t myId;
	};
	class Socket2
	{
		friend class AsymmetricITC;
	public:
		Socket2(uint16_t socketId, AsymmetricITC& _master) : master(_master) { myId = socketId; }
		~Socket2() { master.delete_socket2(myId); }

		bool message_to_socket(uint16_t socketId, T1& message)
		{
			return master.socket2_send_message(socketId, message, myId);
		}
		std::pair<bool, InData2> get_message()
		{
			return master.socket2_get_message(myId);
		}
		bool wait_message()
		{
			return master.socket2_wait_message(myId);
		}

	private:
		AsymmetricITC& master;
		uint16_t myId;
	};

	AsymmetricITC() {}
	~AsymmetricITC() {}

	std::shared_ptr<Socket1> create_socket1(uint16_t socketId)
	{
		std::unique_lock<std::shared_mutex> locker(dataMapLock1);
		auto i = id2SocketDataMap1.find(socketId);
		if (i != id2SocketDataMap1.end())
		{
			return nullptr;
		}
		id2SocketDataMap1[socketId];
		auto& j = id2SignalDataMap1[socketId];
		j.closing = false;
		return std::make_shared<Socket1>(socketId, *this);
	}
	std::shared_ptr<Socket2> create_socket2(uint16_t socketId)
	{
		std::unique_lock<std::shared_mutex> locker(dataMapLock2);
		auto i = id2SocketDataMap2.find(socketId);
		if (i != id2SocketDataMap2.end())
		{
			return nullptr;
		}
		id2SocketDataMap2[socketId];
		auto& j = id2SignalDataMap2[socketId];
		j.closing = false;
		return std::make_shared<Socket2>(socketId, *this);
	}

private:
	bool socket2_send_message(uint16_t destSocketId, T1& message, uint16_t sourceSocketId)
	{
		std::shared_lock<std::shared_mutex> locker(dataMapLock1);
		auto i = id2SocketDataMap1.find(destSocketId);
		if (i == id2SocketDataMap1.end())
		{
			return false;
		}
		std::lock_guard<std::mutex> lock2(i->second.dataMutex);
		i->second.data.push_back({ sourceSocketId, move(message) });
		auto j = id2SignalDataMap1.find(destSocketId);
		j->second.waitCondition.notify_one();
		return true;
	}
	bool socket1_send_message(uint16_t destSocketId, T2& message, uint16_t sourceSocketId)
	{
		std::shared_lock<std::shared_mutex> locker(dataMapLock2);
		auto i = id2SocketDataMap2.find(destSocketId);
		if (i == id2SocketDataMap2.end())
		{
			return false;
		}
		std::lock_guard<std::mutex> lock2(i->second.dataMutex);
		i->second.data.push_back({ sourceSocketId, move(message) });
		auto j = id2SignalDataMap2.find(destSocketId);
		j->second.waitCondition.notify_one();
		return true;
	}

	void delete_socket1(uint16_t socketId)
	{
		std::unique_lock<std::shared_mutex> locker(dataMapLock1);
		auto i = id2SignalDataMap1.find(socketId);
		if (i->second.waitMutex.try_lock())
		{
			//wait for message thread isnt running, can delete
			id2SocketDataMap1.erase(socketId);
			id2SignalDataMap1.erase(socketId);
			i->second.waitMutex.unlock();
		}
		else
		{
			//waiting for message, give a failed message, then wait for exit waiting signal.
			i->second.closing = true;
			i->second.waitCondition.notify_one();
			std::unique_lock<std::mutex> lock2(i->second.waitMutex);
			i->second.waitCondition.wait(lock2);
			id2SocketDataMap1.erase(socketId);
			id2SignalDataMap1.erase(socketId);
		}
	}
	void delete_socket2(uint16_t socketId)
	{
		std::unique_lock<std::shared_mutex> locker(dataMapLock2);
		auto i = id2SignalDataMap2.find(socketId);
		if (i->second.waitMutex.try_lock())
		{
			//wait for message thread isnt running, can delete
			id2SocketDataMap2.erase(socketId);
			id2SignalDataMap2.erase(socketId);
			i->second.waitMutex.unlock();
		}
		else
		{
			//waiting for message, give a failed message, then wait for exit waiting signal.
			i->second.closing = true;
			i->second.waitCondition.notify_one();
			std::unique_lock<std::mutex> lock2(i->second.waitMutex);
			i->second.waitCondition.wait(lock2);
			id2SignalDataMap2.erase(socketId);
			id2SocketDataMap2.erase(socketId);
		}
	}

	std::pair<bool, InData1> socket1_get_message(uint16_t socketId)
	{
		std::shared_lock<std::shared_mutex> locker(dataMapLock1);
		auto i = id2SocketDataMap1.find(socketId);
		if (i == id2SocketDataMap1.end())
		{
			return{ false,{} };
		}
		std::lock_guard<std::mutex> lock2(i->second.dataMutex);
		if (i->second.data.size())
		{
			std::pair<bool, InData1> retVal = { true, std::move(i->second.data.front()) };
			i->second.data.pop_front();
			return retVal;
		}
		return{ false,{} };
	}
	std::pair<bool, InData2> socket2_get_message(uint16_t socketId)
	{
		std::shared_lock<std::shared_mutex> locker(dataMapLock2);
		auto i = id2SocketDataMap2.find(socketId);
		if (i == id2SocketDataMap2.end())
		{
			return{ false,{} };
		}
		std::lock_guard<std::mutex> lock2(i->second.dataMutex);
		if (i->second.data.size())
		{
			std::pair<bool, InData2> retVal = { true, std::move(i->second.data.front()) };
			i->second.data.pop_front();
			return retVal;
		}
		return{ false,{} };
	}

	bool socket1_wait_message(uint16_t socketId)
	{
		//Check if got message already
		dataMapLock1.lock_shared();
		auto j = id2SignalDataMap1.find(socketId);
		std::unique_lock<std::mutex> lock(j->second.waitMutex);
		auto i = id2SocketDataMap1.find(socketId);
		if (i == id2SocketDataMap1.end())
		{
			//socket is no longer valid
			dataMapLock1.unlock_shared();
			return false;
		}
		{
			std::lock_guard<std::mutex> lock2(i->second.dataMutex);
			if (i->second.data.size())
			{
				dataMapLock1.unlock_shared();
				//Got message already, no need to wait.
				return true;
			}
		}
		dataMapLock1.unlock_shared();
		//Start waiting.
		j->second.waitCondition.wait(lock);
		//Got signal, check what kind it is
		if (j->second.closing)
		{
			//A closing signal
			j->second.waitCondition.notify_one();
			return false;
		}
		else
		{
			return true;
		}
	}
	bool socket2_wait_message(uint16_t socketId)
	{
		//Check if got message already
		dataMapLock2.lock_shared();
		auto j = id2SignalDataMap2.find(socketId);
		std::unique_lock<std::mutex> lock(j->second.waitMutex);
		auto i = id2SocketDataMap2.find(socketId);
		if (i == id2SocketDataMap2.end())
		{
			//socket is no longer valid
			dataMapLock2.unlock_shared();
			return false;
		}
		{
			std::lock_guard<std::mutex> lock2(i->second.dataMutex);
			if (i->second.data.size())
			{
				dataMapLock2.unlock_shared();
				//Got message already, no need to wait.
				return true;
			}
		}
		dataMapLock2.unlock_shared();
		//Start waiting.
		j->second.waitCondition.wait(lock);
		//Got signal, check what kind it is
		if (j->second.closing)
		{
			//A closing signal
			j->second.waitCondition.notify_one();
			return false;
		}
		else
		{
			return true;
		}
	}
private:
	struct Socket1Data
	{
		Socket1Data() {}
		std::mutex dataMutex;
		std::list<InData1> data;
	};
	struct Socket2Data
	{
		Socket2Data() {}
		std::mutex dataMutex;
		std::list<InData2> data;
	};
	struct SignalData
	{
		SignalData() {}
		std::mutex waitMutex;
		std::condition_variable waitCondition;
		bool closing;
	};
	std::shared_mutex dataMapLock1;
	std::unordered_map<uint16_t, Socket1Data> id2SocketDataMap1;
	std::unordered_map<uint16_t, SignalData> id2SignalDataMap1;
	std::shared_mutex dataMapLock2;
	std::unordered_map<uint16_t, Socket2Data> id2SocketDataMap2;
	std::unordered_map<uint16_t, SignalData> id2SignalDataMap2;
};

#endif