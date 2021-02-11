#ifndef _PrioritizedValue_H_
#define _PrioritizedValue_H_
#include "Value.h"
#include <map>

//Higher number => higher priority
class PrioritizedValue {
public:
    PrioritizedValue();
    virtual ~PrioritizedValue();

    //Returns true if this set triggers change of read value
    bool set_value(uint8_t priority, const Value& newValue);
    //Returns true if this unset triggers change to a new valid value
    bool unset_value(uint8_t priority);
    //Simular to unset at all priorities value 
    void clear_lower(uint8_t priority);
    Value get_value() const;
    uint8_t get_active_priority();
    //Does not persist, same as clear then set with priority 0
    //void trigger_value(const Value& newValue, uint8_t priority) { clear_value(); set_value(0, newValue); }

private:
    std::map<uint8_t, Value> valueMap;
};
#endif
