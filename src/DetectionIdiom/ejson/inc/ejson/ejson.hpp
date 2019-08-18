
#pragma once

#include "json11/json11.hpp"
#include <string>
#include <stdexcept>

// DOCUMENTED API
namespace ejson {

    /// Deserialize from a json string
    template<typename T>
    T deserialize(const std::string& jsonString);

    /// Deserialize from a json11 object
    template<typename T>
    T deserialize(const json11::Json& json);

    /// Deserialization exception
    class DeserializationError : public std::invalid_argument {
    public:
        using std::invalid_argument::invalid_argument;
    };

    /// Return type choices
    class ReturnObjectTag {};
    class ReturnOptionalTag {};

    /// Possible customization points, in order of precedence:
    /// 1 - free-standing function: T fromJson(const json11&, ReturnObjectTag, T*) or optional<T> fromJson(const json11&, ReturnOptionalTag, T*)
    /// 2 - static member function: T T::fromJson(const json11&, ReturnObjectTag) or optional<T> T::fromJson(const json11&, ReturnOptionalTag)
    /// 3 - constructor: T::T(const json11&)
}

// IMPLEMENTATION
#include "ejson/detect.hpp"
#include "ejson/deserialize.hpp"
