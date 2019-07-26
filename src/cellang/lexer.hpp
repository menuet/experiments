
#pragma once

#include <istream>
#include <ostream>
#include <iterator>
#include <platform/variant.hpp>
#include <platform/string_view.hpp>
#include <cassert>
#include <cctype>
#include <utility>

namespace cellang { namespace lexer {

    namespace detail {

        template <typename CharT>
        struct ErrorTraits
        {
            using CharType = CharT;
            using TextType = std::string;
            static constexpr const CharType* label() { return "Error"; }
        };

        template <typename CharT>
        struct NumberTraits
        {
            using CharType = CharT;
            using TextType = stdnext::basic_string_view<CharT>;
            static constexpr const CharType* label() { return "Number"; }
        };

        template <typename CharT>
        struct StringTraits
        {
            using CharType = CharT;
            using TextType = stdnext::basic_string_view<CharT>;
            static constexpr const CharType* label() { return "String"; }
        };

        template <typename CharT>
        struct SymbolTraits
        {
            using CharType = CharT;
            using TextType = stdnext::basic_string_view<CharT>;
            static constexpr const CharType* label() { return "Symbol"; }
        };

        template <typename CharT>
        struct OperatorTraits
        {
            using CharType = CharT;
            using TextType = stdnext::basic_string_view<CharT>;
            static constexpr const CharType* label() { return "Operator"; }
        };

        template <typename CharT>
        struct PunctuationTraits
        {
            using CharType = CharT;
            using TextType = stdnext::basic_string_view<CharT>;
            static constexpr const CharType* label() { return "Punctuation"; }
        };

        template <typename CharT, template <typename > class TraitsT>
        struct TokenValue
        {
            using TraitsType = TraitsT<CharT>;
            using CharType = CharT;
            using TextType = typename TraitsType::TextType;
            static constexpr const CharType* label() { return TraitsType::label(); }
            TextType text{};
        };

        template <typename CharT, template <typename > class TraitsT>
        inline std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, const TokenValue<CharT, TraitsT>& tv)
        {
            os << TokenValue<CharT, TraitsT>::label() << ": " << tv.text;
            return os;
        }

        template <typename CharT, template <typename > class TraitsT>
        inline bool operator==(const TokenValue<CharT, TraitsT>& tv1, const TokenValue<CharT, TraitsT>& tv2)
        {
            return tv1.text == tv2.text;
        }

        template <typename CharT, template <typename > class TraitsT>
        inline bool operator!=(const TokenValue<CharT, TraitsT>& tv1, const TokenValue<CharT, TraitsT>& tv2)
        {
            return tv1.text != tv2.text;
        }

    } // namespace detail

    using Error = detail::TokenValue<char, detail::ErrorTraits>;
    using Number = detail::TokenValue<char, detail::NumberTraits>;
    using String = detail::TokenValue<char, detail::StringTraits>;
    using Symbol = detail::TokenValue<char, detail::SymbolTraits>;
    using Operator = detail::TokenValue<char, detail::OperatorTraits>;
    using Punctuation = detail::TokenValue<char, detail::PunctuationTraits>;

    template <typename CharT>
    struct Token
    {
        using CharType = CharT;
        stdnext::variant<Error, Number, String, Symbol, Operator, Punctuation> value{};
    };

    template <typename CharT>
    inline bool operator==(const Token<CharT>& t1, const Token<CharT>& t2)
    {
        return t1.value == t2.value;
    }

    template <typename CharT>
    inline bool operator!=(const Token<CharT>& t1, const Token<CharT>& t2)
    {
        return t1.value != t2.value;
    }

    template <typename CharT>
    inline std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, const Token<CharT>& token)
    {
        stdnext::visit([&](const auto& value)
        {
            os << value;
        }, token.value);
        return os;
    }

    namespace detail {

        template <typename CharT>
        inline bool is_space(CharT c)
        {
            return c == '\n' || c == ' ';
        }

        template <typename CharT>
        inline bool is_punctuation(CharT c)
        {
            return c== '(' || c == ')' || c == '{' || c == '}' || c == ',' || c == ';' || c == '=' || c == ':';
        }

        template <typename CharT>
        inline bool is_operator(CharT c)
        {
            return c == '+' || c == '-' || c == '*' || c == '/';
        }

        template <typename CharT>
        inline bool is_tab(CharT c)
        {
            return c == '\t';
        }

        template <typename CharT>
        inline bool is_quote(CharT c)
        {
            return c == '\'' || c == '"';
        }

        template <typename CharT>
        inline bool is_digit(CharT c)
        {
            return std::isdigit(c) || c == '.';
        }

        template <typename CharT>
        inline bool is_alpha(CharT c)
        {
            return std::isalpha(c) || c == '_';
        }

        template <typename CharIteratorT>
        struct ScanResult
        {
            using CharType = typename std::iterator_traits<CharIteratorT>::value_type;
            bool recognized{};
            Token<CharType> token{};
            CharIteratorT next_input{};
        };

        template <typename CharIteratorT>
        inline CharIteratorT skip_spaces(CharIteratorT current_input, CharIteratorT last_input)
        {
            while (current_input != last_input)
            {
                const auto c = *current_input;
                if (!is_space(c))
                    break;
                ++current_input;
            }
            return current_input;
        }

        template <typename CharIteratorT>
        inline ScanResult<CharIteratorT> scan_end(CharIteratorT current_input, CharIteratorT last_input)
        {
            using CharType = typename std::iterator_traits<CharIteratorT>::value_type;
            if (current_input != last_input)
                return ScanResult<CharIteratorT>{false};
            return ScanResult<CharIteratorT>{true, Token<CharType>{ Error{ "Unexpected end" } }, current_input};
        }

        template <typename CharIteratorT>
        inline ScanResult<CharIteratorT> scan_unexpected_char(CharIteratorT current_input, CharIteratorT last_input)
        {
            using CharType = typename std::iterator_traits<CharIteratorT>::value_type;
            assert(current_input != last_input);
            const auto c = *current_input;
            return ScanResult<CharIteratorT>{true, Token<CharType>{ Error{ std::string("Unexpected character: ") + c } }, skip_spaces(++current_input, last_input)};
        }

        template <typename TokenValueType, typename CharIteratorT, typename PredicateT>
        inline ScanResult<CharIteratorT> scan_char(CharIteratorT current_input, CharIteratorT last_input, PredicateT predicate)
        {
            using CharType = typename std::iterator_traits<CharIteratorT>::value_type;
            assert(current_input != last_input);
            const auto c = *current_input;
            if (!predicate(c))
                return ScanResult<CharIteratorT>{false};
            stdnext::string_view text(&*current_input, 1);
            return ScanResult<CharIteratorT>{true, Token<CharType>{ TokenValueType{ text } }, skip_spaces(++current_input, last_input)};
        }

        template <typename CharIteratorT>
        inline ScanResult<CharIteratorT> scan_tab(CharIteratorT current_input, CharIteratorT last_input)
        {
            using CharType = typename std::iterator_traits<CharIteratorT>::value_type;
            assert(current_input != last_input);
            const auto c = *current_input;
            if (!is_tab(c))
                return ScanResult<CharIteratorT>{false};
            return ScanResult<CharIteratorT>{true, Token<CharType>{ Error{ "Tabs are not allowed in Cell." } }, skip_spaces(++current_input, last_input)};
        }

        template <typename CharIteratorT>
        inline ScanResult<CharIteratorT> scan_punctuation(CharIteratorT current_input, CharIteratorT last_input)
        {
            using CharType = typename std::iterator_traits<CharIteratorT>::value_type;
            return scan_char<Punctuation>(current_input, last_input, &is_punctuation<CharType>);
        }

        template <typename CharIteratorT>
        inline ScanResult<CharIteratorT> scan_operator(CharIteratorT current_input, CharIteratorT last_input)
        {
            using CharType = typename std::iterator_traits<CharIteratorT>::value_type;
            return scan_char<Operator>(current_input, last_input, &is_operator<CharType>);
        }

        template <typename CharIteratorT>
        inline ScanResult<CharIteratorT> scan_string(CharIteratorT current_input, CharIteratorT last_input)
        {
            using CharType = typename std::iterator_traits<CharIteratorT>::value_type;
            assert(current_input != last_input);
            const auto quote = *current_input;
            if (!is_quote(quote))
                return ScanResult<CharIteratorT>{false};
            const auto string_start = ++current_input;
            while (current_input != last_input)
            {
                const auto c = *current_input;
                if (c == quote)
                {
                    stdnext::string_view text(&*string_start, std::distance(string_start, current_input));
                    return ScanResult<CharIteratorT>{true, Token<CharType>{ String{ text } }, skip_spaces(++current_input, last_input)};
                }
            }
            return ScanResult<CharIteratorT>{true, Token<CharType>{ Error{ "A string ran off the end of the program." } }, current_input};
        }

        template <typename TokenValueType, typename CharIteratorT, typename PredicateT>
        inline ScanResult<CharIteratorT> scan_sequence(CharIteratorT current_input, CharIteratorT last_input, PredicateT predicate)
        {
            using CharType = typename std::iterator_traits<CharIteratorT>::value_type;
            assert(current_input != last_input);
            const auto c = *current_input;
            if (!predicate(c))
                return ScanResult<CharIteratorT>{false};
            const auto sequence_start = current_input++;
            while (current_input != last_input)
            {
                const auto c = *current_input;
                if (!predicate(c))
                    break;
                ++current_input;
            }
            stdnext::string_view text(&*sequence_start, std::distance(sequence_start, current_input));
            return ScanResult<CharIteratorT>{true, Token<CharType>{ TokenValueType{ text } }, skip_spaces(current_input, last_input)};
        }

        template <typename CharIteratorT>
        inline ScanResult<CharIteratorT> scan_number(CharIteratorT current_input, CharIteratorT last_input)
        {
            using CharType = typename std::iterator_traits<CharIteratorT>::value_type;
            return scan_sequence<Number>(current_input, last_input, &is_digit<CharType>);
        }

        template <typename CharIteratorT>
        inline ScanResult<CharIteratorT> scan_symbol(CharIteratorT current_input, CharIteratorT last_input)
        {
            using CharType = typename std::iterator_traits<CharIteratorT>::value_type;
            return scan_sequence<Symbol>(current_input, last_input, &is_alpha<CharType>);
        }

        template <typename CharIteratorT>
        inline ScanResult<CharIteratorT> scan_token(CharIteratorT current_input, CharIteratorT last_input)
        {
            using CharType = typename std::iterator_traits<CharIteratorT>::value_type;
            current_input = skip_spaces(current_input, last_input);
            if (auto result = scan_end(current_input, last_input); result.recognized)
                return result;
            if (auto result = scan_punctuation(current_input, last_input); result.recognized)
                return result;
            if (auto result = scan_operator(current_input, last_input); result.recognized)
                return result;
            if (auto result = scan_string(current_input, last_input); result.recognized)
                return result;
            if (auto result = scan_number(current_input, last_input); result.recognized)
                return result;
            if (auto result = scan_symbol(current_input, last_input); result.recognized)
                return result;
            if (auto result = scan_tab(current_input, last_input); result.recognized)
                return result;
            return scan_unexpected_char(current_input, last_input);
        }

        template <typename CharIteratorT>
        struct TokenIterator
        {
        public:
            using CharIteratorType = CharIteratorT;
            using CharType = typename std::iterator_traits<CharIteratorType>::value_type;
            using iterator_category = std::input_iterator_tag;
            using value_type = Token<CharType>;
            using difference_type = std::ptrdiff_t;
            using pointer = Token<CharType>*;
            using reference = Token<CharType>&;

            TokenIterator(CharIteratorType first, CharIteratorType last)
                : m_current_input(skip_spaces(first, last))
                , m_next_input(m_current_input)
                , m_last_input(last)
            {
            }

            void swap(TokenIterator& other)
            {
                using std::swap;
                swap(m_current_input, other.m_current_input);
                swap(m_next_input, other.m_next_input);
                swap(m_last_input, other.m_last_input);
            }

            TokenIterator& operator++()
            {
                assert(m_current_input != m_last_input);
                if (m_current_input == m_next_input)
                    m_next_input = scan_token(m_current_input, m_last_input).next_input;
                m_current_input = m_next_input;
                return *this;
            }

            TokenIterator operator++(int)
            {
                auto copy(*this);
                operator++();
                return copy;
            }

            value_type operator*()
            {
                assert(m_current_input != m_last_input);
                assert(m_current_input == m_next_input);
                auto[recognized, token, next_input] = scan_token(m_current_input, m_last_input);
                m_next_input = next_input;
                return token;
            }

            template <typename T>
            friend bool operator==(TokenIterator<T> iter1, TokenIterator<T> iter2);

        private:

            CharIteratorType m_current_input;
            CharIteratorType m_next_input;
            CharIteratorType m_last_input;
        };

        template <typename CharIteratorT>
        inline bool operator==(TokenIterator<CharIteratorT> iter1, TokenIterator<CharIteratorT> iter2)
        {
            return iter1.m_current_input == iter2.m_current_input;
        }

        template <typename CharIteratorT>
        inline bool operator!=(TokenIterator<CharIteratorT> iter1, TokenIterator<CharIteratorT> iter2)
        {
            return !operator==(iter1, iter2);
        }

    } // namespace detail

    template <typename CharIteratorT>
    class Lexer
    {
    public:

        using CharIteratorType = CharIteratorT;
        using iterator = detail::TokenIterator<CharIteratorType>;
        using value_type = typename std::iterator_traits<iterator>::value_type;
        using CharType = typename std::iterator_traits<iterator>::value_type;

        Lexer(CharIteratorType first, CharIteratorType last)
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

        CharIteratorType m_first_input;
        CharIteratorType m_last_input;
    };

} } // namespace cellang::lexer
