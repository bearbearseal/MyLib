#pragma once
#include <utility>
#include <string>
#include <chrono>
#include "../../OtherLib/nlohmann/json.hpp"
#include "HashKey.h"
#include "Value.h"

namespace Helper {
    nlohmann::json value_to_json(const Value& input);
    std::pair<bool, Value> json_to_value(const nlohmann::json& input);
    std::pair<bool, HashKey::EitherKey> json_to_hashkey(const nlohmann::json& input);
    std::pair<bool, nlohmann::json> hashkey_to_json(const HashKey::EitherKey& input);
    HashKey::EitherKey value_to_either_key(const Value& input);
    std::pair<std::string, std::string> millisecond_to_date_time(uint64_t milliSecondCount);
    const char* to_day_name(uint8_t dayIndex);
}

inline const char* Helper::to_day_name(uint8_t dayIndex) {
    static const char* dayName[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    dayIndex %= (sizeof(dayName)/sizeof(dayName[0]));
    return dayName[dayIndex];
}

inline nlohmann::json Helper::value_to_json(const Value& input) {
    if(input.is_float()) {
        return input.get_float();
    }
    else if(input.is_integer()) {
        return input.get_int();
    }
    return input.get_string();
}

inline std::pair<bool, HashKey::EitherKey> Helper::json_to_hashkey(const nlohmann::json& input) {
    if(input.is_number_integer()) {
        return {true, input.get<int64_t>()};
    }
    else if(input.is_string()) {
        return {true, input.get<std::string>()};
    }
    return {false, HashKey::EitherKey()};
}

inline std::pair<bool, nlohmann::json> Helper::hashkey_to_json(const HashKey::EitherKey& input) {
    if(input.is_integer()) {
        return {true, input.get_integer()};
    }
    else if(input.is_string()) {
        return {true, input.get_string()};
    }
    return {false, nlohmann::json()};
}

inline std::pair<bool, Value> Helper::json_to_value(const nlohmann::json& input) {
    if(input.is_number_integer()) {
        return {true, input.get<int64_t>()};
    }
    else if(input.is_number_float()) {
        return {true, input.get<double>()};
    }
    else if(input.is_string()) {
        return {true, input.get<std::string>()};
    }
    else if(input.is_boolean()) {
        return {true, input.get<bool>()};
    }
    return {false, 0};
}

inline HashKey::EitherKey Helper::value_to_either_key(const Value& input) {
    if(input.is_integer()) {
        return input.get_int();
    }
    else {
        return input.get_string();
    }
}

inline std::pair<std::string, std::string> Helper::millisecond_to_date_time(uint64_t milliSecondCount) {
    std::chrono::milliseconds mSec(milliSecondCount);
    std::chrono::time_point<std::chrono::system_clock> sysClock(mSec);
    std::time_t cTime = std::chrono::system_clock::to_time_t(sysClock);
    std::tm structTime = *std::localtime(&cTime);
    uint16_t remainer = uint16_t(milliSecondCount%1000);
    char dateBuffer[32], timeBuffer[32];
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d:%02d.%03d", structTime.tm_hour, structTime.tm_min, structTime.tm_sec, remainer);
    snprintf(dateBuffer, sizeof(dateBuffer), "%02d-%02d-%04d %s", structTime.tm_mday, structTime.tm_mon + 1, structTime.tm_year + 1900, Helper::to_day_name(structTime.tm_wday));
    return {dateBuffer, timeBuffer};    
}
