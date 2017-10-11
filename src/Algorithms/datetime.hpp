
#pragma once


#include <chrono>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
#include <platform/platform.h>

#if EXP_PLATFORM_OS_IS_WINDOWS
#include <Windows.h>

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
        union CalendarDatetime
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

    namespace stdc {
        using CalendarDatetime = std::tm;
    }

    namespace win {
        using CalendarDatetime = SYSTEMTIME;
    }

    using Clock = std::chrono::system_clock;
    using Nanoseconds = std::chrono::duration<std::int64_t, std::nano>;
    using PointDatetime = std::chrono::time_point<Clock, Nanoseconds>;

    struct CalendarDatetime
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

    struct DefaultConversionMethod {};

    template< typename ToT, typename FromT, typename ConversionMethodT >
    inline ToT datetime_cast(const FromT&, const ConversionMethodT&)
    {
        static_assert(false, "This datetime cast is not supported");
    }

    template< typename ToT, typename FromT >
    inline ToT datetime_cast(const FromT& from)
    {
        return datetime_cast<ToT>(from, DefaultConversionMethod{});
    }

    template<>
    inline old::CalendarDatetime datetime_cast<old::CalendarDatetime, CalendarDatetime>(const CalendarDatetime& dt)
    {
        old::CalendarDatetime odt{};
        odt.fields.year = dt.year;
        odt.fields.month = dt.month;
        odt.fields.day = dt.day;
        odt.fields.hour = dt.hour;
        odt.fields.minute = dt.minute;
        odt.fields.second = dt.second;
        odt.fields.millisecond = dt.millisecond;
        return odt;
    }

    template<>
    inline CalendarDatetime datetime_cast<CalendarDatetime, old::CalendarDatetime>(const old::CalendarDatetime& dt)
    {
        CalendarDatetime ndt{};
        ndt.year = dt.fields.year;
        ndt.month = dt.fields.month;
        ndt.day = dt.fields.day;
        ndt.hour = dt.fields.hour;
        ndt.minute = dt.fields.minute;
        ndt.second = dt.fields.second;
        ndt.millisecond = dt.fields.millisecond;
        return ndt;
    }

    template<>
    inline stdc::CalendarDatetime datetime_cast<stdc::CalendarDatetime, CalendarDatetime>(const CalendarDatetime& dt)
    {
        stdc::CalendarDatetime sdt{};
        sdt.tm_year = dt.year - 1900;
        sdt.tm_mon = dt.month - 1;
        sdt.tm_mday = dt.day;
        sdt.tm_hour = dt.hour;
        sdt.tm_min = dt.minute;
        sdt.tm_sec = dt.second;
        return sdt;
    }

    template<>
    inline CalendarDatetime datetime_cast<CalendarDatetime, stdc::CalendarDatetime>(const stdc::CalendarDatetime& dt)
    {
        CalendarDatetime ndt{};
        ndt.year = dt.tm_year + 1900;
        ndt.month = dt.tm_mon + 1;
        ndt.day = dt.tm_mday;
        ndt.hour = dt.tm_hour;
        ndt.minute = dt.tm_min;
        ndt.second = dt.tm_sec;
        return ndt;
    }

    template<>
    inline win::CalendarDatetime datetime_cast<win::CalendarDatetime, CalendarDatetime>(const CalendarDatetime& dt)
    {
        win::CalendarDatetime wdt{};
        wdt.wYear = dt.year;
        wdt.wMonth = dt.month;
        wdt.wDay = dt.day;
        wdt.wHour = dt.hour;
        wdt.wMinute = dt.minute;
        wdt.wSecond = dt.second;
        wdt.wMilliseconds = dt.millisecond;
        return wdt;
    }

    template<>
    inline CalendarDatetime datetime_cast<CalendarDatetime, win::CalendarDatetime>(const win::CalendarDatetime& dt)
    {
        CalendarDatetime ndt{};
        ndt.year = dt.wYear;
        ndt.month = static_cast<decltype(ndt.month)>(dt.wMonth);
        ndt.day = static_cast<decltype(ndt.day)>(dt.wDay);
        ndt.hour = static_cast<decltype(ndt.hour)>(dt.wHour);
        ndt.minute = static_cast<decltype(ndt.minute)>(dt.wMinute);
        ndt.second = static_cast<decltype(ndt.second)>(dt.wSecond);
        ndt.millisecond = dt.wMilliseconds;
        return ndt;
    }

    template<>
    inline CalendarDatetime datetime_cast<CalendarDatetime, PointDatetime>(const PointDatetime& dt)
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

        auto dtc = datetime_cast<CalendarDatetime>(clockTm);
        dtc.millisecond = static_cast<std::uint16_t>(remainingMilliseconds.count());
        dtc.microsecond = static_cast<std::uint16_t>(remainingMicroseconds.count());
        dtc.nanosecond = static_cast<std::uint16_t>(remainingNanoseconds.count());
        return dtc;
    }

    template<>
    inline PointDatetime datetime_cast<PointDatetime, CalendarDatetime>(const CalendarDatetime& dt)
    {
        auto clockTm = datetime_cast<stdc::CalendarDatetime>(dt);
        clockTm.tm_isdst = 1;
        const auto clockTimeT = std::mktime(&clockTm);
        const auto clockTimepoint = Clock::from_time_t(clockTimeT);
        const auto clockDurationSinceEpoch = clockTimepoint.time_since_epoch();
        const auto durationSinceEpoch = std::chrono::duration_cast<Nanoseconds>(clockDurationSinceEpoch);

        PointDatetime dtp{ durationSinceEpoch };
        dtp += std::chrono::duration<std::int64_t, std::milli>{dt.millisecond};
        dtp += std::chrono::duration<std::int64_t, std::micro>{dt.microsecond};
        dtp += std::chrono::duration<std::int64_t, std::nano>{dt.nanosecond};
        return dtp;
    }

    template<>
    inline CalendarDatetime datetime_cast<CalendarDatetime, std::string, const char*>(const std::string& dt, const char* const& format)
    {
#if 0 // BUGGY
        std::stringstream ss;
        ss.str(dt);
        std::tm tmt{};
        ss >> std::get_time(&tmt, format);
        auto ndt = datetime_cast<CalendarDatetime>(tmt);
        return ndt;
#endif
        // NOT IMPLEMENTED
        return CalendarDatetime{};
    }

    template<>
    inline std::string datetime_cast<std::string, CalendarDatetime, const char*>(const CalendarDatetime& dt, const char* const& format)
    {
        const auto tmt = datetime_cast<stdc::CalendarDatetime>(dt);
        std::stringstream ss;
        ss << std::put_time(&tmt, format);
        auto sdt = ss.str();
        return sdt;
    }

    template<>
    inline CalendarDatetime datetime_cast<CalendarDatetime, std::wstring, const wchar_t*>(const std::wstring& dt, const wchar_t* const& format)
    {
        // NOT IMPLEMENTED
        return CalendarDatetime{};
    }

    template<>
    inline std::wstring datetime_cast<std::wstring, CalendarDatetime, const wchar_t*>(const CalendarDatetime& dt, const wchar_t* const& format)
    {
        const auto wdt = datetime_cast<win::CalendarDatetime>(dt);
        std::wstring sdt{ format };
        wchar_t datetimeBuffer[128] = {};
        if (GetDateFormatEx(LOCALE_NAME_INVARIANT, 0, &wdt, sdt.c_str(), datetimeBuffer, _countof(datetimeBuffer), nullptr) > 0)
            sdt = datetimeBuffer;
        if (GetTimeFormatEx(LOCALE_NAME_INVARIANT, 0, &wdt, sdt.c_str(), datetimeBuffer, _countof(datetimeBuffer)) > 0)
            sdt = datetimeBuffer;
        return sdt;
    }

    inline PointDatetime getCurrentDatetime()
    {
        return Clock::now();
    }

} // namespace datetime

#endif
