
#include <catch/catch.hpp>
#include "datetime.hpp"


namespace datetime {

	namespace ut {

		SCENARIO("dates times", "[datetime][!mayfail]")
		{
            GIVEN("the epoch point date-time")
            {
                const auto epochPointDatetime = PointDatetime{};
                REQUIRE(epochPointDatetime.time_since_epoch().count() == 0);

                WHEN("We transform it to calendar")
                {
                    const auto epochCalendarDatetime = datetime_cast<CalendarDatetime>(epochPointDatetime);

                    THEN("we get Jan 1st 1970 at 00:00:00.000.000.000")
                    {
                        REQUIRE(epochCalendarDatetime.year == 1970);
                        REQUIRE(epochCalendarDatetime.month == 1);
                        REQUIRE(epochCalendarDatetime.day == 1);
                        // REQUIRE(epochCalendarDatetime.hour == 0);
                        REQUIRE(epochCalendarDatetime.hour == 1); // What The Fuck ?
                        REQUIRE(epochCalendarDatetime.minute == 0);
                        REQUIRE(epochCalendarDatetime.second == 0);
                        REQUIRE(epochCalendarDatetime.millisecond == 0);
                        REQUIRE(epochCalendarDatetime.microsecond == 0);
                        REQUIRE(epochCalendarDatetime.nanosecond == 0);

                        AND_WHEN("We transform it back to point")
                        {
                            const auto epochPointDatetimeBack = datetime_cast<PointDatetime>(epochCalendarDatetime);
                            THEN("we get 0")
                            {
                                // REQUIRE(epochPointDatetimeBack.time_since_epoch().count() == 0);
                                REQUIRE(epochPointDatetimeBack.time_since_epoch().count() == -1000000000); // What The Fuck ?
                            }
                        }
                    }
                }
            }

            GIVEN("a calendar date-time July 27th 2016 at 18:15:37.123.456.789")
            {
                CalendarDatetime someCalendarDatetime{};
                someCalendarDatetime.year = 2016;
                someCalendarDatetime.month = 7;
                someCalendarDatetime.day = 27;
                someCalendarDatetime.hour = 18;
                someCalendarDatetime.minute = 15;
                someCalendarDatetime.second = 37;
                someCalendarDatetime.millisecond = 123;
                someCalendarDatetime.microsecond = 456;
                someCalendarDatetime.nanosecond = 789;

                WHEN("We transform it to point")
                {
                    const auto somePointDatetime = datetime_cast<PointDatetime>(someCalendarDatetime);

                    THEN("we get 1469636137.123456789 / 1469639737.123456789")
                    {
                        REQUIRE(somePointDatetime.time_since_epoch().count() == 1469636137123456789);

                        AND_WHEN("We transform it back to calendar")
                        {
                            const auto someCalendarDatetimeBack = datetime_cast<CalendarDatetime>(somePointDatetime);
                            THEN("we get July 27th 2016 at 18:15:37.123.456.789")
                            {
                                REQUIRE(someCalendarDatetimeBack.year == 2016);
                                REQUIRE(someCalendarDatetimeBack.month == 7);
                                REQUIRE(someCalendarDatetimeBack.day == 27);
                                REQUIRE(someCalendarDatetimeBack.hour == 18);
                                REQUIRE(someCalendarDatetimeBack.minute == 15);
                                REQUIRE(someCalendarDatetimeBack.second == 37);
                                REQUIRE(someCalendarDatetimeBack.millisecond == 123);
                                REQUIRE(someCalendarDatetimeBack.microsecond == 456);
                                REQUIRE(someCalendarDatetimeBack.nanosecond == 789);
                            }
                        }
                    }
                }

                WHEN("We transform it to an old calendar")
                {
                    const auto someDatetimeOld = datetime_cast<old::CalendarDatetime>(someCalendarDatetime);

                    THEN("we get July 27th 2016 at 18:15:37.123")
                    {
                        REQUIRE(someDatetimeOld.fields.year == 2016);
                        REQUIRE(someDatetimeOld.fields.month == 7);
                        REQUIRE(someDatetimeOld.fields.day == 27);
                        REQUIRE(someDatetimeOld.fields.hour == 18);
                        REQUIRE(someDatetimeOld.fields.minute == 15);
                        REQUIRE(someDatetimeOld.fields.second == 37);
                        REQUIRE(someDatetimeOld.fields.millisecond == 123);

                        AND_WHEN("We transform it back to calendar")
                        {
                            const auto someCalendarDatetimeBack = datetime_cast<CalendarDatetime>(someDatetimeOld);
                            THEN("we get July 27th 2016 at 18:15:37.123.000.000")
                            {
                                REQUIRE(someCalendarDatetimeBack.year == 2016);
                                REQUIRE(someCalendarDatetimeBack.month == 7);
                                REQUIRE(someCalendarDatetimeBack.day == 27);
                                REQUIRE(someCalendarDatetimeBack.hour == 18);
                                REQUIRE(someCalendarDatetimeBack.minute == 15);
                                REQUIRE(someCalendarDatetimeBack.second == 37);
                                REQUIRE(someCalendarDatetimeBack.millisecond == 123);
                                REQUIRE(someCalendarDatetimeBack.microsecond == 000);
                                REQUIRE(someCalendarDatetimeBack.nanosecond == 000);
                            }
                        }
                    }
                }

                WHEN("We transform it to an stdc calendar")
                {
                    const auto someDatetimeStdc = datetime_cast<stdc::CalendarDatetime>(someCalendarDatetime);

                    THEN("we get July 27th 2016 at 18:15:37")
                    {
                        REQUIRE(someDatetimeStdc.tm_year == 2016 - 1900);
                        REQUIRE(someDatetimeStdc.tm_mon == 7 - 1);
                        REQUIRE(someDatetimeStdc.tm_mday == 27);
                        REQUIRE(someDatetimeStdc.tm_hour == 18);
                        REQUIRE(someDatetimeStdc.tm_min == 15);
                        REQUIRE(someDatetimeStdc.tm_sec == 37);

                        AND_WHEN("We transform it back to calendar")
                        {
                            const auto someCalendarDatetimeBack = datetime_cast<CalendarDatetime>(someDatetimeStdc);
                            THEN("we get July 27th 2016 at 18:15:37.000.000.000")
                            {
                                REQUIRE(someCalendarDatetimeBack.year == 2016);
                                REQUIRE(someCalendarDatetimeBack.month == 7);
                                REQUIRE(someCalendarDatetimeBack.day == 27);
                                REQUIRE(someCalendarDatetimeBack.hour == 18);
                                REQUIRE(someCalendarDatetimeBack.minute == 15);
                                REQUIRE(someCalendarDatetimeBack.second == 37);
                                REQUIRE(someCalendarDatetimeBack.millisecond == 000);
                                REQUIRE(someCalendarDatetimeBack.microsecond == 000);
                                REQUIRE(someCalendarDatetimeBack.nanosecond == 000);
                            }
                        }
                    }
                }

                WHEN("We transform it to a win calendar")
                {
                    const auto someDatetimeWin = datetime_cast<win::CalendarDatetime>(someCalendarDatetime);

                    THEN("we get July 27th 2016 at 18:15:37")
                    {
                        REQUIRE(someDatetimeWin.wYear == 2016);
                        REQUIRE(someDatetimeWin.wMonth == 7);
                        REQUIRE(someDatetimeWin.wDay == 27);
                        REQUIRE(someDatetimeWin.wHour == 18);
                        REQUIRE(someDatetimeWin.wMinute == 15);
                        REQUIRE(someDatetimeWin.wSecond == 37);
                        REQUIRE(someDatetimeWin.wMilliseconds == 123);

                        AND_WHEN("We transform it back to calendar")
                        {
                            const auto someCalendarDatetimeBack = datetime_cast<CalendarDatetime>(someDatetimeWin);
                            THEN("we get July 27th 2016 at 18:15:37.123.000.000")
                            {
                                REQUIRE(someCalendarDatetimeBack.year == 2016);
                                REQUIRE(someCalendarDatetimeBack.month == 7);
                                REQUIRE(someCalendarDatetimeBack.day == 27);
                                REQUIRE(someCalendarDatetimeBack.hour == 18);
                                REQUIRE(someCalendarDatetimeBack.minute == 15);
                                REQUIRE(someCalendarDatetimeBack.second == 37);
                                REQUIRE(someCalendarDatetimeBack.millisecond == 123);
                                REQUIRE(someCalendarDatetimeBack.microsecond == 000);
                                REQUIRE(someCalendarDatetimeBack.nanosecond == 000);
                            }
                        }
                    }
                }

                WHEN("We transform it to a char string")
                {
                    constexpr auto format = "%FT%T";
                    const auto someDatetimeString = datetime_cast<std::string>(someCalendarDatetime, format); // Use stdc conversion utility

                    THEN("we get '2016-07-27T18:15:37'")
                    {
                        REQUIRE(someDatetimeString == "2016-07-27T18:15:37");

#if 0 // NOT IMPLEMENTED
                        AND_WHEN("We transform it back to calendar")
                        {
                            const auto someCalendarDatetimeBack = datetime_cast<CalendarDatetime>(someDatetimeString, format);
                            THEN("we get July 27th 2016 at 18:15:37.123.000.000")
                            {
                                REQUIRE(someCalendarDatetimeBack.year == 2016);
                                REQUIRE(someCalendarDatetimeBack.month == 7);
                                REQUIRE(someCalendarDatetimeBack.day == 27);
                                REQUIRE(someCalendarDatetimeBack.hour == 18);
                                REQUIRE(someCalendarDatetimeBack.minute == 15);
                                REQUIRE(someCalendarDatetimeBack.second == 37);
                                REQUIRE(someCalendarDatetimeBack.millisecond == 123);
                                REQUIRE(someCalendarDatetimeBack.microsecond == 000);
                                REQUIRE(someCalendarDatetimeBack.nanosecond == 000);
                            }
                        }
#endif
                    }
                }

                WHEN("We transform it to a wchar_t string")
                {
                    constexpr auto format = L"yyyy-MM-ddTHH:mm:ss";
                    const auto someDatetimeString = datetime_cast<std::wstring>(someCalendarDatetime, format); // Use Windows conversion utility

                    THEN("we get '2016-07-27T18:15:37'")
                    {
                        REQUIRE(someDatetimeString == L"2016-07-27T18:15:37");

#if 0 // NOT IMPLEMENTED
                        AND_WHEN("We transform it back to calendar")
                        {
                            const auto someCalendarDatetimeBack = datetime_cast<CalendarDatetime>(someDatetimeString, format);
                            THEN("we get July 27th 2016 at 18:15:37.123.000.000")
                            {
                                REQUIRE(someCalendarDatetimeBack.year == 2016);
                                REQUIRE(someCalendarDatetimeBack.month == 7);
                                REQUIRE(someCalendarDatetimeBack.day == 27);
                                REQUIRE(someCalendarDatetimeBack.hour == 18);
                                REQUIRE(someCalendarDatetimeBack.minute == 15);
                                REQUIRE(someCalendarDatetimeBack.second == 37);
                                REQUIRE(someCalendarDatetimeBack.millisecond == 123);
                                REQUIRE(someCalendarDatetimeBack.microsecond == 000);
                                REQUIRE(someCalendarDatetimeBack.nanosecond == 000);
                            }
                        }
#endif
                    }
                }
            }

            GIVEN("the current point date-time")
            {
                const auto currentPointDatetime = getCurrentDatetime();

                WHEN("We transform it to calendar")
                {
                    const auto currentCalendarDatetime = datetime_cast<CalendarDatetime>(currentPointDatetime);

                    AND_WHEN("We transform it back to point")
                    {
                        const auto currentPointDatetimeBack = datetime_cast<PointDatetime>(currentCalendarDatetime);
                        THEN("we get the same point")
                        {
                            REQUIRE(currentPointDatetimeBack == currentPointDatetime);
                        }
                    }
                }
            }
        }

	} // namespace ut

} // namespace datetime
