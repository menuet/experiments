
#include "ejson/ejson.hpp"
#include <catch/catch.hpp>

namespace {

    struct HasFreeStandingFromJson {
        int m_i = 0;
        std::string m_s;
    };

    auto fromJson(const json11::Json& json, ejson::ReturnObjectTag, HasFreeStandingFromJson*) {
        return HasFreeStandingFromJson{
           ejson::deserialize<int>(json["i"]),
           ejson::deserialize<std::string>(json["s"]),
        };
    }

    auto fromJson(const json11::Json& json, ejson::ReturnOptionalTag, HasFreeStandingFromJson*) {
        return stdnext::optional<HasFreeStandingFromJson>(HasFreeStandingFromJson{
           ejson::deserialize<int>(json["i"]),
           ejson::deserialize<std::string>(json["s"]),
        });
    }

    struct HasStaticMemberFromJson {
        static auto fromJson(const json11::Json& json, ejson::ReturnObjectTag) {
            return HasStaticMemberFromJson{
                ejson::deserialize<int>(json["i"]),
                ejson::deserialize<std::string>(json["s"]),
            };
        }

        static auto fromJson(const json11::Json& json, ejson::ReturnOptionalTag) {
            return stdnext::optional<HasStaticMemberFromJson>(HasStaticMemberFromJson{
                ejson::deserialize<int>(json["i"]),
                ejson::deserialize<std::string>(json["s"]),
            });
        }

        int m_i = 0;
        std::string m_s;
    };

    struct IsConstructibleFromJson {
        IsConstructibleFromJson(const json11::Json& json)
            : m_i{ejson::deserialize<int>(json["i"])}
            , m_s{ejson::deserialize<std::string>(json["s"])} {
        }

        int m_i = 0;
        std::string m_s;
    };

    struct HasStaticMemberFromJsonAndIsConstructible {
        HasStaticMemberFromJsonAndIsConstructible(const json11::Json&) {
        }

        HasStaticMemberFromJsonAndIsConstructible(int i, std::string s)
            : m_i(i)
            , m_s(s) {
        }

        static auto fromJson(const json11::Json& json, ejson::ReturnObjectTag) {
            return HasStaticMemberFromJsonAndIsConstructible{
                ejson::deserialize<int>(json["i"]),
                ejson::deserialize<std::string>(json["s"]),
            };
        }

        static auto fromJson(const json11::Json& json, ejson::ReturnOptionalTag) {
            return stdnext::optional<HasStaticMemberFromJsonAndIsConstructible>(HasStaticMemberFromJsonAndIsConstructible{
                ejson::deserialize<int>(json["i"]),
                ejson::deserialize<std::string>(json["s"]),
            });
        }

        int m_i = 666;
        std::string m_s;
    };
}

TEST_CASE("detect", "[traits]") {
    SECTION("freeStandingFromJsonObjectExpr") {
        SECTION("When type has associated free-standing function fromJson returning object") {
            const auto result = stdnext::is_detected_v<ejson::detail::freeStandingFromJsonObjectExpr, HasFreeStandingFromJson>;
            REQUIRE(result);
        }
        SECTION("When type has a static member function fromJson returning object") {
            // TODO: fix
            // const auto result = stdnext::is_detected_v<ejson::detail::freeStandingFromJsonObjectExpr, HasStaticMemberFromJson>;
            // REQUIRE(!result);
        }
        SECTION("When type is constructible from Json") {
            // TODO: fix
            // const auto result = stdnext::is_detected_v<ejson::detail::freeStandingFromJsonObjectExpr, IsConstructibleFromJson>;
            // REQUIRE(!result);
        }
        SECTION("When type has a static member function fromJson returning object and is constructible from Json") {
            // TODO: fix
            // const auto result = stdnext::is_detected_v<ejson::detail::freeStandingFromJsonObjectExpr, HasStaticMemberFromJsonAndIsConstructible>;
            // REQUIRE(!result);
        }
    }
    SECTION("freeStandingFromJsonOptionalExpr") {
        SECTION("When type has associated free-standing function fromJson returning optional") {
            const auto result = stdnext::is_detected_v<ejson::detail::freeStandingFromJsonOptionalExpr, HasFreeStandingFromJson>;
            REQUIRE(result);
        }
        SECTION("When type has a static member function fromJson returning optional") {
            // TODO: fix
            // const auto result = stdnext::is_detected_v<ejson::detail::freeStandingFromJsonOptionalExpr, HasStaticMemberFromJson>;
            // REQUIRE(!result);
        }
        SECTION("When type is constructible from Json") {
            // TODO: fix
            // const auto result = stdnext::is_detected_v<ejson::detail::freeStandingFromJsonOptionalExpr, IsConstructibleFromJson>;
            // REQUIRE(!result);
        }
        SECTION("When type has a static member function fromJson returning optional and is constructible from Json") {
            // TODO: fix
            // const auto result = stdnext::is_detected_v<ejson::detail::freeStandingFromJsonOptionalExpr, HasStaticMemberFromJsonAndIsConstructible>;
            // REQUIRE(!result);
        }
    }
    SECTION("staticMemberFromJsonObjectExpr") {
        SECTION("When type has associated free-standing function fromJson returning object") {
            const auto result = stdnext::is_detected_v<ejson::detail::staticMemberFromJsonObjectExpr, HasFreeStandingFromJson>;
            REQUIRE(!result);
        }
        SECTION("When type has a static member function fromJson returning object") {
            const auto result = stdnext::is_detected_v<ejson::detail::staticMemberFromJsonObjectExpr, HasStaticMemberFromJson>;
            REQUIRE(result);
        }
        SECTION("When type is constructible from Json") {
            const auto result = stdnext::is_detected_v<ejson::detail::staticMemberFromJsonObjectExpr, IsConstructibleFromJson>;
            REQUIRE(!result);
        }
        SECTION("When type has a static member function fromJson returning object and is constructible from Json") {
            const auto result = stdnext::is_detected_v<ejson::detail::staticMemberFromJsonObjectExpr, HasStaticMemberFromJsonAndIsConstructible>;
            REQUIRE(result);
        }
    }
    SECTION("staticMemberFromJsonOptionalExpr") {
        SECTION("When type has associated free-standing function fromJson returning optional") {
            const auto result = stdnext::is_detected_v<ejson::detail::staticMemberFromJsonOptionalExpr, HasFreeStandingFromJson>;
            REQUIRE(!result);
        }
        SECTION("When type has a static member function fromJson returning optional") {
            const auto result = stdnext::is_detected_v<ejson::detail::staticMemberFromJsonOptionalExpr, HasStaticMemberFromJson>;
            REQUIRE(result);
        }
        SECTION("When type is constructible from Json") {
            const auto result = stdnext::is_detected_v<ejson::detail::staticMemberFromJsonOptionalExpr, IsConstructibleFromJson>;
            REQUIRE(!result);
        }
        SECTION("When type has a static member function fromJson returning optional and is constructible from Json") {
            const auto result = stdnext::is_detected_v<ejson::detail::staticMemberFromJsonOptionalExpr, HasStaticMemberFromJsonAndIsConstructible>;
            REQUIRE(result);
        }
    }
}

TEST_CASE("deserialize", "[parsing]") {
    SECTION("non-optional") {
        SECTION("When type is bool") {
            const auto val = ejson::deserialize<bool>(std::string(R"(false)"));
            REQUIRE(val == false);
        }
        SECTION("When type is bool") {
            const auto val = ejson::deserialize<bool>(std::string(R"(true)"));
            REQUIRE(val == true);
        }
        SECTION("When type is int") {
            const auto val = ejson::deserialize<int>(std::string(R"(1234)"));
            REQUIRE(val == 1234);
        }
        SECTION("When type is string") {
            const auto val = ejson::deserialize<std::string>(std::string(R"("blablah")"));
            REQUIRE(val == "blablah");
        }
        SECTION("When type is HasFreeStandingFromJson") {
            const auto val = ejson::deserialize<HasFreeStandingFromJson>(std::string(R"({ "i": 789, "s": "blablah" })"));
            REQUIRE(val.m_i == 789);
            REQUIRE(val.m_s == "blablah");
        }
        SECTION("When type is HasStaticMemberFromJson") {
            const auto val = ejson::deserialize<HasStaticMemberFromJson>(std::string(R"({ "i": 789, "s": "blablah" })"));
            REQUIRE(val.m_i == 789);
            REQUIRE(val.m_s == "blablah");
        }
        SECTION("When type is IsConstructibleFromJson") {
            const auto val = ejson::deserialize<IsConstructibleFromJson>(std::string(R"({ "i": 789, "s": "blablah" })"));
            REQUIRE(val.m_i == 789);
            REQUIRE(val.m_s == "blablah");
        }
        SECTION("When type is HasStaticMemberFromJsonAndIsConstructible") {
            const auto val = ejson::deserialize<HasStaticMemberFromJsonAndIsConstructible>(std::string(R"({ "i": 789, "s": "blablah" })"));
            REQUIRE(val.m_i == 789);
            REQUIRE(val.m_s == "blablah");
        }
        SECTION("When type is std::vector<int>") {
            const auto val = ejson::deserialize<std::vector<int>>(std::string(R"([ 1, 2, 3 ])"));
            REQUIRE(val.size() == 3);
            REQUIRE(val[0] == 1);
            REQUIRE(val[1] == 2);
            REQUIRE(val[2] == 3);
        }
    }
    SECTION("non-optional") {
        SECTION("When type is bool") {
            const auto val = ejson::deserialize<stdnext::optional<bool>>(std::string(R"(false)"));
            REQUIRE(val);
            REQUIRE(*val == false);
        }
        SECTION("When type is bool") {
            const auto val = ejson::deserialize<stdnext::optional<bool>>(std::string(R"(true)"));
            REQUIRE(val);
            REQUIRE(*val == true);
        }
        SECTION("When type is int") {
            const auto val = ejson::deserialize<stdnext::optional<int>>(std::string(R"(1234)"));
            REQUIRE(val);
            REQUIRE(*val == 1234);
        }
        SECTION("When type is string") {
            const auto val = ejson::deserialize<stdnext::optional<std::string>>(std::string(R"("blablah")"));
            REQUIRE(val);
            REQUIRE(*val == "blablah");
        }
        SECTION("When type is HasFreeStandingFromJson") {
            const auto val = ejson::deserialize<stdnext::optional<HasFreeStandingFromJson>>(std::string(R"({ "i": 789, "s": "blablah" })"));
            REQUIRE(val);
            REQUIRE(val->m_i == 789);
            REQUIRE(val->m_s == "blablah");
        }
        SECTION("When type is HasStaticMemberFromJson") {
            const auto val = ejson::deserialize<stdnext::optional<HasStaticMemberFromJson>>(std::string(R"({ "i": 789, "s": "blablah" })"));
            REQUIRE(val);
            REQUIRE(val->m_i == 789);
            REQUIRE(val->m_s == "blablah");
        }
        SECTION("When type is IsConstructibleFromJson") {
            const auto val = ejson::deserialize<stdnext::optional<IsConstructibleFromJson>>(std::string(R"({ "i": 789, "s": "blablah" })"));
            REQUIRE(val);
            REQUIRE(val->m_i == 789);
            REQUIRE(val->m_s == "blablah");
        }
        SECTION("When type is HasStaticMemberFromJsonAndIsConstructible") {
            const auto val = ejson::deserialize<stdnext::optional<HasStaticMemberFromJsonAndIsConstructible>>(std::string(R"({ "i": 789, "s": "blablah" })"));
            REQUIRE(val);
            REQUIRE(val->m_i == 789);
            REQUIRE(val->m_s == "blablah");
        }
        SECTION("When type is std::vector<int>") {
            const auto val = ejson::deserialize<stdnext::optional<std::vector<int>>>(std::string(R"([ 1, 2, 3 ])"));
            REQUIRE(val);
            REQUIRE(val->size() == 3);
            REQUIRE((*val)[0] == 1);
            REQUIRE((*val)[1] == 2);
            REQUIRE((*val)[2] == 3);
        }
    }
}
