
#pragma once


#include <fmt/format.h>


template< typename... Args >
inline void println(const char* format, const Args&... args) {
    fmt::print(format, args...);
    fmt::print("\n");
}

template< typename T >
struct Size {
    T w{};
    T h{};
};

template< typename T >
inline constexpr bool operator==(Size<T> s1, Size<T> s2) {
    return s1.w == s2.w && s1.h == s2.h;
}

template< typename T >
inline constexpr bool operator!=(Size<T> s1, Size<T> s2) {
    return !(s1 == s2);
}

template< typename T >
inline void format(fmt::BasicFormatter<char>& f, const char*& format_str, const Size<T>& s) {
    f.writer().write("[w={}, h={}]", s.w, s.h);
}

template< typename T >
struct Point {
    int x{};
    int y{};
    Point& operator+=(Size<T> s) {
        x += s.w;
        y += s.h;
        return *this;
    }
};

template< typename T >
inline constexpr bool operator==(Point<T> p1, Point<T> p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

template< typename T >
inline constexpr bool operator!=(Point<T> p1, Point<T> p2) {
    return !(p1 == p2);
}

template< typename T >
inline void format(fmt::BasicFormatter<char>& f, const char*& format_str, const Point<T>& p) {
    f.writer().write("[x={}, y={}]", p.x, p.y);
}

using SizeI = Size<int>;
using SizeU = Size<unsigned int>;
using PointI = Point<int>;
