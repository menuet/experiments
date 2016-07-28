
#pragma once


#include <chrono>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>


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
            struct {
                std::uint64_t
                    unused : 10,
                    millisecond : 10,
                    second : 6,
                    minute : 6,
                    hour : 5,
                    day : 5,
                    month : 4,
                    year : 16;
            } fields;
        };
    }

    namespace system {
        using DatetimeCalendar = std::tm;
    }

    using Clock = std::chrono::system_clock;
    using Nanoseconds = std::chrono::duration<std::int64_t, std::nano>;
    using DatetimePoint = std::chrono::time_point<Clock, Nanoseconds>;

    struct DatetimeCalendar
    {
        std::uint16_t year{};
        std::uint8_t month{};
        std::uint8_t day{};
        std::uint8_t hour{};
        std::uint8_t minute{};
        std::uint8_t second{};
        std::uint16_t millisecond{};
        std::uint16_t microsecond{};
        std::uint16_t nanosecond{};
    };

    template< typename ToT, typename FromT >
    inline ToT datetime_cast(const FromT&)
    {
        static_assert(false, "This datetime cast is not supported");
    }

    template<>
    inline old::DatetimeCalendar datetime_cast<old::DatetimeCalendar, DatetimeCalendar>(const DatetimeCalendar& dt)
    {
        old::DatetimeCalendar odtc{};
        odtc.fields.year = dt.year;
        odtc.fields.month = dt.month;
        odtc.fields.day = dt.day;
        odtc.fields.hour = dt.hour;
        odtc.fields.minute = dt.minute;
        odtc.fields.second = dt.second;
        odtc.fields.millisecond = dt.millisecond;
        return odtc;
    }

    template<>
    inline DatetimeCalendar datetime_cast<DatetimeCalendar, old::DatetimeCalendar>(const old::DatetimeCalendar& dt)
    {
        DatetimeCalendar ndtc{};
        ndtc.year = dt.fields.year;
        ndtc.month = dt.fields.month;
        ndtc.day = dt.fields.day;
        ndtc.hour = dt.fields.hour;
        ndtc.minute = dt.fields.minute;
        ndtc.second = dt.fields.second;
        ndtc.millisecond = dt.fields.millisecond;
        return ndtc;
    }

    template<>
    inline system::DatetimeCalendar datetime_cast<system::DatetimeCalendar, DatetimeCalendar>(const DatetimeCalendar& dtc)
    {
        system::DatetimeCalendar sdtc{};
        sdtc.tm_year = dtc.year - 1900;
        sdtc.tm_mon = dtc.month - 1;
        sdtc.tm_mday = dtc.day;
        sdtc.tm_hour = dtc.hour;
        sdtc.tm_min = dtc.minute;
        sdtc.tm_sec = dtc.second;
        return sdtc;
    }

    template<>
    inline DatetimeCalendar datetime_cast<DatetimeCalendar, system::DatetimeCalendar>(const system::DatetimeCalendar& sdtc)
    {
        DatetimeCalendar dtc{};
        dtc.year = sdtc.tm_year + 1900;
        dtc.month = sdtc.tm_mon + 1;
        dtc.day = sdtc.tm_mday;
        dtc.hour = sdtc.tm_hour;
        dtc.minute = sdtc.tm_min;
        dtc.second = sdtc.tm_sec;
        return dtc;
    }

    template<>
    inline DatetimeCalendar datetime_cast<DatetimeCalendar, DatetimePoint>(const DatetimePoint& dt)
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

        auto dtc = datetime_cast<DatetimeCalendar>(clockTm);
        dtc.millisecond = static_cast<std::uint16_t>(remainingMilliseconds.count());
        dtc.microsecond = static_cast<std::uint16_t>(remainingMicroseconds.count());
        dtc.nanosecond = static_cast<std::uint16_t>(remainingNanoseconds.count());
        return dtc;
    }

    template<>
    inline DatetimePoint datetime_cast<DatetimePoint, DatetimeCalendar>(const DatetimeCalendar& dt)
    {
        auto clockTm = datetime_cast<system::DatetimeCalendar>(dt);
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

    template< typename ToT, typename FromT >
    inline ToT datetime_cast(const FromT&, const char* format)
    {
        static_assert(false, "This datetime cast is not supported");
    }

    template<>
    inline DatetimeCalendar datetime_cast<DatetimeCalendar, std::string>(const std::string& dts, const char* format)
    {
        std::stringstream ss;
        ss.str(dts);
        std::tm tmt{};
        ss >> std::get_time(&tmt, format);
        auto dtc = datetime_cast<DatetimeCalendar>(tmt);
        return dtc;
    }

    template<>
    inline std::string datetime_cast<std::string, DatetimeCalendar>(const DatetimeCalendar& dtc, const char* format)
    {
        const auto tmt = datetime_cast<system::DatetimeCalendar>(dtc);
        std::stringstream ss;
        ss << std::put_time(&tmt, format);
        auto dts = ss.str();
        return dts;
    }

    inline DatetimePoint getCurrentDatetime()
    {
        return Clock::now();
    }

} // namespace datetime
