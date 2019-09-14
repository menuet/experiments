
#pragma once


#include <cstdio>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <type_traits>


namespace pasc { namespace detail {

    struct StringView
    {
        StringView() = default;

        StringView(const char* string)
            : m_begin(string)
            , m_end(string + std::strlen(string))
        {
        }

        StringView(const char* begin, const char* end)
            : m_begin(begin)
            , m_end(end)
        {
        }

        const char* m_begin = nullptr;
        const char* m_end = nullptr;
    };

    struct Specifier : public StringView
    {
        const char* m_flags = nullptr;
        const char* m_minWidth = nullptr;
        const char* m_precision = nullptr;
        const char* m_lengthModifier = nullptr;
        const char* m_typeSpecifier = nullptr;
    };

    inline bool isFlag(char character)
    {
        switch (character)
        {
        case '-': case '+': case ' ': case '#': case '0':
            return true;
        }
        return false;
    }

    inline bool isStar(char character)
    {
        return character == '*';
    }

    inline bool isDot(char character)
    {
        return character == '.';
    }

    inline bool parseFlags(StringView& formatView, Specifier& specifier)
    {
        if (formatView.m_begin == formatView.m_end)
            return false;
        if (!isFlag(*formatView.m_begin))
            return true;
        specifier.m_flags = formatView.m_begin++;
        while (formatView.m_begin != formatView.m_end && isFlag(*formatView.m_begin))
            ++formatView.m_begin;
        return true;
    }

    inline bool parseMinWidth(StringView& formatView, Specifier& specifier)
    {
        if (formatView.m_begin == formatView.m_end)
            return false;
        if (isStar(*formatView.m_begin))
        {
            specifier.m_minWidth = formatView.m_begin++;
            return true;
        }
        if (std::isdigit(*formatView.m_begin))
        {
            specifier.m_minWidth = formatView.m_begin++;
            while (formatView.m_begin != formatView.m_end && std::isdigit(*formatView.m_begin))
                ++formatView.m_begin;
            return true;
        }
        return true;
    }

    inline bool parsePrecision(StringView& formatView, Specifier& specifier)
    {
        if (formatView.m_begin == formatView.m_end)
            return false;
        if (!isDot(*formatView.m_begin))
            return true;
        formatView.m_begin++;
        if (formatView.m_begin == formatView.m_end)
            return false;
        if (isStar(*formatView.m_begin))
        {
            specifier.m_precision = formatView.m_begin++;
            return true;
        }
        if (std::isdigit(*formatView.m_begin))
        {
            specifier.m_precision = formatView.m_begin++;
            while (formatView.m_begin != formatView.m_end && std::isdigit(*formatView.m_begin))
                ++formatView.m_begin;
            return true;
        }
        return false; // Error: '.' must be followed by '*' or an integer number
    }

    inline bool parseLengthModifier(StringView& formatView, Specifier& specifier)
    {
        if (formatView.m_begin == formatView.m_end)
            return false;
        switch (*formatView.m_begin)
        {
        case 'j': case 'z': case 't': case 'L':
            specifier.m_lengthModifier = formatView.m_begin++;
            break;
        case 'h':case 'l':
            specifier.m_lengthModifier = formatView.m_begin++;
            if (formatView.m_begin != formatView.m_end && *formatView.m_begin == *specifier.m_lengthModifier)
                    ++formatView.m_begin;
            break;
        }
        return true;
    }

    inline bool parseTypeSpecifier(StringView& formatView, Specifier& specifier)
    {
        if (formatView.m_begin == formatView.m_end)
            return false;
        switch (*formatView.m_begin)
        {
        case '%':
        case 'c':
        case 's':
        case 'd': case 'i':
        case 'o':
        case 'x': case 'X':
        case 'u':
        case 'f': case 'F':
        case 'e': case 'E':
        case 'a': case 'A':
        case 'g': case 'G':
        case 'n':
        case 'p':
            specifier.m_typeSpecifier = formatView.m_begin++;
            return true;
        }
        return false;
    }

    inline Specifier parseSpecifier(StringView& formatView)
    {
        Specifier specifier;
        formatView.m_begin = std::find(formatView.m_begin, formatView.m_end, '%');
        for (; formatView.m_begin != formatView.m_end; formatView.m_begin = std::find(formatView.m_begin, formatView.m_end, '%'))
        {
            const auto begin = formatView.m_begin++;

            if (!parseFlags(formatView, specifier))
                break; // Error parsing the flags

            if (!parseMinWidth(formatView, specifier))
                break; // Error parsing the min width

            if (!parsePrecision(formatView, specifier))
                break; // Error parsing the precision

            if (!parseLengthModifier(formatView, specifier))
                break; // Error parsing the length modifier

            if (!parseTypeSpecifier(formatView, specifier))
                break; // Error parsing the type specifier

            if (*specifier.m_typeSpecifier != '%')
            {
                specifier.m_begin = begin;
                specifier.m_end = formatView.m_begin;
                break; // OK
            }
        }
        return specifier;
    }

    inline bool isValid(const Specifier& specifier)
    {
        return specifier.m_begin && specifier.m_end;
    }

    inline size_t getStarsCount(const Specifier& specifier)
    {
        size_t starsCount = 0;
        if (isValid(specifier))
        {
            if (specifier.m_minWidth && *specifier.m_minWidth == '*')
                ++starsCount;
            if (specifier.m_precision && *specifier.m_precision == '*')
                ++starsCount;
        }
        return starsCount;
    }

    template< typename ArgT >
    inline bool isCompatible(const Specifier& specifier, const ArgT& )
    {
        switch (*specifier.m_typeSpecifier)
        {
        case 'c':
            return std::is_integral<ArgT>::value;
        case 's':
            return std::is_pointer<std::decay_t<ArgT>>::value && std::is_convertible<std::remove_pointer_t<std::decay_t<ArgT>>, char>::value;
        case 'd': case 'i':
            return std::is_integral<ArgT>::value;
        case 'o':
            return std::is_integral<ArgT>::value;
        case 'x': case 'X':
            return std::is_integral<ArgT>::value;
        case 'u':
            return std::is_integral<ArgT>::value;
        case 'f': case 'F':
            return std::is_floating_point<ArgT>::value;
        case 'e': case 'E':
            return std::is_floating_point<ArgT>::value;
        case 'a': case 'A':
            return std::is_floating_point<ArgT>::value;
        case 'g': case 'G':
            return std::is_floating_point<ArgT>::value;
        case 'n':
            return std::is_pointer<std::decay_t<ArgT>>::value && std::is_same<std::remove_pointer_t<std::decay_t<ArgT>>, int>::value;
        case 'p':
            return std::is_pointer<std::decay_t<ArgT>>::value;
        }
        return false;
    }

    template< typename ArgT >
    inline bool is_integer()
    {
        return std::is_integral<ArgT>::value;
    }

    inline bool check_printf_args(StringView& formatView)
    {
        return true;
    }

    template< typename ArgT0 >
    inline bool check_printf_args(StringView& formatView, const ArgT0& arg0)
    {
        const auto& specifier = parseSpecifier(formatView);
        if (!isValid(specifier))
            return false;
        if (getStarsCount(specifier) != 0)
            return false;
        if (!isCompatible(specifier, arg0))
            return false;
        return true;
    }

    template< typename ArgT0, typename ArgT1 >
    inline bool check_printf_args(StringView& formatView, const ArgT0& arg0, const ArgT1& arg1)
    {
        const auto& specifier = parseSpecifier(formatView);
        if (!isValid(specifier))
            return false;
        switch (getStarsCount(specifier))
        {
        case 0:
            if (!isCompatible(specifier, arg0))
                return false;
            return check_printf_args(formatView, arg1);
        case 1:
            if (!is_integer<ArgT0>())
                return false;
            if (!isCompatible(specifier, arg1))
                return false;
            return true;
        }
        return false;
    }

    template< typename ArgT0, typename ArgT1, typename ArgT2, typename... ArgsT >
    inline bool check_printf_args(StringView& formatView, const ArgT0& arg0, const ArgT1& arg1, const ArgT2& arg2, const ArgsT&... args)
    {
        const auto& specifier = parseSpecifier(formatView);
        if (!isValid(specifier))
            return false;
        switch (getStarsCount(specifier))
        {
        case 0:
            if (!isCompatible(specifier, arg0))
                return false;
            return check_printf_args(formatView, arg1, arg2, args...);
        case 1:
            if (!is_integer<ArgT0>())
                return false;
            if (!isCompatible(specifier, arg1))
                return false;
            return check_printf_args(formatView, arg2, args...);
        }
        if (!is_integer<ArgT0>())
            return false;
        if (!is_integer<ArgT1>())
            return false;
        if (!isCompatible(specifier, arg2))
            return false;
        return check_printf_args(formatView, args...);
    }

    template< typename... ArgsT >
    inline bool check_printf_args(const char* format, const ArgsT&... args)
    {
        StringView formatView{ format };
        if (!check_printf_args(formatView, args...))
            return false;
        return !isValid(parseSpecifier(formatView));
    }

    template <size_t BufferSizeV>
    inline bool buffer_check_adjust_size(char (&buffer)[BufferSizeV],
                                         size_t requiredBufferSize)
    {
        return requiredBufferSize <= BufferSizeV;
    }

    template <size_t BufferSizeV>
    inline char* buffer_get_data(char (&buffer)[BufferSizeV])
    {
        return buffer;
    }

    template< typename BufferT, typename... ArgsT >
    inline int sprintf_check_adjust_buffer_size(BufferT& buffer, const char* format, const ArgsT&... args)
    {
#pragma warning(push)
#pragma warning(disable : 4996) // warning C4996: 'sprintf': This function or variable may be unsafe. Consider using sprintf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
        const auto resulting_string_length = _snprintf(nullptr, 0, format, args...);
        if (resulting_string_length < 0)
            return -1;
        if (!buffer_check_adjust_size(buffer, static_cast<size_t>(resulting_string_length) + 1))
            return -1;
        return std::sprintf(buffer_get_data(buffer), format, args...);
#pragma warning(pop)
    }

} } // namespace pasc::detail


namespace pasc {

    template< size_t BufferSizeV, typename... ArgsT >
    inline int safe_sprintf(char (&buffer)[BufferSizeV], const char* format, const ArgsT&... args)
    {
        if (!detail::check_printf_args(format, args...))
            return -1;
        return pasc::detail::sprintf_check_adjust_buffer_size(buffer, format, args...);
    }

    template< typename... ArgsT >
    int safe_snprintf(char* buffer, int string_length, const char* format, const ArgsT&... args);

    template< typename... ArgsT >
    int safe_fprintf(FILE* file, const char* format, const ArgsT&... args)
    {
        if (!detail::check_printf_args(format, args...))
            return -1;
        return std::fprintf(file, format, args...);
    }

    template< typename... ArgsT >
    int safe_printf(const char* format, const ArgsT&... args)
    {
        if (!detail::check_printf_args(format, args...))
            return -1;
        return std::printf(format, args...);
    }

} // namespace pasc
