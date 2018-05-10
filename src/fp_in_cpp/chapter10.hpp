
#pragma once

#include <boost/optional>
#include <vector>
#include <algorithm>
#include <iterator>

namespace fp_in_cpp
{
    // map (transform) : (F<T1>, T1 → T2) → F<T2>
    // join            : M<M<T>> → M<T>
    // construct       : T → M<T>
    // mbind           : (M<T1>, T1 → M<T2>) → M<T2>

    template <typename T, typename F> // F: T -> T2
    auto transform(const std::optional<T>& opt, F f)
        -> decltype(std::make_optional(f(*opt)))
    {
        if (!opt) {
            return {};
        }
        return std::make_optional(f(*opt));
    }

    template <typename T, typename F> // F: T -> optional<T2>
    auto mbind(const std::optional<T>& opt, F f)
        -> decltype(f(*opt))
    {
        if (!opt) {
            return {};
        }
        return f(*opt);
    }

    template <typename T, typename F>
    auto transform(const std::vector<T>& in, F f)
    {
        std::vector<decltype(f(in.back()))> out(in.size());
        std::transform(in.begin(), in.end(), out.begin(), f);
        return out;
    }

    template <typename T>
    auto join(const std::vector<std::vector<T>>& in)
    {
        std::vector<T> out;
        for (const auto& v : in)
        {
            std::copy(v.begin(), v.end(), std::back_inserter(out));
        }
        return out;
    }

    template <typename T>
    std::vector<T> make_vector(T&& value) // construct
    {
        std::vector<T> out;
        out.push_back(std::forward<T>(value));
        return out;
    }

}
