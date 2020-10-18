#pragma once
#include <utility>
#include <string>
#include "../../OtherLib/nlohmann/json.hpp"
#include "HashKey.h"
#include "Value.h"

namespace Helper {
    nlohmann::json value_to_json(const Value& input);
    std::pair<bool, Value> json_to_value(const nlohmann::json& input);
    std::pair<bool, HashKey::EitherKey> json_to_hashkey(const nlohmann::json& input);
    std::pair<bool, nlohmann::json> hashkey_to_json(const HashKey::EitherKey& input);
    HashKey::EitherKey value_to_either_key(const Value& input);
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

inline HashKey::EitherKey value_to_either_key(const Value& input) {
    if(input.is_integer()) {
        return input.get_int();
    }
    else {
        return input.get_string();
    }
}
