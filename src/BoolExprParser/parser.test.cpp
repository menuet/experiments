
#include "catch.hpp"
#include "parser.manual.h"
#include <map>


namespace ut {

    SCENARIO("the parser can parse valid simple comparisons", "[parser]")
    {

        GIVEN("a parser")
        {
            Parser l_parser;

            WHEN("We parse 'CHANNEL == HELLO'")
            {
                const auto l_parseResult = l_parser.parse("CHANNEL == HELLO");

                THEN("It succeeds")
                {
                    REQUIRE(l_parseResult);
                }
            }

            WHEN("We parse 'UUID= HELLO'")
            {
                const auto l_parseResult = l_parser.parse("UUID= HELLO");

                THEN("It succeeds")
                {
                    REQUIRE(l_parseResult);
                }
            }

            WHEN("We parse '  GUID !=123456  '")
            {
                const auto l_parseResult = l_parser.parse("  GUID =123456  ");

                THEN("It succeeds")
                {
                    REQUIRE(l_parseResult);
                }
            }

            WHEN("We parse ' \t CHANNEL !=MOOO'")
            {
                const auto l_parseResult = l_parser.parse(" \t CHANNEL !=MOOO");

                THEN("It succeeds")
                {
                    REQUIRE(l_parseResult);
                }
            }

            WHEN("We parse ' \t not (UUID !=MOOO)'")
            {
                const auto l_parseResult = l_parser.parse(" \t not (UUID !=MOOO)");

                THEN("It succeeds")
                {
                    REQUIRE(l_parseResult);
                }
            }
        }

    }

    SCENARIO("the parser can parse valid complex expressions", "[parser]")
    {

        GIVEN("a parser")
        {
            Parser l_parser;

            WHEN("We parse 'CHANNEL == HELLO and (UUID != 456 or GUID == qwesdf!!)'")
            {
                const auto l_parseResult = l_parser.parse("CHANNEL == HELLO and (UUID != 456 or GUID == qwesdf!!)");

                THEN("It succeeds")
                {
                    REQUIRE(l_parseResult);
                }
            }

            WHEN("We parse '(   UUID = HELLO or not (UUID != BOB and GUID =PPP))'")
            {
                const auto l_parseResult = l_parser.parse("(   UUID = HELLO or not (UUID != BOB and GUID =PPP))");

                THEN("It succeeds")
                {
                    REQUIRE(l_parseResult);
                }
            }
        }

    }

    class EvalContext
    {
    public:

        const std::string& getFieldValue(FieldType a_fieldType) const
        {
            static std::string l_empty;
            const auto l_iterField= m_fields.find(a_fieldType);
            if (l_iterField == m_fields.end())
                return l_empty;
            return l_iterField->second;
        }

        EvalContext& operator()(FieldType a_fieldType, const std::string& a_fieldValue)
        {
            m_fields[a_fieldType] = a_fieldValue;
            return *this;
        }

    private:

        std::map<FieldType, std::string> m_fields;
    };

    SCENARIO("the parser can evaluate valid simple comparisons", "[parser]")
    {

        GIVEN("a parser")
        {
            Parser l_parser;

            WHEN("We parse 'CHANNEL == HELLO'")
            {
                const auto l_parseResult = l_parser.parse("CHANNEL == HELLO");
                REQUIRE(l_parseResult);

                AND_WHEN("We evaluate with context: CHANNEL=HELLO")
                {
                    const auto l_evalResult = l_parser.eval(EvalContext()(FieldType_Channel, "HELLO"));

                    THEN("It succeeds")
                    {
                        REQUIRE(l_evalResult);
                    }
                }

                AND_WHEN("We evaluate with context: CHANNEL=BOO, UUID=HELLO")
                {
                    const auto l_evalResult = l_parser.eval(EvalContext()(FieldType_Channel, "BOO")(FieldType_Guid, "HELLO"));

                    THEN("It fails")
                    {
                        REQUIRE(!l_evalResult);
                    }
                }
            }

            WHEN("We parse '(not (GUID == 123ljkl))'")
            {
                const auto l_parseResult = l_parser.parse("(not (GUID == 123ljkl))");
                REQUIRE(l_parseResult);

                AND_WHEN("We evaluate with context: UUID=PLOUF, GUID=123ljk")
                {
                    const auto l_evalResult = l_parser.eval(EvalContext()(FieldType_Guid, "123ljk")(FieldType_Uuid, "PLOUF"));

                    THEN("It succeeds")
                    {
                        REQUIRE(l_evalResult);
                    }
                }

                AND_WHEN("We evaluate with context: CHANNEL=BOO, UUID=123ljk")
                {
                    const auto l_evalResult = l_parser.eval(EvalContext()(FieldType_Channel, "BOO")(FieldType_Uuid, "123ljk"));

                    THEN("It succeeds")
                    {
                        REQUIRE(l_evalResult);
                    }
                }

                AND_WHEN("We evaluate with context: GUID=123ljkl")
                {
                    const auto l_evalResult = l_parser.eval(EvalContext()(FieldType_Guid, "123ljkl"));

                    THEN("It fails")
                    {
                        REQUIRE(!l_evalResult);
                    }
                }
            }

            WHEN("We parse 'not (UUID != PLOUF)'")
            {
                const auto l_parseResult = l_parser.parse("not (UUID != PLOUF)");
                REQUIRE(l_parseResult);

                AND_WHEN("We evaluate with context: CHANNEL=HELLO, UUID=PLOUF")
                {
                    const auto l_evalResult = l_parser.eval(EvalContext()(FieldType_Channel, "HELLO")(FieldType_Uuid, "PLOUF"));

                    THEN("It succeeds")
                    {
                        REQUIRE(l_evalResult);
                    }
                }

                AND_WHEN("We evaluate with context: CHANNEL=BOO, UUID=")
                {
                    const auto l_evalResult = l_parser.eval(EvalContext()(FieldType_Channel, "BOO")(FieldType_Uuid, ""));

                    THEN("It fails")
                    {
                        REQUIRE(!l_evalResult);
                    }
                }
            }

            WHEN("We parse 'GUID != 2 and GUID != 3'")
            {
                const auto l_parseResult = l_parser.parse("GUID != 2 and GUID != 3");
                REQUIRE(l_parseResult);

                AND_WHEN("We evaluate with context: GUID=4, UUID=PLOUF")
                {
                    const auto l_evalResult = l_parser.eval(EvalContext()(FieldType_Guid, "4")(FieldType_Uuid, "PLOUF"));

                    THEN("It succeeds")
                    {
                        REQUIRE(l_evalResult);
                    }
                }

                AND_WHEN("We evaluate with context: GUID=2")
                {
                    const auto l_evalResult = l_parser.eval(EvalContext()(FieldType_Guid, "2"));

                    THEN("It fails")
                    {
                        REQUIRE(!l_evalResult);
                    }
                }

                AND_WHEN("We evaluate with context: GUID=3")
                {
                    const auto l_evalResult = l_parser.eval(EvalContext()(FieldType_Guid, "3"));

                    THEN("It fails")
                    {
                        REQUIRE(!l_evalResult);
                    }
                }
            }

            WHEN("We parse 'GUID != 2 or UUID != 3'")
            {
                const auto l_parseResult = l_parser.parse("GUID != 2 or UUID != 3");
                REQUIRE(l_parseResult);

                AND_WHEN("We evaluate with context: GUID=4, UUID=3")
                {
                    const auto l_evalResult = l_parser.eval(EvalContext()(FieldType_Guid, "4")(FieldType_Uuid, "3"));

                    THEN("It succeeds")
                    {
                        REQUIRE(l_evalResult);
                    }
                }

                AND_WHEN("We evaluate with context: GUID=2, UUID=3")
                {
                    const auto l_evalResult = l_parser.eval(EvalContext()(FieldType_Guid, "2")(FieldType_Uuid, "3"));

                    THEN("It fails")
                    {
                        REQUIRE(!l_evalResult);
                    }
                }

                AND_WHEN("We evaluate with context: GUID=2, UUID=2")
                {
                    const auto l_evalResult = l_parser.eval(EvalContext()(FieldType_Guid, "2")(FieldType_Uuid, "2"));

                    THEN("It succeeds")
                    {
                        REQUIRE(l_evalResult);
                    }
                }
            }
        }

    }

    SCENARIO("the parser can detect invalid expressions", "[parser]")
    {

        GIVEN("a parser")
        {
            Parser l_parser;

            WHEN("We parse 'CHANNEL == HELLO and (UUID != 456 or GUID == qwesdf!!'")
            {
                const auto l_parseResult = l_parser.parse("CHANNEL == HELLO and (UUID != 456 or GUID == qwesdf!!");

                THEN("It fails")
                {
                    REQUIRE(!l_parseResult);
                }
            }

            WHEN("We parse '(CHANNEL === HELLO)'")
            {
                const auto l_parseResult = l_parser.parse("(CHANNEL === HELLO)");

                THEN("It fails")
                {
                    REQUIRE(!l_parseResult);
                }
            }

            WHEN("We parse '('")
            {
                const auto l_parseResult = l_parser.parse("(");

                THEN("It fails")
                {
                    REQUIRE(!l_parseResult);
                }
            }

            WHEN("We parse '(CHANNEL'")
            {
                const auto l_parseResult = l_parser.parse("(CHANNEL");

                THEN("It fails")
                {
                    REQUIRE(!l_parseResult);
                }
            }

            WHEN("We parse 'not(booo'")
            {
                const auto l_parseResult = l_parser.parse("not(booo");

                THEN("It fails")
                {
                    REQUIRE(!l_parseResult);
                }
            }

            WHEN("We parse 'not UUID == MEUH'")
            {
                const auto l_parseResult = l_parser.parse("not UUID == MEUH");

                THEN("It fails")
                {
                    REQUIRE(!l_parseResult);
                }
            }

            WHEN("We parse 'not (  GUID != MEUH  BAH'")
            {
                const auto l_parseResult = l_parser.parse("not (  GUID != MEUH  BAH");

                THEN("It fails")
                {
                    REQUIRE(!l_parseResult);
                }
            }

            WHEN("We parse 'UUID = 1 or ZOBI'")
            {
                const auto l_parseResult = l_parser.parse("UUID = 1 or ZOBI");

                THEN("It fails")
                {
                    REQUIRE(!l_parseResult);
                }
            }

            WHEN("We parse 'UUID = ,")
            {
                const auto l_parseResult = l_parser.parse("UUID = ,");

                THEN("It fails")
                {
                    REQUIRE(!l_parseResult);
                }
            }

            WHEN("We parse 'UUID !=)")
            {
                const auto l_parseResult = l_parser.parse("UUID !=)");

                THEN("It fails")
                {
                    REQUIRE(!l_parseResult);
                }
            }

            WHEN("We parse 'UUID !=")
            {
                const auto l_parseResult = l_parser.parse("UUID !=");

                THEN("It fails")
                {
                    REQUIRE(!l_parseResult);
                }
            }
        }

    }

}
