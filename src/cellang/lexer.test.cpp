

#include "lexer.hpp"
#include <catch2/catch.hpp>
#include <string_view>
#include <iostream>
#include <vector>

TEST_CASE("lexer", "[lexer]")
{
    SECTION("number")
    {
        constexpr stdnext::string_view CELLANG_SAMPLE = "3";

        cellang::lexer::Lexer lexer(begin(CELLANG_SAMPLE), end(CELLANG_SAMPLE));

        const std::vector<cellang::lexer::Token<char>> expected_tokens =
        {
            { cellang::lexer::Number{ "3" } },
        };

        const std::vector<cellang::lexer::Token<char>> scanned_tokens(lexer.begin(), lexer.end());

        REQUIRE(scanned_tokens == expected_tokens);
    }

    SECTION("line break")
    {
        constexpr stdnext::string_view CELLANG_SAMPLE = "\n";

        cellang::lexer::Lexer lexer(begin(CELLANG_SAMPLE), end(CELLANG_SAMPLE));

        const std::vector<cellang::lexer::Token<char>> expected_tokens;

        const std::vector<cellang::lexer::Token<char>> scanned_tokens(lexer.begin(), lexer.end());

        REQUIRE(scanned_tokens == expected_tokens);
    }

    SECTION("Spaces then number then line break")
    {
        constexpr stdnext::string_view CELLANG_SAMPLE = "  123\n";

        cellang::lexer::Lexer lexer(begin(CELLANG_SAMPLE), end(CELLANG_SAMPLE));

        const std::vector<cellang::lexer::Token<char>> expected_tokens =
        {
            { cellang::lexer::Number{ "123" } },
        };

        const std::vector<cellang::lexer::Token<char>> scanned_tokens(lexer.begin(), lexer.end());

        REQUIRE(scanned_tokens == expected_tokens);
    }

    SECTION("Parenthesis then semicolon then then line break")
    {
        constexpr stdnext::string_view CELLANG_SAMPLE = ");\n";

        cellang::lexer::Lexer lexer(begin(CELLANG_SAMPLE), end(CELLANG_SAMPLE));

        const std::vector<cellang::lexer::Token<char>> expected_tokens =
        {
            { cellang::lexer::Punctuation{ ")" } },
            { cellang::lexer::Punctuation{ ";" } },
        };

        const std::vector<cellang::lexer::Token<char>> scanned_tokens(lexer.begin(), lexer.end());

        REQUIRE(scanned_tokens == expected_tokens);
    }

    SECTION("symbol then unexpected then operator then space")
    {
        constexpr stdnext::string_view CELLANG_SAMPLE = "ABC $ + ";

        cellang::lexer::Lexer lexer(begin(CELLANG_SAMPLE), end(CELLANG_SAMPLE));

        const std::vector<cellang::lexer::Token<char>> expected_tokens =
        {
            { cellang::lexer::Symbol{ "ABC" } },
            { cellang::lexer::Error{ "Unexpected character: $" } },
            { cellang::lexer::Operator{ "+" } },
        };

        const std::vector<cellang::lexer::Token<char>> scanned_tokens(lexer.begin(), lexer.end());

        REQUIRE(scanned_tokens == expected_tokens);
    }

    SECTION("sample")
    {
        constexpr stdnext::string_view CELLANG_SAMPLE = R"(
x = 3;
y = x + 2;
print(y);
)";

        cellang::lexer::Lexer lexer(begin(CELLANG_SAMPLE), end(CELLANG_SAMPLE));

        const std::vector<cellang::lexer::Token<char>> expected_tokens =
        {
            { cellang::lexer::Symbol{ "x" } },
            { cellang::lexer::Punctuation{ "=" } },
            { cellang::lexer::Number{ "3" } },
            { cellang::lexer::Punctuation{ ";" } },
            { cellang::lexer::Symbol{ "y" } },
            { cellang::lexer::Punctuation{ "=" } },
            { cellang::lexer::Symbol{ "x" } },
            { cellang::lexer::Operator{ "+" } },
            { cellang::lexer::Number{ "2" } },
            { cellang::lexer::Punctuation{ ";" } },
            { cellang::lexer::Symbol{ "print" } },
            { cellang::lexer::Punctuation{ "(" } },
            { cellang::lexer::Symbol{ "y" } },
            { cellang::lexer::Punctuation{ ")" } },
            { cellang::lexer::Punctuation{ ";" } },
        };

        const std::vector<cellang::lexer::Token<char>> scanned_tokens(lexer.begin(), lexer.end());

        REQUIRE(scanned_tokens == expected_tokens);
    }

    SECTION("sample no spaces")
    {
        constexpr stdnext::string_view CELLANG_SAMPLE = "x=3;y=x+2;print(y);";

        cellang::lexer::Lexer lexer(begin(CELLANG_SAMPLE), end(CELLANG_SAMPLE));

        const std::vector<cellang::lexer::Token<char>> expected_tokens =
        {
            { cellang::lexer::Symbol{ "x" } },
            { cellang::lexer::Punctuation{ "=" } },
            { cellang::lexer::Number{ "3" } },
            { cellang::lexer::Punctuation{ ";" } },
            { cellang::lexer::Symbol{ "y" } },
            { cellang::lexer::Punctuation{ "=" } },
            { cellang::lexer::Symbol{ "x" } },
            { cellang::lexer::Operator{ "+" } },
            { cellang::lexer::Number{ "2" } },
            { cellang::lexer::Punctuation{ ";" } },
            { cellang::lexer::Symbol{ "print" } },
            { cellang::lexer::Punctuation{ "(" } },
            { cellang::lexer::Symbol{ "y" } },
            { cellang::lexer::Punctuation{ ")" } },
            { cellang::lexer::Punctuation{ ";" } },
        };

        const std::vector<cellang::lexer::Token<char>> scanned_tokens(lexer.begin(), lexer.end());

        REQUIRE(scanned_tokens == expected_tokens);
    }
}
