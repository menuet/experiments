

#include "parser.hpp"
#include <catch.hpp>

using ExprHolder = cellang::parser::ExpressionHolder<char>;
using Expr = cellang::parser::Expression<char>;

static ExprHolder makeAssignment()
{
    return ExprHolder(std::unique_ptr<Expr>(std::make_unique<cellang::parser::detail::ExpressionAssignment<char>>()));
}

static ExprHolder makeFuncCall()
{
    return ExprHolder(std::unique_ptr<Expr>(std::make_unique<cellang::parser::detail::ExpressionFuncCall<char>>()));
}

TEST_CASE("parser", "[parser]")
{
    SECTION("constructor")
    {
        const std::vector<cellang::lexer::Token<char>> SAMPLE_TOKENS =
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

        cellang::parser::Parser parser(begin(SAMPLE_TOKENS), end(SAMPLE_TOKENS));

        std::vector<cellang::parser::ExpressionHolder<char>> expected_expressions;
        expected_expressions.push_back(makeAssignment());
        expected_expressions.push_back(makeAssignment());
        expected_expressions.push_back(makeFuncCall());

        const std::vector<ExprHolder> parsed_expressions(parser.begin(), parser.end());

//        REQUIRE(parsed_expressions == expected_expressions);
    }

}
