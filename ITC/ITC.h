/************************************************************************************************************
* ITC stands for Inter-Threads Communicator, it helps threads to exchange information.						* 
* ITC<T>::Socket and ITC<T>::FixedSocket can only talk to other Socket(s) created by the same ITC<T>		*
* The IDs of the Sockets created by the same ITC<T> cannot repeat. 											*
* ITC<T>::FixedSocket allows sending message without specifying destination (it is defined in constructor)	*
* ITC<T>::Socket needs destination specified each time it sends message (that's the only difference)		*
************************************************************************************************************/
#pragma once
#ifndef _ITC_H_
#define _ITC_H_
#include <unordered_map>
#include <tuple>
#include <string>
#include <mutex>
#include <list>
#include <condition_variable>
#include <thread>

template <typename T>
class ITC
{
	friend class Proxy;
public:
	struct InData
	{
		size_t sourceSocketId;
		T message;
	};
	enum class WaitResult
	{
		Terminated,
		Timeout,
		Interrupted
	};
private:
	class Proxy
	{
	public:
		Proxy(ITC &_master) : master(_master) {}
		~Proxy() {}
		bool send_message_to_socket(size_t destSocketId, T &message, size_t sourceSocketId)
		{
			return master.send_message_to_socket(destSocketId, message, sourceSocketId);
		}
		bool send_message_to_socket(size_t destSocketId, const T& message, size_t sourceSocketId)
		{
			return master.send_message_to_socket(destSocketId, message, sourceSocketId);
		}
		void delete_socket(size_t socketId)
		{
			master.delete_socket(socketId);
		}
		InData get_message(size_t socketId)
		{
			return master.get_message(socketId);
		}
		bool wait_message(size_t socketId)
		{
			return master.wait_message(socketId);
		}
		std::pair<bool, bool> wait_message(size_t socketId, std::chrono::duration<uint64_t> duration)
		{
			return master.wait_message(socketId, duration);
		}
		bool has_message(size_t socketId)
		{
			return master.has_message(socketId);
		}

	private:
		ITC &master;
	};
public:
	class Socket
	{
		friend class ITC;
	public:
		Socket(size_t socketId, std::shared_ptr<Proxy> _masterProxy)
		{
			myId = socketId;
			masterProxy = _masterProxy;
		}
		virtual ~Socket()
		{
			auto shared = masterProxy.lock();
			if (shared != nullptr)
			{
				shared->delete_socket(myId);
			}
		}
		bool message_to_socket(size_t socketId, T &message)
		{
			auto shared = masterProxy.lock();
			if (shared != nullptr)
			{
				return shared->send_message_to_socket(socketId, message, myId);
			}
			return false;
		}
		bool message_to_socket(size_t socketId, const T &message)
		{
			auto shared = masterProxy.lock();
			if (shared != nullptr)
			{
				return shared->send_message_to_socket(socketId, message, myId);
			}
			return false;
		}
		InData get_message()
		{
			auto shared = masterProxy.lock();
			if (shared != nullptr)
			{
				return shared->get_message(myId);
			}
			return InData();
		}
		bool wait_message()
		{
			auto shared = masterProxy.lock();
			if (shared != nullptr)
			{
				return shared->wait_message(myId);
			}
			return false;
		}
		std::pair<bool, bool> wait_message(std::chrono::duration<uint64_t> duration)
		{
			auto shared = masterProxy.lock();
			if(shared != nullptr)
			{
				return shared->wait_message(myId, duration);
			}
			return {false, false};
		}
		bool has_message()
		{
			auto shared = masterProxy.lock();
			return shared->has_message(myId);
		}
		size_t get_id()
		{
			return myId;
		}

	private:
		std::weak_ptr<Proxy> masterProxy;
		size_t myId;
	};

	class FixedSocket : public Socket
	{
	public:
		FixedSocket(size_t myId, size_t _hisId, std::shared_ptr<Proxy> _masterProxy) : Socket(myId, _masterProxy)
		{
			hisId = _hisId;
		}
		virtual ~FixedSocket() {}
		bool send_message(T &message)
		{
			return Socket::message_to_socket(hisId, message);
		}
		bool send_message(const T &message)
		{
			return Socket::message_to_socket(hisId, message);
		}

	private:
		size_t hisId;
	};

	ITC() { myProxy = std::make_shared<Proxy>(*this); }
	~ITC()
	{
		std::weak_ptr<Proxy> proxyShadow = myProxy;
		myProxy.reset();
		//Notify all
		{
			std::lock_guard<std::mutex> locker(dataMapLock);
			for (auto i = id2SignalDataMap.begin(); i != id2SignalDataMap.end(); ++i)
			{
				std::unique_lock<std::mutex> lock(i->second.waitMutex);
				i->second.closing = true;
				i->second.waitCondition.notify_one();
			}
		}
		//Make sure no one is accessing this proxy before ending
		while (proxyShadow.lock() != nullptr)
		{
			std::this_thread::yield();
		}
	}

	std::unique_ptr<Socket> create_socket(size_t socketId)
	{
		std::unique_lock<std::mutex> locker(dataMapLock);
		auto i = id2SocketDataMap.find(socketId);
		if (i != id2SocketDataMap.end())
		{
			return nullptr;
		}
		id2SocketDataMap[socketId];
		auto &j = id2SignalDataMap[socketId];
		j.closing = false;
		return std::make_unique<Socket>(socketId, myProxy);
	}

	std::unique_ptr<FixedSocket> create_fixed_socket(size_t myId, size_t hisId)
	{
		std::unique_lock<std::mutex> locker(dataMapLock);
		auto i = id2SocketDataMap.find(myId);
		if (i != id2SocketDataMap.end())
		{
			return nullptr;
		}
		id2SocketDataMap[myId];
		auto &j = id2SignalDataMap[myId];
		j.closing = false;
		return std::make_unique<FixedSocket>(myId, hisId, myProxy);
	}

private:
	//messages would be moved
	bool send_message_to_socket(size_t destSocketId, T &message, size_t sourceSocketId)
	{
		std::lock_guard<std::mutex> locker(dataMapLock);
		auto i = id2SocketDataMap.find(destSocketId);
		if (i == id2SocketDataMap.end())
		{
			return false;
		}
		std::lock_guard<std::mutex> lock2(i->second.dataMutex);
		i->second.data.push_back({sourceSocketId, std::move(message)});
		auto j = id2SignalDataMap.find(destSocketId);
		j->second.waitCondition.notify_one();
		return true;
	}

	bool send_message_to_socket(size_t destSocketId, const T &message, size_t sourceSocketId)
	{
		std::lock_guard<std::mutex> locker(dataMapLock);
		auto i = id2SocketDataMap.find(destSocketId);
		if (i == id2SocketDataMap.end())
		{
			return false;
		}
		std::lock_guard<std::mutex> lock2(i->second.dataMutex);
		i->second.data.push_back({sourceSocketId, message});
		auto j = id2SignalDataMap.find(destSocketId);
		j->second.waitCondition.notify_one();
		return true;
	}

	void delete_socket(size_t socketId)
	{
		std::lock_guard<std::mutex> locker(dataMapLock);
		auto i = id2SignalDataMap.find(socketId);
		if (i->second.waitMutex.try_lock())
		{
			//wait for message thread isnt running, can delete
			id2SocketDataMap.erase(socketId);
			i->second.waitMutex.unlock();
			id2SignalDataMap.erase(socketId);
		}
		else
		{
			//waiting for message, give a failed message, then wait for exit waiting signal.
			i->second.closing = true;
			i->second.waitCondition.notify_one();
			std::unique_lock<std::mutex> lock2(i->second.waitMutex);
			i->second.waitCondition.wait(lock2);
			id2SignalDataMap.erase(socketId);
			id2SignalDataMap.erase(socketId);
		}
	}

	InData get_message(size_t socketId)
	{
		std::lock_guard<std::mutex> locker(dataMapLock);
		auto i = id2SocketDataMap.find(socketId);
		if (i == id2SocketDataMap.end())
		{
			return InData();
		}
		std::lock_guard<std::mutex> lock2(i->second.dataMutex);
		if (i->second.data.size())
		{
			ITC::InData retVal = std::move(i->second.data.front());
			i->second.data.pop_front();
			return retVal;
		}
		return InData();
	}

	bool has_message(size_t socketId)
	{
		std::lock_guard<std::mutex> locker(dataMapLock);
		auto i = id2SocketDataMap.find(socketId);
		if (i == id2SocketDataMap.end())
		{
			return false;
		}
		std::lock_guard<std::mutex> lock2(i->second.dataMutex);
		if (i->second.data.size())
		{
			return true;
		}
		return false;
	}

	bool wait_message(size_t socketId)
	{
		//Check if got message already
		dataMapLock.lock();
		auto j = id2SignalDataMap.find(socketId);
		std::unique_lock<std::mutex> lock(j->second.waitMutex);
		auto i = id2SocketDataMap.find(socketId);
		if (i == id2SocketDataMap.end())
		{
			//socket is no longer valid
			dataMapLock.unlock();
			return false;
		}
		{
			std::lock_guard<std::mutex> lock2(i->second.dataMutex);
			if (i->second.data.size())
			{
				dataMapLock.unlock();
				//Got message already, no need to wait.
				return true;
			}
		}
		dataMapLock.unlock();
		//Start waiting.
		j->second.waitCondition.wait(lock);
		//Got signal, check what kind it is
		if (j->second.closing)
		{
			//A closing signal
			//j->second.waitCondition.notify_one();
			return false;
		}
		else
		{
			return true;
		}
	}

	//1st true if ITC still valid, false if ITC is dectructed.
	//2nd true if got message come in, false if time out.
	std::pair<bool, bool> wait_message(size_t socketId, std::chrono::duration<uint64_t> duration)
	{
		//Check if got message already
		dataMapLock.lock();
		auto j = id2SignalDataMap.find(socketId);
		std::unique_lock<std::mutex> lock(j->second.waitMutex);
		auto i = id2SocketDataMap.find(socketId);
		if (i == id2SocketDataMap.end())
		{
			//socket is no longer valid
			dataMapLock.unlock();
			return {false, false};
		}
		{
			std::lock_guard<std::mutex> lock2(i->second.dataMutex);
			if (i->second.data.size())
			{
				dataMapLock.unlock();
				//Got message already, no need to wait.
				return {true, true};
			}
		}
		dataMapLock.unlock();
		//Start waiting.
		auto result = j->second.waitCondition.wait_for(lock, duration);
		//auto result = j->second.waitCondition.wait_for(lock, std::chrono::seconds(10));
		//Got signal, check what kind it is
		if (j->second.closing)
		{
			//A closing signal
			//j->second.waitCondition.notify_one();
			return {false, false};
		}
		else if (result == std::cv_status::timeout)
		{
			return {true, false};
		}
		return {true, true};
	}

private:
	struct SocketData
	{
		SocketData() {}
		std::mutex dataMutex;
		std::list<InData> data;
	};
	struct SignalData
	{
		SignalData() {}
		std::mutex waitMutex;
		std::condition_variable waitCondition;
		bool closing;
	};
	std::mutex dataMapLock;
	std::unordered_map<size_t, SocketData> id2SocketDataMap;
	std::unordered_map<size_t, SignalData> id2SignalDataMap;
	std::shared_ptr<Proxy> myProxy;
};

#endif
