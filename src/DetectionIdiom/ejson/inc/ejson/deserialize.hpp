
#pragma once

#include "ejson/detect.hpp"
#include <stdexcept>

namespace ejson { namespace detail {

    inline stdnext::optional<bool> fromJson(const json11::Json& json, ReturnOptionalTag, bool*) {
        if (!json.is_bool())
            return stdnext::nullopt;
        return stdnext::make_optional(json.bool_value());
    }

    inline stdnext::optional<int> fromJson(const json11::Json& json, ReturnOptionalTag, int*) {
        if (!json.is_number())
            return stdnext::nullopt;
        return stdnext::make_optional(json.int_value());
    }

    inline stdnext::optional<std::string> fromJson(const json11::Json& json, ReturnOptionalTag, std::string*) {
        if (!json.is_string())
            return stdnext::nullopt;
        return stdnext::make_optional(json.string_value());
    }

    template<typename T>
    inline stdnext::optional<std::vector<T>> fromJson(const json11::Json& json, ReturnOptionalTag, std::vector<T>*) {
        if (!json.is_array())
            return stdnext::nullopt;
        std::vector<T> array;
        for(const auto& jsonItem : json.array_items()) {
            auto item = fromJson(jsonItem, ReturnOptionalTag{}, (T*)nullptr);
            if (!item)
                return stdnext::nullopt;
            array.emplace_back(std::move(*item));
        }
        return stdnext::optional<std::vector<T>>(std::move(array));
    }

    template<typename T>
    inline std::enable_if_t<
        stdnext::is_detected_v<detail::staticMemberFromJsonOptionalExpr, T>,
        stdnext::optional<T>>
    fromJson(const json11::Json& json, ReturnOptionalTag tag, T*) {
        return T::fromJson(json, tag);
    }

    template<typename T>
    inline std::enable_if_t<
        !stdnext::is_detected_v<detail::staticMemberFromJsonOptionalExpr, T> && stdnext::is_constructible_v<T, const json11::Json&>,
        stdnext::optional<T>>
    fromJson(const json11::Json& json, ReturnOptionalTag tag, T*) {
        try {
            return stdnext::make_optional<T>(json);
        } catch(DeserializationError&) {
            return stdnext::nullopt;
        }
    }

    template<typename T>
    inline std::enable_if_t<
        stdnext::is_detected_v<detail::staticMemberFromJsonObjectExpr, T>,
        T>
    fromJson(const json11::Json& json, ReturnObjectTag tag, T*) {
        return T::fromJson(json, tag);
    }

    template<typename T>
    inline std::enable_if_t<
        !stdnext::is_detected_v<detail::staticMemberFromJsonObjectExpr, T> && !stdnext::is_constructible_v<T, const json11::Json&>,
        T>
    fromJson(const json11::Json& json, ReturnObjectTag, T*) {
        auto opt = fromJson(json, ReturnOptionalTag{}, (T*)nullptr);
        if (!opt)
            throw DeserializationError("C++/JSON type mismatch");
        return *opt;
    }

    template<typename T>
    inline std::enable_if_t<
        !stdnext::is_detected_v<detail::staticMemberFromJsonObjectExpr, T> && stdnext::is_constructible_v<T, const json11::Json&>,
        T>
    fromJson(const json11::Json& json, ReturnObjectTag, T*) {
        return T(json);
    }

    template<typename T>
    struct ReturnTypeTraits {
        using InnerType = T;
        using ReturnType = T;
        using TagType = ReturnObjectTag;

        static T error(const std::string& errorMsg) {
            throw DeserializationError(errorMsg);
        }
    };

    template<typename T>
    struct ReturnTypeTraits<stdnext::optional<T>> {
        using InnerType = T;
        using ReturnType = stdnext::optional<T>;
        using TagType = ReturnOptionalTag;

        static stdnext::optional<T> error(const std::string&) {
            return stdnext::nullopt;
        }
    };

    template<typename T>
    struct DeserializerEx {
        using InnerType = typename ReturnTypeTraits<T>::InnerType;
        using ReturnType = typename ReturnTypeTraits<T>::ReturnType;
        using TagType = typename ReturnTypeTraits<T>::TagType;

        static ReturnType deserialize(const json11::Json& json) {
            return fromJson(json, TagType{}, (InnerType*)nullptr);
        }
    };

} } // namespace ejson::detail

namespace ejson {

    template<typename T>
    inline T deserialize(const std::string& jsonString) {
        std::string parseError;
        const auto json = json11::Json::parse(jsonString, parseError);
        if (!parseError.empty())
            return detail::ReturnTypeTraits<T>::error("json parsing error: " + parseError);
        return detail::DeserializerEx<T>::deserialize(json);
    }

    template<typename T>
    inline T deserialize(const json11::Json& json) {
        return detail::DeserializerEx<T>::deserialize(json);
    }

}
