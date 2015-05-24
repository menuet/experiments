
#include "catch.hpp"
#include "safe_printf.h"
#include <iostream>


namespace ut {

    namespace {

        template< size_t ArraySizeV >
        class Buffer
        {
        public:

            using ArrayRef = char(&)[ArraySizeV];
            using ConstArrayRef = const char(&)[ArraySizeV];

            Buffer()
            {
                std::memset(m_array, INITIAL_CHAR, ArraySizeV);
            }

            ArrayRef getArray()
            {
                return m_array;
            }

            ConstArrayRef getArray() const
            {
                return m_array;
            }

            template< size_t StringSizeV >
            bool isEqualTo(const char(&string)[StringSizeV]) const
            {
                static_assert(ArraySizeV >= StringSizeV, "");

                using std::begin;
                using std::end;

                if (!std::equal(begin(string), end(string), m_array))
                    return false;

                return std::all_of(begin(m_array) + StringSizeV, end(m_array), [](char c) { return c == INITIAL_CHAR; });
            }

            bool isModified() const
            {
                using std::begin;
                using std::end;

                return std::any_of(begin(m_array), end(m_array), [](char c) { return c != INITIAL_CHAR; });
            }

        private:

            static const char INITIAL_CHAR = 127;

            char m_array[ArraySizeV];
        };

        template< size_t ArraySizeV, size_t StringSizeV >
        bool operator==(const Buffer<ArraySizeV>& buffer, const char(&string)[StringSizeV])
        {
            return buffer.isEqualTo(string);
        }

        template< size_t ArraySizeV >
        std::string toString(const Buffer<ArraySizeV>& buffer)
        {
            return std::string(buffer.getArray(), ArraySizeV);
        }

        template< size_t ArraySizeV >
        std::ostream& operator<<(std::ostream& os, const Buffer<ArraySizeV>& buffer)
        {
            os << toString(buffer);
            return os;
        }

        template< size_t ArraySizeV, typename ArgT >
        struct SafeSPrintfCaller
        {
            SafeSPrintfCaller(Buffer<ArraySizeV>& buffer, const char* format, const char* argAsString, const ArgT& arg)
                : m_buffer(buffer)
                , m_format(format)
                , m_argAsString(argAsString)
                , m_arg(arg)
            {
            }

            template< size_t ExpectedBufferSizeV >
            void then(const char(&expectedBuffer)[ExpectedBufferSizeV]) const
            {
                std::string whenTitle = "    When: safe_sprintf is called with the format ";
                whenTitle += m_format;
                whenTitle += " and the arg ";
                whenTitle += m_argAsString;

                SECTION(whenTitle)
                {
                    static const auto expectedBufferLength = ExpectedBufferSizeV - 1;
                    const auto result = pasc::safe_sprintf(m_buffer.getArray(), m_format, m_arg);
                    std::string thenTitle = "    Then: the result is ";
                    thenTitle += std::to_string((int)expectedBufferLength);
                    thenTitle += " and the buffer contains ";
                    thenTitle += expectedBuffer;
                    SECTION(thenTitle)
                    {
                        REQUIRE(result == expectedBufferLength);
                        REQUIRE(m_buffer == expectedBuffer);
                    }
                }
            }

            Buffer<ArraySizeV>& m_buffer;
            const char* m_format;
            const char* m_argAsString;
            const ArgT& m_arg;
        };

        template< size_t ArraySizeV, typename ArgT >
        SafeSPrintfCaller<ArraySizeV, ArgT> createSafeSPrintfCaller(Buffer<ArraySizeV>& buffer, const char* format, const char* argAsString, const ArgT& arg)
        {
            return SafeSPrintfCaller<ArraySizeV, ArgT>(buffer, format, argAsString, arg);
        }

    } // anonymous namespace

#define WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, format, arg) createSafeSPrintfCaller(buffer, format, #arg, arg)
#define THEN_RESULT_IS(expectedBuffer) then(expectedBuffer)

    SCENARIO("safe_sprintf can be called with 0 parameter", "[printf]")
    {

        GIVEN("a buffer of 10 characters")
        {
            Buffer<10> buffer;

            WHEN("safe_sprintf is called with the format 12345")
            {
                const auto result = pasc::safe_sprintf(buffer.getArray(), "12345");
                THEN("the result is 5 and the buffer contains 12345")
                {
                    REQUIRE(result == 5);
                    REQUIRE(buffer.isEqualTo("12345"));
                }
            }

            WHEN("safe_sprintf is called with the format 123456789")
            {
                const auto result = pasc::safe_sprintf(buffer.getArray(), "123456789");
                THEN("the result is 9 and the buffer contains 123456789")
                {
                    REQUIRE(result == 9);
                    REQUIRE(buffer.isEqualTo("123456789"));
                }
            }

            WHEN("safe_sprintf is called with the format 1234567890 (too much characters)")
            {
                const auto result = pasc::safe_sprintf(buffer.getArray(), "1234567890");
                THEN("the result is -1 and the buffer is not modified")
                {
                    REQUIRE(result == -1);
                    REQUIRE(!buffer.isModified());
                }
            }

            WHEN("safe_sprintf is called with the format 12345678901234 (really too much characters)")
            {
                const auto result = pasc::safe_sprintf(buffer.getArray(), "12345678901234");
                THEN("the result is -1 and the buffer is not modified")
                {
                    REQUIRE(result == -1);
                    REQUIRE(!buffer.isModified());
                }
            }

            WHEN("safe_sprintf is called with the format 0123%d456 (too much specifiers)")
            {
                const auto result = pasc::safe_sprintf(buffer.getArray(), "0123%d456");
                THEN("the result is -1 and the buffer is not modified")
                {
                    REQUIRE(result == -1);
                    REQUIRE(!buffer.isModified());
                }
            }
        }

    }

    SCENARIO("safe_sprintf can be called with 1 parameter", "[printf]")
    {

        GIVEN("a buffer of 30 characters")
        {
            Buffer<30> buffer;

            WHEN("safe_sprintf is called with the format 12345 (not enough specifiers)")
            {
                const auto result = pasc::safe_sprintf(buffer.getArray(), "12345", 1);
                THEN("the result is -1 and the buffer is not modified")
                {
                    REQUIRE(result == -1);
                    REQUIRE(!buffer.isModified());
                }
            }

            WHEN("safe_sprintf is called with the format 12%s34%d (too much specifiers)")
            {
                const auto result = pasc::safe_sprintf(buffer.getArray(), "12%s34%d", 1);
                THEN("the result is -1 and the buffer is not modified")
                {
                    REQUIRE(result == -1);
                    REQUIRE(!buffer.isModified());
                }
            }

            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, "%%%c$", 'A').THEN_RESULT_IS("%A$");

            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, "he-%s++", "BOO").THEN_RESULT_IS("he-BOO++");

            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, "%d!!!", 42).THEN_RESULT_IS("42!!!");

            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, " %i!!!", 42).THEN_RESULT_IS(" 42!!!");

            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, "Octal:%o", 42).THEN_RESULT_IS("Octal:52");

            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, "0x%x0x", 15).THEN_RESULT_IS("0xf0x");

            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, "0X%X0X", 15).THEN_RESULT_IS("0XF0X");

            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, ".%u.", 1234).THEN_RESULT_IS(".1234.");

            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, "%f****", 23.45).THEN_RESULT_IS("23.450000****");

#if TODO_INVESTIGATE_WHY_PERCENT_F_DOES_NOT_WORK
            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, "%F****", 23.45).THEN_RESULT_IS("23.450000****");
#endif

            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, "%e****", 23.45).THEN_RESULT_IS("2.345000e+001****");

            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, "%E****", 23.45).THEN_RESULT_IS("2.345000E+001****");

            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, "%a/hex", 23.45).THEN_RESULT_IS("0x1.773333p+4/hex");

            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, "%A/hex", 23.45).THEN_RESULT_IS("0X1.773333P+4/hex");

            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, "???%g???", 23.45).THEN_RESULT_IS("???23.45???");

            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, "???%G???", 23.45).THEN_RESULT_IS("???23.45???");

#if TODO_IMPLEMENT_PERCENT_N
            int n = 0;
            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, "evil%n!", n).THEN_RESULT_IS("evil");
#endif

            WHEN_SAFE_SPRINTF_IS_CALLED_WITH(buffer, "p:%p", (void*)0x12345678).THEN_RESULT_IS("p:12345678");
        }

    }

} // namespace ut
