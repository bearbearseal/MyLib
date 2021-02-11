#include "PrioritizedValue.h"

using namespace std;

PrioritizedValue::PrioritizedValue()
{
}

PrioritizedValue::~PrioritizedValue()
{
}

bool PrioritizedValue::set_value(uint8_t priority, const Value &newValue)
{
    auto i = valueMap.find(priority);
    if (i != valueMap.end())
    {
        if (newValue == i->second)
        {
            //This set value doesnt change anything
            return false;
        }
    }
    valueMap[priority] = newValue;
    if (priority == valueMap.rbegin()->first) //reverse begin is the highest priority
    {
        //This set value would change the get value's value.
        return true;
    }
    return false;
}

bool PrioritizedValue::unset_value(uint8_t priority)
{
    if (!valueMap.size())
    {
        return false;
    }
    if (priority == valueMap.rbegin()->first)
    {
        //This unset would affect the read value
        valueMap.erase(priority);
        if (valueMap.size())
        {
            return true;
        }
        return false;
    }
    //This unset would not affect the read value
    valueMap.erase(priority);
    return false;
}

Value PrioritizedValue::get_value() const
{
    Value retValue;
    auto i = valueMap.rbegin();
    if (i != valueMap.rend())
    {
        retValue = i->second;
    }
    return retValue;
}

uint8_t PrioritizedValue::get_active_priority()
{
    uint8_t retVal = 0;
    if (valueMap.size())
    {
        retVal = valueMap.rbegin()->first;
    }
    return retVal;
    ;
}

void PrioritizedValue::clear_lower(uint8_t priority)
{
    for (auto i = valueMap.begin(); i != valueMap.end();)
    {
        if (i->second <= priority)
        {
            auto temp = i;
            ++i;
            valueMap.erase(temp);
        }
        else
        {
            break;
        }
    }
}
