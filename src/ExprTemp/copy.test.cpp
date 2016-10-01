
#include <catch/catch.hpp>
#include <type_traits>
#include <array>
#include <memory>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

namespace {

    struct DurationOutput
    {
        using ClockType = std::chrono::steady_clock;
        using TimepointType = ClockType::time_point;

        DurationOutput()
            : m_start(ClockType::now())
        {
        }

        ~DurationOutput()
        {
            const auto stop = ClockType::now();
            std::cout << "Duration = " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - m_start).count() << " ms\n";
        }

        TimepointType m_start;
    };

    struct Trivial
    {
        int m_i;
        double m_d;
        char m_c;
        short m_s;
        long long m_ll;
        char m_ac[100];
    };
    static_assert(std::is_trivial<Trivial>::value, "Ensures Trivial is trivial");
    static_assert(std::is_trivially_copyable<Trivial>::value, "Ensures Trivial is trivially-copyable");

    struct NonTrivial
    {
        NonTrivial() = default;

        NonTrivial(const NonTrivial& nonTrivial)
            : m_i(nonTrivial.m_i)
            , m_d(nonTrivial.m_d)
            , m_c(nonTrivial.m_c)
            , m_s(nonTrivial.m_s)
        {
        }

        int m_i;
        double m_d;
        char m_c;
        short m_s;
    };
    static_assert(!std::is_trivial<NonTrivial>::value, "Ensures NonTrivial is non-trivial");
    static_assert(!std::is_trivially_copyable<NonTrivial>::value, "Ensures NonTrivial is non-trivially-copyable");

    struct WithHeapData
    {
        WithHeapData()
            : m_v()
        {
        }

        std::string m_s{"some long enough string to avoid SBO"};
        std::vector<char> m_v;
    };
    static_assert(!std::is_trivial<WithHeapData>::value, "Ensures NonTrivial is non-trivial");
    static_assert(!std::is_trivially_copyable<WithHeapData>::value, "Ensures NonTrivial is non-trivially-copyable");

    template< typename T, size_t LoopCount, size_t ItemCount, typename Generator >
    void loopAndComputeDuration(Generator generator)
    {
        using std::begin; using std::end;

        const auto sourceArray = std::make_unique<std::array<T, ItemCount>>();
        std::generate(begin(*sourceArray), end(*sourceArray), generator);

        const auto targetArray = std::make_unique<std::array<T, ItemCount>>();

        std::cout << "Copying " << LoopCount << " times " << ItemCount << " items of type " << typeid(T).name() << " and of size " << sizeof(T) << " bytes...\n";

        {
            DurationOutput durationOuput;
            for (size_t index = 0; index < LoopCount; ++index)
            {
                std::copy(begin(*sourceArray), end(*sourceArray), begin(*targetArray));
            }
        }

        std::cout << "\n";
    }

}

SCENARIO("copy", "[copy]")
{
    constexpr size_t LOOP_COUNT = 100;
    constexpr size_t ITEM_COUNT = 100000;

    loopAndComputeDuration<std::shared_ptr<Trivial>, LOOP_COUNT, ITEM_COUNT>([]() { return std::make_shared<Trivial>(); });

    loopAndComputeDuration<Trivial, LOOP_COUNT, ITEM_COUNT>([]() { return Trivial{}; });

    loopAndComputeDuration<NonTrivial, LOOP_COUNT, ITEM_COUNT>([]() { return NonTrivial{}; });

    loopAndComputeDuration<WithHeapData, LOOP_COUNT, ITEM_COUNT>([]() { return WithHeapData{}; });
}
