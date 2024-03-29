#include "Variable.h"

using namespace std;

Variable::Variable() {

}

Variable::~Variable() {

}

Value Variable::read_value() const {
	lock_guard<mutex> lock(valueMutex);
	return value;
}
/*
bool Variable::set_value(const Value& newValue, uint8_t priority) {
	if(writeValue.set_value(priority, newValue)) {
		if(value != newValue) {
			_write_value(newValue);
		}
		return true;
	}
	return false;
}

bool Variable::write_value(const Value& newValue, uint8_t priority) {
	//clear those priority lower than input priority.
	writeValue.clear_lower(priority);
	if(writeValue.set_value(priority, newValue)) {
		if(value != newValue) {
			_write_value(newValue);
		}
		return true;
	}
	return false;
}

void Variable::update_value_to_cache(const Value& newValue) {
	//If value not equal writeValue, call _write_value
	if(!writeValue.get_value().is_empty()) {
		if(newValue != writeValue.get_value()) {
			_write_value(writeValue.get_value());
		}
	}

	{
		lock_guard<mutex> lock(valueMutex);
		if(value == newValue) {
			return;
		}
		value = newValue;
	}
	{
		auto shared = listener.lock();
		if (shared != nullptr) {
			shared->catch_value_change(newValue, chrono::system_clock::now());
		}
	}
}
*/

void Variable::update_value_to_cache(const Value& newValue) {
	//Value update
	{
		lock_guard<mutex> lock(valueMutex);
		if(value == newValue) {
			return;
		}
		value = newValue;
	}
	//Notify listeners
	{
		lock_guard<mutex> lock(listenersMutex);
		for(auto i = listeners.begin(); i != listeners.end();) {
			auto shared = i->lock();
			if (shared != nullptr) {
				shared->catch_value_change(newValue, chrono::system_clock::now());
				++i;
			}
			else {
				i = listeners.erase(i);
			}
		}
	}
}

void Variable::add_listener(std::weak_ptr<Listener> _listener) {
	lock_guard<mutex> lock(listenersMutex);
	listeners.push_back(_listener);
}
/*
void Variable::set_listener(std::shared_ptr<Listener> _listener) {
	listener = _listener;
	printf("Listener now is %p\n", _listener.get());
}
*/