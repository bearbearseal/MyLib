#ifndef _Variable_H_
#define _Variable_H_
#include "Value.h"
#include <memory>
#include <vector>
#include <unordered_set>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <tuple>

class Variable
{
public:
	class Listener {
	public:
		Listener() {}
		virtual ~Listener() {}
		virtual void catch_value_change(const Value& newValue, std::chrono::time_point<std::chrono::system_clock> theMoment) = 0;
	};
	Variable();
	virtual ~Variable();
	Value read_value() const;
	virtual bool write_value(const Value& newValue) = 0;
	void add_listener(std::weak_ptr<Listener> _listener);

protected:
	virtual void update_value_to_cache(const Value& newValue);

private:
	mutable std::mutex valueMutex;
	Value value;

	std::mutex listenersMutex;
	std::vector<std::weak_ptr<Listener>> listeners;
};
#endif
