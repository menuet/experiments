
#pragma once

#include "lexer.hpp"
#include <memory>

namespace cellang { namespace parser {

    template <typename CharT>
    class Expression
    {
    public:

        using CharType = CharT;

        virtual const CharType* label() const = 0;
    };

    namespace detail {

        template <typename CharT>
        struct ErrorTraits
        {
            using CharType = CharT;
            static constexpr const CharType* label() { return "Error"; }
        };

        template <typename CharT>
        struct AssignmentTraits
        {
            using CharType = CharT;
            static constexpr const CharType* label() { return "Assignment"; }
        };

        template <typename CharT>
        struct OperationTraits
        {
            using CharType = CharT;
            static constexpr const CharType* label() { return "Operation"; }
        };

        template <typename CharT>
        struct FuncCallTraits
        {
            using CharType = CharT;
            static constexpr const CharType* label() { return "FuncCall"; }
        };

        template <typename CharT>
        struct FuncDefTraits
        {
            using CharType = CharT;
            static constexpr const CharType* label() { return "FuncDef"; }
        };

        template <typename CharT, template <typename> class TraitsT>
        class ExpressionBase : public Expression<CharT>
        {
        public:

            using CharType = CharT;

            const CharType* label() const override
            {
                return TraitsT<CharT>::label();
            }
        };

        template <typename CharT>
        class ExpressionError : public ExpressionBase<CharT, ErrorTraits>
        {
        public:
        };

        template <typename CharT>
        class ExpressionAssignment : public ExpressionBase<CharT, AssignmentTraits>
        {
        public:
        };

        template <typename CharT>
        class ExpressionOperation : public ExpressionBase<CharT, OperationTraits>
        {
        public:
        };

        template <typename CharT>
        class ExpressionFuncCall : public ExpressionBase<CharT, FuncCallTraits>
        {
        public:
        };

        template <typename CharT>
        class ExpressionFuncDef : public ExpressionBase<CharT, FuncDefTraits>
        {
        public:
        };

    } // namespace detail

    template <typename CharT>
    class ExpressionHolder
    {
    public:

        using CharType = CharT;

        ExpressionHolder(std::unique_ptr<Expression<CharType>> expr)
            : m_expr(std::move(expr))
        {
        }

        const CharType* label() const
        {
            return m_expr->label();
        }

    private:

        std::unique_ptr<Expression<CharType>> m_expr;
    };

    template <typename CharT>
    inline bool operator==(const ExpressionHolder<CharT>& eh1, const ExpressionHolder<CharT>& eh2)
    {
        return false;
    }

    template <typename CharT>
    inline bool operator!=(const ExpressionHolder<CharT>& eh1, const ExpressionHolder<CharT>& eh2)
    {
        return !operator==(eh1, eh2);
    }

    namespace detail {

        template <typename TokenIteratorT>
        struct ExprIterator
        {
        public:
            using TokenIteratorType = TokenIteratorT;
            using TokenType = typename std::iterator_traits<TokenIteratorType>::value_type;
            using CharType = typename TokenType::CharType;
            using iterator_category = std::input_iterator_tag;
            using value_type = ExpressionHolder<CharType>;
            using difference_type = std::ptrdiff_t;
            using pointer = ExpressionHolder<CharType>*;
            using reference = ExpressionHolder<CharType>&;

            ExprIterator(TokenIteratorType first, TokenIteratorType last)
                : m_current_input(first)
                , m_next_input(m_current_input)
                , m_last_input(last)
            {
            }

            void swap(ExprIterator& other)
            {
                using std::swap;
                swap(m_current_input, other.m_current_input);
                swap(m_next_input, other.m_next_input);
                swap(m_last_input, other.m_last_input);
            }

            ExprIterator& operator++()
            {
                assert(m_current_input != m_last_input);
                ++m_current_input;
                ++m_next_input;
                return *this;
            }

            ExprIterator operator++(int)
            {
                auto copy(*this);
                operator++();
                return copy;
            }

            value_type operator*()
            {
                assert(m_current_input != m_last_input);
                assert(m_current_input == m_next_input);
                return ExpressionHolder<CharType>(std::unique_ptr<Expression<CharType>>());
            }

            template <typename T>
            friend bool operator==(ExprIterator<T> iter1, ExprIterator<T> iter2);

        private:

            TokenIteratorType m_current_input;
            TokenIteratorType m_next_input;
            TokenIteratorType m_last_input;
        };

        template <typename TokenIteratorT>
        inline bool operator==(ExprIterator<TokenIteratorT> iter1, ExprIterator<TokenIteratorT> iter2)
        {
            return iter1.m_current_input == iter2.m_current_input;
        }

        template <typename TokenIteratorT>
        inline bool operator!=(ExprIterator<TokenIteratorT> iter1, ExprIterator<TokenIteratorT> iter2)
        {
            return !operator==(iter1, iter2);
        }

    } // namespace detail

    template <typename TokenIteratorT>
    class Parser
    {
    public:

        using TokenIteratorType = TokenIteratorT;
        using iterator = detail::ExprIterator<TokenIteratorType>;
        using value_type = typename std::iterator_traits<iterator>::value_type;
        using TokenType = typename std::iterator_traits<TokenIteratorType>::value_type;
        using CharType = typename TokenType::CharType;

        Parser(TokenIteratorType first, TokenIteratorType last)
            : m_first_input(first)
            , m_last_input(last)
        {
        }

        iterator begin() const
        {
            return iterator(m_first_input, m_last_input);
        }

        iterator end() const
        {
            return iterator(m_last_input, m_last_input);
        }

    private:

        TokenIteratorType m_first_input;
        TokenIteratorType m_last_input;
    };

} } // namespace cellang::parser
