
#include <catch2/catch.hpp>
#include "safe_printf.h"
#include <iostream>
#include <array>


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

        template< size_t ArgsStringsArraySizeV >
        struct ArgsStringsHolder
        {
            ArgsStringsHolder() = default;

            ArgsStringsHolder(const std::array<const char*, ArgsStringsArraySizeV>& argsStrings)
                : m_pArgsStrings(&argsStrings)
            {
            }

            std::string buildWhenTitle(const char* format) const
            {
                std::string whenTitle = "    When: safe_sprintf is called with the format ";
                whenTitle += format;
                if (m_pArgsStrings)
                {
                    whenTitle += " and the arg ";
                    whenTitle += (*m_pArgsStrings)[0];
                    for (auto argIndex = 1U; argIndex < ArgsStringsArraySizeV; ++argIndex)
                    {
                        whenTitle += " and the arg ";
                        whenTitle += (*m_pArgsStrings)[argIndex];
                    }
                }
                return whenTitle;
            }

            const std::array<const char*, ArgsStringsArraySizeV>* m_pArgsStrings = nullptr;
        };

        ArgsStringsHolder<1> make_args_strings_holder()
        {
            return ArgsStringsHolder<1>();
        }

        template< size_t ArgsStringsArraySizeV >
        ArgsStringsHolder<ArgsStringsArraySizeV> make_args_strings_holder(const std::array<const char*, ArgsStringsArraySizeV>& argsStrings)
        {
            return ArgsStringsHolder<ArgsStringsArraySizeV>(argsStrings);
        }

        template< size_t ExpectedBufferSizeV >
        struct ResultHolder
        {
            using ArrayType = const char [ExpectedBufferSizeV];

            ResultHolder() = default;

            ResultHolder(ArrayType& expectedBuffer)
                : m_pExpectedBuffer(&expectedBuffer)
            {
            }

            std::string buildThenTitle() const
            {
                std::string thenTitle = "    Then: the result is ";
                if (m_pExpectedBuffer)
                {
                    thenTitle += std::to_string((int) (ExpectedBufferSizeV - 1));
                    thenTitle += " and the buffer contains ";
                    thenTitle += *m_pExpectedBuffer;
                }
                else
                {
                    thenTitle += std::to_string(-1);
                    thenTitle += " and the buffer is not modified";
                }
                return thenTitle;
            }

            template< typename BufferT >
            void assertExpectations(int result, BufferT& buffer) const
            {
                if (m_pExpectedBuffer)
                {
                    REQUIRE(result == ExpectedBufferSizeV - 1);
                    REQUIRE(buffer == *m_pExpectedBuffer);
                }
                else
                {
                    REQUIRE(result == -1);
                    REQUIRE(!buffer.isModified());
                }
            }

            ArrayType* m_pExpectedBuffer = nullptr;
        };

        ResultHolder<1> make_result_holder()
        {
            return ResultHolder<1>();
        }

        template< size_t ExpectedBufferSizeV >
        ResultHolder<ExpectedBufferSizeV> make_result_holder(const char(&expectedBuffer)[ExpectedBufferSizeV])
        {
            return ResultHolder<ExpectedBufferSizeV>(expectedBuffer);
        }

        template< typename BufferT, typename ResultHolderT, typename ArgsStringsHolderT, typename... ArgTs >
        void call_safe_sprintf(BufferT& buffer, const char* format, const ResultHolderT& resultHolder, const ArgsStringsHolderT& argsStringsHolder, const ArgTs&... args)
        {
            SECTION(argsStringsHolder.buildWhenTitle(format))
            {
                const auto result = pasc::safe_sprintf(buffer.getArray(), format, args...);

                SECTION(resultHolder.buildThenTitle())
                {
                    resultHolder.assertExpectations(result, buffer);
                }
            }
        }

    } // anonymous namespace


#define WHEN_SAFE_SPRINTF_IS_CALLED(buffer, format, args, result) call_safe_sprintf(buffer, format, result, args)
#define WITH_ZERO_ARG() make_args_strings_holder()
#define WITH_ONE_ARG(arg) make_args_strings_holder(std::array<const char*, 1>{#arg}), arg
#define WITH_TWO_ARGS(arg0, arg1) make_args_strings_holder(std::array<const char*, 2>{#arg0, #arg1}), arg0, arg1
#define THEN_RESULT_IS_ERROR() make_result_holder()
#define THEN_RESULT_IS(expectedBuffer) make_result_holder(expectedBuffer)


    SCENARIO("safe_sprintf can be called with 0 parameter", "[printf]")
    {

        GIVEN("a buffer of 10 characters")
        {
            Buffer<10> buffer;

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "12345", WITH_ZERO_ARG(), THEN_RESULT_IS("12345"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "123456789", WITH_ZERO_ARG(), THEN_RESULT_IS("123456789"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "1234567890", WITH_ZERO_ARG(), THEN_RESULT_IS_ERROR());

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "12345678901234", WITH_ZERO_ARG(), THEN_RESULT_IS_ERROR());

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "0123%d456", WITH_ZERO_ARG(), THEN_RESULT_IS_ERROR());
        }

    }

    SCENARIO("safe_sprintf can be called with 1 parameter", "[printf]")
    {

        GIVEN("a buffer of 30 characters")
        {
            Buffer<30> buffer;

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "12345", WITH_ONE_ARG(42), THEN_RESULT_IS_ERROR());

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "12%s34%d", WITH_ONE_ARG(42), THEN_RESULT_IS_ERROR());

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%%%c$", WITH_ONE_ARG('A'), THEN_RESULT_IS("%A$"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "he-%s++", WITH_ONE_ARG("BOO"), THEN_RESULT_IS("he-BOO++"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%d!!!", WITH_ONE_ARG(42), THEN_RESULT_IS("42!!!"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, " %i!!!", WITH_ONE_ARG(42), THEN_RESULT_IS(" 42!!!"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "Octal:%o", WITH_ONE_ARG(42), THEN_RESULT_IS("Octal:52"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "0x%x0x", WITH_ONE_ARG(15), THEN_RESULT_IS("0xf0x"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "0X%X0X", WITH_ONE_ARG(15), THEN_RESULT_IS("0XF0X"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, ".%u.", WITH_ONE_ARG(1234), THEN_RESULT_IS(".1234."));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%f****", WITH_ONE_ARG(23.45), THEN_RESULT_IS("23.450000****"));

#if TODO_INVESTIGATE_WHY_PERCENT_F_DOES_NOT_WORK
            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%F****", WITH_ONE_ARG(23.45), THEN_RESULT_IS("23.450000****"));
#endif

            // TODO: fix
            // WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%e****", WITH_ONE_ARG(23.45), THEN_RESULT_IS("2.345000e+001****"));

            // TODO: fix
            // WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%E****", WITH_ONE_ARG(23.45), THEN_RESULT_IS("2.345000E+001****"));

            // TODO: fix
            // WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%a/hex", WITH_ONE_ARG(23.45), THEN_RESULT_IS("0x1.773333p+4/hex"));

            // TODO: fix
            // WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%A/hex", WITH_ONE_ARG(23.45), THEN_RESULT_IS("0X1.773333P+4/hex"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "???%g???", WITH_ONE_ARG(23.45), THEN_RESULT_IS("???23.45???"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "???%G???", WITH_ONE_ARG(23.45), THEN_RESULT_IS("???23.45???"));

#if TODO_DEBUG_PERCENT_N
            SECTION("SPECIAL %n")
            {
                _set_printf_count_output(1);
                int n = 0;
                WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "evil%n!", WITH_ONE_ARG(&n), THEN_RESULT_IS("evil!"));
                REQUIRE(n == 3);
                _set_printf_count_output(0);
            }
#endif

            // TODO: fix 64 bits
            // WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "p:%p", WITH_ONE_ARG((void*) 0x12345678), THEN_RESULT_IS("p:12345678"));
        }

    }

    SCENARIO("safe_sprintf can be called with 2 parameters", "[printf]")
    {

        GIVEN("a buffer of 30 characters")
        {
            Buffer<30> buffer;

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "12345", WITH_TWO_ARGS(42, "ABC"), THEN_RESULT_IS_ERROR());

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "12%s34", WITH_TWO_ARGS(42, "ABC"), THEN_RESULT_IS_ERROR());

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%li%s%s", WITH_TWO_ARGS(42, "ABC"), THEN_RESULT_IS_ERROR());

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "12%s34%d", WITH_TWO_ARGS(42, "ABC"), THEN_RESULT_IS_ERROR());

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%%%c$%lld", WITH_TWO_ARGS('A', 123LL), THEN_RESULT_IS("%A$123"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "he%f-%s++", WITH_TWO_ARGS(12345.5, "BOO"), THEN_RESULT_IS("he12345.500000-BOO++"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%+010d!!!%i", WITH_TWO_ARGS(42, 42), THEN_RESULT_IS("+000000042!!!42"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, " %i!!!0x%08X", WITH_TWO_ARGS(42, 0x123), THEN_RESULT_IS(" 42!!!0x00000123"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "Oct%cl:%o", WITH_TWO_ARGS('a', 42), THEN_RESULT_IS("Octal:52"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "0x%x%X0x", WITH_TWO_ARGS(255, 15), THEN_RESULT_IS("0xffF0x"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "0X%X0X%s", WITH_TWO_ARGS(15, "QWERTY"), THEN_RESULT_IS("0XF0XQWERTY"));

            // TODO: fix
            // WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%e.%u.", WITH_TWO_ARGS(1234., 1234), THEN_RESULT_IS("1.234000e+003.1234."));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%f**%.1f**", WITH_TWO_ARGS(23.45, 23.45), THEN_RESULT_IS("23.450000**23.4**"));

            // TODO: fix
            // WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%e**%llu**", WITH_TWO_ARGS(23.45, 123LLU), THEN_RESULT_IS("2.345000e+001**123**"));

            // TODO: fix
            // WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%E**%.3f**", WITH_TWO_ARGS(23.45, 987.12), THEN_RESULT_IS("2.345000E+001**987.120**"));

            // TODO: fix
            // WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%a/he%010.10sx", WITH_TWO_ARGS(23.45, "MIAM"), THEN_RESULT_IS("0x1.773333p+4/he000000MIAMx"));

            // TODO: fix
            // WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%A/hex%xAsterix%%", WITH_TWO_ARGS(23.45, 0x987), THEN_RESULT_IS("0X1.773333P+4/hex987Asterix%"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "???%g|%g???", WITH_TWO_ARGS(23.45, 98.6543), THEN_RESULT_IS("???23.45|98.6543???"));

            WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "%s=%G", WITH_TWO_ARGS("result", 23.45), THEN_RESULT_IS("result=23.45"));

            // TODO: fix 64 bits
            // WHEN_SAFE_SPRINTF_IS_CALLED(buffer, "p:%p-%08x", WITH_TWO_ARGS((void*) 0x12345678, 0xABCDEF), THEN_RESULT_IS("p:12345678-00abcdef"));
        }

    }

} // namespace ut
