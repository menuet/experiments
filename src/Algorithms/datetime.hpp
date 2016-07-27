
#pragma once


#include <chrono>
#include <ctime>
#include <string>
#include <sstream>


namespace datetime {

    namespace old {
        // the time and date (tad) is based on the following 64 bits format:
        // uuyyyyyy yyyyyyyy yymmmmdd dddhhhhh mmmmmmss ssssmmmm mmmmmmuu uuuuuuuu
        //                                     nnnnnn       ssss ssssssss ssssssss
        //       name  range  mask
        //    ---------------------------------------
        // u [unused] [63:62] C0 00 00 00 00 00 00 00
        // y [  year] [61:46] 3F FF C0 00 00 00 00 00
        // m [ month] [45:42] 00 00 3C 00 00 00 00 00
        // d [   day] [41:37] 00 00 03 E0 00 00 00 00
        // d [  hour] [36:32] 00 00 00 1F 00 00 00 00
        // mn[minute] [31:26] 00 00 00 00 FC 00 00 00
        // s [second] [25:20] 00 00 00 00 03 F0 00 00
        // ms[millis] [19:10] 00 00 00 00 00 0F FC 00
        union DatetimeCalendar
        {
            std::uint64_t raw;
            std::uint64_t
                unused : 10,
                millis : 10,
                second : 6,
                minute : 6,
                hour : 5,
                day : 5,
                month : 4,
                year : 16;
        };
    }

    using Clock = std::chrono::system_clock;
    using Nanoseconds = std::chrono::duration<std::int64_t, std::nano>;
    using DatetimePoint = std::chrono::time_point<Clock, Nanoseconds>;

    struct DatetimeCalendar
    {
        std::uint16_t year;
        std::uint8_t month;
        std::uint8_t day;
        std::uint8_t hour;
        std::uint8_t minute;
        std::uint8_t second;
        std::uint16_t millisecond;
        std::uint16_t microsecond;
        std::uint16_t nanosecond;
    };

    inline DatetimePoint getCurrentDatetime()
    {
        return Clock::now();
    }

    inline DatetimeCalendar toCalendar(DatetimePoint dt)
    {
        const auto durationSinceEpoch = dt.time_since_epoch();

        const auto clockDurationSinceEpoch = std::chrono::duration_cast<Clock::duration>(durationSinceEpoch);
        const auto clockTimepoint = Clock::time_point{ clockDurationSinceEpoch };
        const auto clockTimeT = Clock::to_time_t(clockTimepoint);
        const auto clockTm = *std::localtime(&clockTimeT);

        const auto secondsSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(durationSinceEpoch);
        auto remainingNanoseconds = durationSinceEpoch - secondsSinceEpoch;
        const auto remainingMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(remainingNanoseconds);
        remainingNanoseconds = remainingNanoseconds - remainingMilliseconds;
        const auto remainingMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(remainingNanoseconds);
        remainingNanoseconds = remainingNanoseconds - remainingMicroseconds;

        DatetimeCalendar dtc{};
        dtc.year = clockTm.tm_year + 1900;
        dtc.month = clockTm.tm_mon + 1;
        dtc.day = clockTm.tm_mday;
        dtc.hour = clockTm.tm_hour;
        dtc.minute = clockTm.tm_min;
        dtc.second = clockTm.tm_sec;
        dtc.millisecond = static_cast<std::uint16_t>(remainingMilliseconds.count());
        dtc.microsecond = static_cast<std::uint16_t>(remainingMicroseconds.count());
        dtc.nanosecond = static_cast<std::uint16_t>(remainingNanoseconds.count());
        return dtc;
    }

    inline DatetimePoint toPoint(const DatetimeCalendar& dt)
    {
        std::tm clockTm{};
        clockTm.tm_year = dt.year - 1900;
        clockTm.tm_mon = dt.month - 1;
        clockTm.tm_mday = dt.day;
        clockTm.tm_hour = dt.hour;
        clockTm.tm_min = dt.minute;
        clockTm.tm_sec = dt.second;
        clockTm.tm_isdst = 1;
        const auto clockTimeT = std::mktime(&clockTm);
        const auto clockTimepoint = Clock::from_time_t(clockTimeT);
        const auto clockDurationSinceEpoch = clockTimepoint.time_since_epoch();
        const auto durationSinceEpoch = std::chrono::duration_cast<Nanoseconds>(clockDurationSinceEpoch);

        DatetimePoint dtp{ durationSinceEpoch };
        dtp += std::chrono::duration<std::int64_t, std::milli>{dt.millisecond};
        dtp += std::chrono::duration<std::int64_t, std::micro>{dt.microsecond};
        dtp += std::chrono::duration<std::int64_t, std::nano>{dt.nanosecond};
        return dtp;
    }

    inline old::DatetimeCalendar toOld(const DatetimeCalendar& dt)
    {
        old::DatetimeCalendar odtc{};
        return odtc;
    }

    inline DatetimeCalendar toNew(const old::DatetimeCalendar& dt)
    {
        DatetimeCalendar dtc{};
        return dtc;
    }

    inline DatetimeCalendar toCalendar(const std::string& dt)
    {
        return DatetimeCalendar{};
    }

    inline std::string toString(const DatetimeCalendar& dt)
    {
        return std::string{};
    }

} // namespace datetime
