
#pragma once


#include <type_traits>


#ifndef _MSC_VER

namespace std {

    template< class From, class To >
    constexpr bool is_convertible_v = is_convertible<From, To>::value;

    template< class T, class U >
    constexpr bool is_same_v = is_same<T, U>::value;

    template< class T, class... Args >
    constexpr bool is_constructible_v = is_constructible<T, Args...>::value;

    template< class T >
    constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

    template< class Base, class Derived >
    constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

}

#endif
