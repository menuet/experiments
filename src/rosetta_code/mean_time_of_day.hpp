
#pragma once

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <numeric>
#include <optional>
#include <tuple>

namespace sc = std::chrono;

constexpr double Pi = 3.14159265358979323846;
constexpr double DegreesInHalfCircle = 180.;
constexpr double DegreesInOneCircle = 360.;
constexpr double SecondsInOneDay = 24. * 60. * 60.;

struct TimeOfDay
{
    sc::hours h{};
    sc::minutes m{};
    sc::seconds s{};
};

static constexpr double time_to_degrees(const TimeOfDay& time_of_day) noexcept
{
    const auto time_in_seconds = time_of_day.h + time_of_day.m + time_of_day.s;
    return time_in_seconds.count() * DegreesInOneCircle / SecondsInOneDay;
}

static TimeOfDay degrees_to_time(double time_in_degrees) noexcept
{
    time_in_degrees = std::fmod(time_in_degrees, DegreesInOneCircle);
    if (time_in_degrees < 0.)
        time_in_degrees += DegreesInOneCircle;
    const auto time_in_seconds = sc::seconds{static_cast<sc::seconds::rep>(
        time_in_degrees * SecondsInOneDay / DegreesInOneCircle)};
    const auto h = sc::duration_cast<sc::hours>(time_in_seconds);
    const auto m = sc::duration_cast<sc::minutes>(time_in_seconds - h);
    const auto s = time_in_seconds - h - m;
    return {h, m, s};
}

static std::optional<TimeOfDay> parse_time_of_day(std::string time_of_day)
{
    try
    {
        std::size_t pos{};

        const auto h = sc::hours{std::stoi(time_of_day, &pos)};
        if (time_of_day[pos] != ':')
            return std::nullopt;

        time_of_day = time_of_day.substr(pos + 1);

        const auto m = sc::minutes{std::stoi(time_of_day, &pos)};
        if (time_of_day[pos] != ':')
            return std::nullopt;

        time_of_day = time_of_day.substr(pos + 1);

        const auto s = sc::seconds{std::stoi(time_of_day, &pos)};
        if (time_of_day[pos] != 0)
            return std::nullopt;

        return TimeOfDay{h, m, s};
    }
    catch (std::exception&)
    {
        return std::nullopt;
    }
}

static std::string to_string(const TimeOfDay& time_of_day)
{
    char time_as_string[9] = {};
    std::sprintf(time_as_string, "%02u:%02u:%02u", (unsigned int)time_of_day.h.count(),
                 (unsigned int)time_of_day.m.count(), (unsigned int)time_of_day.s.count());
    return time_as_string;
}

template <typename InputIter>
static auto mean_angle(InputIter angles_begin, InputIter angles_end)
{
    const auto [x, y, length] = std::accumulate(
        angles_begin, angles_end, std::tuple{0., 0., 0},
        [](const auto& accu, auto elem) {
            const auto angle_in_radians = elem * Pi / DegreesInHalfCircle;
            const auto angle_cos = std::cos(angle_in_radians);
            const auto angle_sin = std::sin(angle_in_radians);
            return std::tuple{std::get<0>(accu) + angle_cos,
                              std::get<1>(accu) + angle_sin,
                              std::get<2>(accu) + 1};
        });

    assert(length > 0);

    const auto mean_angle_in_radians = std::atan2(y / length, x / length);
    const auto mean_angle_in_degrees = mean_angle_in_radians * DegreesInHalfCircle / Pi;
    if (std::fabs(mean_angle_in_degrees) < 0.000001)
        return 0.;
    return mean_angle_in_degrees;
}

static auto mean_angle(std::initializer_list<double> angles_in_degrees)
{
    return mean_angle(begin(angles_in_degrees), end(angles_in_degrees));
}

template <typename InputIter>
static auto mean_time_of_day(InputIter strings_begin, InputIter strings_end)
{
    std::vector<double> times_in_degrees;
    std::transform(
        strings_begin, strings_end, std::back_inserter(times_in_degrees),
        [](const auto& time_as_string) {
            return time_to_degrees(
                parse_time_of_day(time_as_string).value_or(TimeOfDay{}));
        });

    const auto mean =
        mean_angle(begin(times_in_degrees), end(times_in_degrees));
    return to_string(degrees_to_time(mean));
}

static auto
mean_time_of_day(std::initializer_list<std::string> times_as_strings)
{
    return mean_time_of_day(begin(times_as_strings), end(times_as_strings));
}
