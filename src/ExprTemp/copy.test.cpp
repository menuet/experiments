
#include <catch2/catch.hpp>
#include <type_traits>
#include <array>
#include <memory>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <locale>


struct SmallTrivial
{
    SmallTrivial()
        : SmallTrivial('0')
    {
    }

    SmallTrivial(char c)
    {
        std::fill(std::begin(m_data), std::end(m_data), c);
    }

    std::array<char, sizeof(std::shared_ptr<void>)> m_data;
};
static_assert(std::is_trivially_copyable<SmallTrivial>::value, "Ensures SmallTrivial is trivially-copyable");

struct BigTrivial
{
    BigTrivial()
        : BigTrivial('0')
    {
    }

    BigTrivial(char c)
    {
        std::fill(std::begin(m_data), std::end(m_data), c);
    }

    std::array<char, sizeof(std::shared_ptr<void>) * 10> m_data;
};
static_assert(std::is_trivially_copyable<BigTrivial>::value, "Ensures BigTrivial is trivially-copyable");

struct NonTrivialWithHeapData
{
    NonTrivialWithHeapData()
        : NonTrivialWithHeapData('0')
    {
    }

    NonTrivialWithHeapData(char c)
        : m_data(sizeof(std::shared_ptr<void>) * 5)
    {
        std::fill(std::begin(m_data), std::end(m_data), c);
    }

    std::vector<char> m_data;
};
static_assert(!std::is_trivially_copyable<NonTrivialWithHeapData>::value, "Ensures BigTrivial is non-trivially-copyable");

template< typename T >
std::ostream& operator<<(std::ostream& os, const std::shared_ptr<T>& spT)
{
    os << *spT;
    return os;
}

template< size_t ArraySize >
std::ostream& operator<<(std::ostream& os, const std::array<char, ArraySize>& arr)
{
    for (const auto& x : arr)
    {
        os << x;
        os << ',';
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<char>& vec)
{
    for (const auto& x : vec)
    {
        os << x;
        os << ',';
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const SmallTrivial& t)
{
    os << t.m_data;
    return os;
}

std::ostream& operator<<(std::ostream& os, const BigTrivial& t)
{
    os << t.m_data;
    return os;
}

std::ostream& operator<<(std::ostream& os, const NonTrivialWithHeapData& t)
{
    os << t.m_data;
    return os;
}

struct ThousandsSeparator : std::numpunct<char> {
    char do_thousands_sep() const { return ' '; }
    std::string do_grouping() const { return "\3"; }
};

using ClockType = std::chrono::steady_clock;
using TimepointType = ClockType::time_point;
using DurationType = ClockType::duration;

template< typename Func >
DurationType measureDuration(Func func)
{
    const auto start = ClockType::now();
    func();
    const auto stop = ClockType::now();
    const auto duration = stop - start;
    return duration;
}

template< typename T, size_t ItemCount, typename Engine, typename Generator >
void loopAndMeasureDuration(Engine e, Generator generator)
{
    using std::begin; using std::end;

    const auto sourceArray = std::make_unique<std::array<T, ItemCount>>();
    std::generate(begin(*sourceArray), end(*sourceArray), generator);

    const auto targetArray = std::make_unique<std::array<T, ItemCount>>();

    std::cout << "Copying " << ItemCount << " items of type " << typeid(T).name() << " and of size " << sizeof(T) << " bytes...\n";

    const auto duration = measureDuration([&]()
    {
        std::copy(begin(*sourceArray), end(*sourceArray), begin(*targetArray));
    });
    std::cout << "Duration = " << std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() << " ns\n";

    auto d = std::uniform_int_distribution<size_t>(0, ItemCount - 1);
    const auto& item = (*targetArray)[d(e)];
    std::cout << "Random pick to avoid optimization: " << item << "\n\n";
}

void test()
{
    std::cout.imbue(std::locale(std::cout.getloc(), new ThousandsSeparator));

    constexpr size_t ITEM_COUNT = 10000;

    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    std::default_random_engine e(static_cast<std::default_random_engine::result_type>(now));
    std::uniform_int_distribution<> d('a', 'z');

    loopAndMeasureDuration<std::shared_ptr<SmallTrivial>, ITEM_COUNT>(e, [&]() { return std::make_shared<SmallTrivial>(d(e)); });

    loopAndMeasureDuration<SmallTrivial, ITEM_COUNT>(e, [&]() { return SmallTrivial(d(e)); });

    loopAndMeasureDuration<BigTrivial, ITEM_COUNT>(e, [&]() { return BigTrivial(d(e)); });

    loopAndMeasureDuration<NonTrivialWithHeapData, ITEM_COUNT>(e, [&]() { return NonTrivialWithHeapData(d(e)); });
}

SCENARIO("copy", "[copy]")
{
    test();
}
