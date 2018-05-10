
#pragma once

#include <string>
#include <functional>
#include <iostream>

// Unit type == first-class void
struct U
{
};

// IO as Side Effect
// Simplest program:
// 1 print "What’s your name?"
// 2 name = getLine
// 3 print "Hi, " + name
// No dependency between 1 and 2, yet cannot rearrange
// Inconsistent with STM
// IO monad postpones the action, like a future (or a packaged_task)
template <class T>
class IO
{
  public:
    using Act = std::function<T()>;

  private:
    Act _act;

  public:
    IO(Act act)
        : _act(std::move(act))
    {
    }

    template <class F> // callable of signature IO<U>(T)
    auto bind(F f) -> decltype(f(_act()))
    {
        using ValueReturnedByFDotRun = decltype(f(_act()).run());
        return IO<ValueReturnedByFDotRun>([ act = _act, f ]() {
            T x = act();
            return f(x).run();
        });
    }

    template <class F> // callable of signature U(T)
    auto fmap(F f) -> IO<decltype(f(_act()))>
    {
        using ValueReturnedByF = decltype(f(_act()));
        return IO<ValueReturnedByF>([ act = _act, f ]() {
            T x = act();
            return f(x);
        });
    }

    T run()
    {
        return _act();
    }
};

template <class T>
inline IO<T> pure(T x)
{
    return IO<T>([x]() { return x; });
}

inline IO<U> putStr(std::string s)
{
    return IO<U>([s = std::move(s)]() {
        std::cout << s;
        return U{};
    });
}

inline IO<std::string> getLine(U /*u*/)
{
    return IO<std::string>([]() {
        std::string s;
        //        std::getline(std::cin, s);
        s = "blablah";
        return s;
    });
}

// inline IO<bool> ask(int i)
// {
//     return putStr("Is it less than ")
//         .fmap([i](U) { return /*putNumber(i)*/ true; })
//         .bind([](U) { return putStr(" (y/n)?\n"); })
//         .bind(getLine);
//         .bind([](std::string s) { return pure(s == "y"); });
// }

// inline IO<int> guess(int a, int b)
// {
//     if (a >= b)
//         return pure(a);
//     int m = (b + 1 + a) / 2;
//     return ask(m).bind([=](bool yes) { if (yes) return guess(a, m - 1); else  return guess(m, b); });
// }
