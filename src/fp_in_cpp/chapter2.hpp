
#pragma once

#include <string>
#include <vector>
#include <numeric>

namespace fp_in_cpp
{

struct person_t
{
    bool is_female{};
    std::string name{};
};

inline bool is_female(const person_t &person)
{
    return person.is_female;
}

inline bool is_not_female(const person_t &person)
{
    return !person.is_female;
}

inline std::string name(const person_t &person)
{
    return person.name;
}

namespace rawloop
{
template <typename FilterFunction>
std::vector<std::string> names_for(
    const std::vector<person_t> &people,
    FilterFunction filter)
{
    std::vector<std::string> result;

    for (const person_t &person : people)
    {
        if (filter(person))
            result.push_back(name(person));
    }

    return result;
}
}

namespace recursive
{
template <typename T>
std::vector<T> tail(std::vector<T> v)
{
    if (!v.empty())
        v.erase(v.begin());
    return v;
}

namespace naive
{

template <typename T>
std::vector<T> prepend(const T &t, std::vector<T> v)
{
    v.insert(v.begin(), t);
    return v;
}

template <typename FilterFunction>
std::vector<std::string> names_for(
    const std::vector<person_t> &people,
    FilterFunction filter)
{
    if (people.empty())
        return {};

    const auto head = people.front();
    const auto processed_tail = names_for(tail(people), filter);
    if (filter(head))
        return prepend(name(head), processed_tail);

    return processed_tail;
}
}

namespace tailcall
{
template <typename FilterFunction>
std::vector<std::string> names_for_helper(
    const std::vector<person_t> &people,
    FilterFunction filter,
    std::vector<std::string> previously_collected)
{
    if (people.empty())
        return previously_collected;

    const auto head = people.front();
    if (filter(head))
        previously_collected.push_back(name(head));

    const auto processed_tail = names_for_helper(tail(people), filter, std::move(previously_collected));

    return processed_tail;
}

template <typename FilterFunction>
std::vector<std::string> names_for(
    const std::vector<person_t> &people,
    FilterFunction filter)
{
    return names_for_helper(people, filter, {});
}
}
}

namespace fold
{
template <typename FilterFunction>
auto append_name_if(
    FilterFunction filter)
{
    return [filter](
               std::vector<std::string> previously_collected,
               const person_t &person) {
        if (filter(person))
            previously_collected.push_back(name(person));
        return previously_collected;
    };
}

template <typename FilterFunction>
std::vector<std::string> names_for(
    const std::vector<person_t> &people,
    FilterFunction filter)
{
    return std::accumulate(
        people.cbegin(),
        people.cend(),
        std::vector<std::string>{},
        append_name_if(filter));
}

template< class InputIt, class UnaryPredicate >
bool any_of( InputIt first, InputIt last, UnaryPredicate p ) {
    return std::accumulate(first, last, false, [&](bool acc, auto item) { return acc || p(item); });
}

template< class InputIt, class UnaryPredicate >
bool all_of( InputIt first, InputIt last, UnaryPredicate p ) {
    return std::accumulate(first, last, true, [&](bool acc, auto item) { return acc && p(item); });
}
}
}
