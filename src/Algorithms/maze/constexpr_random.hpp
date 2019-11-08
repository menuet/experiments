
#include <cstddef>
#include <cstdint>
#include <limits>
#include <platform/platform.h>

namespace maze {

    constexpr auto build_time_seed()
    {
        std::uint64_t shifted = 0;

        for (const auto c : __TIME__)
        {
            shifted <<= 8;
            shifted |= c;
        }

        return shifted;
    }

    class PCG
    {
    public:
        using result_type = std::uint32_t;

        constexpr PCG(std::uint64_t seed) noexcept : inc{seed} {}

        constexpr result_type operator()() noexcept
        {
            std::uint64_t oldstate = state;
            // Advance internal state
            state = oldstate * 6364136223846793005ULL + (inc | 1);
            // Calculate output function (XSH RR), uses old state for max ILP
            const auto xorshifted = static_cast<std::uint32_t>(
                ((oldstate >> 18u) ^ oldstate) >> 27u);
            const auto rot = static_cast<std::uint32_t>(oldstate >> 59u);
#if EXP_PLATFORM_CPL_IS_MSVC
#pragma warning(push)
#pragma warning(                                                               \
    disable : 4146) // Disable warning C4146: unary minus operator applied to
                    // unsigned type, result still unsigned
#endif
            return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
#if EXP_PLATFORM_CPL_IS_MSVC
#pragma warning(pop)
#endif
        }

        static constexpr result_type min() noexcept
        {
            return std::numeric_limits<result_type>::min();
        }

        static constexpr result_type max() noexcept
        {
            return std::numeric_limits<result_type>::min();
        }

    private:
        std::uint64_t state = 0;
        std::uint64_t inc = 0;
    };

    template <typename T, typename Gen>
    constexpr auto distribution(Gen& g, T min, T max) noexcept
    {
        const auto range = max - min + 1;
        const auto bias_remainder = std::numeric_limits<T>::max() % range;
        const auto unbiased_max =
            std::numeric_limits<T>::max() - bias_remainder - 1;

        auto r = g();
        for (; r > unbiased_max; r = g())
            ;

        return (r % range) + min;
    }

} // namespace maze
