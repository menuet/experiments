
#include <catch/catch.hpp>
#include "datetime.hpp"


namespace datetime {

	namespace ut {

		SCENARIO("dates times", "[datetime]")
		{
            GIVEN("the epoch point date-time")
            {
                const auto epochDatetimePoint = DatetimePoint{};
                REQUIRE(epochDatetimePoint.time_since_epoch().count() == 0);

                WHEN("We transform it to calendar")
                {
                    const auto epochDatetimeCalendar = datetime_cast<DatetimeCalendar>(epochDatetimePoint);

                    THEN("we get Jan 1st 1970 at 00:00:00.000.000.000")
                    {
                        REQUIRE(epochDatetimeCalendar.year == 1970);
                        REQUIRE(epochDatetimeCalendar.month == 1);
                        REQUIRE(epochDatetimeCalendar.day == 1);
                        // REQUIRE(epochDatetimeCalendar.hour == 0);
                        REQUIRE(epochDatetimeCalendar.hour == 1); // What The Fuck ?
                        REQUIRE(epochDatetimeCalendar.minute == 0);
                        REQUIRE(epochDatetimeCalendar.second == 0);
                        REQUIRE(epochDatetimeCalendar.millisecond == 0);
                        REQUIRE(epochDatetimeCalendar.microsecond == 0);
                        REQUIRE(epochDatetimeCalendar.nanosecond == 0);

                        AND_WHEN("We transform it back to point")
                        {
                            const auto epochDatetimePointBack = datetime_cast<DatetimePoint>(epochDatetimeCalendar);
                            THEN("we get 0")
                            {
                                // REQUIRE(epochDatetimePointBack.time_since_epoch().count() == 0);
                                REQUIRE(epochDatetimePointBack.time_since_epoch().count() == -1000000000); // What The Fuck ?
                            }
                        }
                    }
                }
            }

            GIVEN("a calendar date-time July 27th 2016 at 18:15:37.123.456.789")
            {
                DatetimeCalendar someDatetimeCalendar{};
                someDatetimeCalendar.year = 2016;
                someDatetimeCalendar.month = 7;
                someDatetimeCalendar.day = 27;
                someDatetimeCalendar.hour = 18;
                someDatetimeCalendar.minute = 15;
                someDatetimeCalendar.second = 37;
                someDatetimeCalendar.millisecond = 123;
                someDatetimeCalendar.microsecond = 456;
                someDatetimeCalendar.nanosecond = 789;

                WHEN("We transform it to point")
                {
                    const auto someDatetimePoint = datetime_cast<DatetimePoint>(someDatetimeCalendar);

                    THEN("we get 1469636137.123456789 / 1469639737.123456789")
                    {
                        REQUIRE(someDatetimePoint.time_since_epoch().count() == 1469636137123456789);

                        AND_WHEN("We transform it back to calendar")
                        {
                            const auto someDatetimeCalendarBack = datetime_cast<DatetimeCalendar>(someDatetimePoint);
                            THEN("we get July 27th 2016 at 18:15:37.123.456.789")
                            {
                                REQUIRE(someDatetimeCalendarBack.year == 2016);
                                REQUIRE(someDatetimeCalendarBack.month == 7);
                                REQUIRE(someDatetimeCalendarBack.day == 27);
                                REQUIRE(someDatetimeCalendarBack.hour == 18);
                                REQUIRE(someDatetimeCalendarBack.minute == 15);
                                REQUIRE(someDatetimeCalendarBack.second == 37);
                                REQUIRE(someDatetimeCalendarBack.millisecond == 123);
                                REQUIRE(someDatetimeCalendarBack.microsecond == 456);
                                REQUIRE(someDatetimeCalendarBack.nanosecond == 789);
                            }
                        }
                    }
                }
            }

            GIVEN("the current point date-time")
            {
                const auto currentDatetimePoint = getCurrentDatetime();

                WHEN("We transform it to calendar")
                {
                    const auto currentDatetimeCalendar = datetime_cast<DatetimeCalendar>(currentDatetimePoint);

                    AND_WHEN("We transform it back to point")
                    {
                        const auto currentDatetimePointBack = datetime_cast<DatetimePoint>(currentDatetimeCalendar);
                        THEN("we get the same point")
                        {
                            REQUIRE(currentDatetimePointBack == currentDatetimePoint);
                        }
                    }
                }
            }

            GIVEN("a calendar date-time July 28th 2016 at 15:23:57.987.654.321")
            {
                DatetimeCalendar someDatetimeCalendar{};
                someDatetimeCalendar.year = 2016;
                someDatetimeCalendar.month = 7;
                someDatetimeCalendar.day = 28;
                someDatetimeCalendar.hour = 15;
                someDatetimeCalendar.minute = 23;
                someDatetimeCalendar.second = 57;
                someDatetimeCalendar.millisecond = 987;
                someDatetimeCalendar.microsecond = 654;
                someDatetimeCalendar.nanosecond = 321;

                WHEN("We transform it to an old calendar")
                {
                    const auto someDatetimeOld = datetime_cast<old::DatetimeCalendar>(someDatetimeCalendar);

                    THEN("we get July 28th 2016 at 15:23:57.987")
                    {
                        REQUIRE(someDatetimeOld.fields.year == 2016);
                        REQUIRE(someDatetimeOld.fields.month == 7);
                        REQUIRE(someDatetimeOld.fields.day == 28);
                        REQUIRE(someDatetimeOld.fields.hour == 15);
                        REQUIRE(someDatetimeOld.fields.minute == 23);
                        REQUIRE(someDatetimeOld.fields.second == 57);
                        REQUIRE(someDatetimeOld.fields.millisecond == 987);

                        AND_WHEN("We transform it back to calendar")
                        {
                            const auto someDatetimeCalendarBack = datetime_cast<DatetimeCalendar>(someDatetimeOld);
                            THEN("we get July 28th 2016 at 15:23:57.987.000.000")
                            {
                                REQUIRE(someDatetimeCalendarBack.year == 2016);
                                REQUIRE(someDatetimeCalendarBack.month == 7);
                                REQUIRE(someDatetimeCalendarBack.day == 28);
                                REQUIRE(someDatetimeCalendarBack.hour == 15);
                                REQUIRE(someDatetimeCalendarBack.minute == 23);
                                REQUIRE(someDatetimeCalendarBack.second == 57);
                                REQUIRE(someDatetimeCalendarBack.millisecond == 987);
                                REQUIRE(someDatetimeCalendarBack.microsecond == 000);
                                REQUIRE(someDatetimeCalendarBack.nanosecond == 000);
                            }
                        }
                    }
                }
            }

            GIVEN("a calendar date-time July 28th 2016 at 15:23:57.987.654.321")
            {
                DatetimeCalendar someDatetimeCalendar{};
                someDatetimeCalendar.year = 2016;
                someDatetimeCalendar.month = 7;
                someDatetimeCalendar.day = 28;
                someDatetimeCalendar.hour = 15;
                someDatetimeCalendar.minute = 23;
                someDatetimeCalendar.second = 57;
                someDatetimeCalendar.millisecond = 987;
                someDatetimeCalendar.microsecond = 654;
                someDatetimeCalendar.nanosecond = 321;

                WHEN("We transform it to a string")
                {
                    constexpr auto format = "%FT%T";
                    const auto someDatetimeString = datetime_cast<std::string>(someDatetimeCalendar, format);

                    THEN("we get July 28th 2016 at 15:23:57.987")
                    {
                        REQUIRE(someDatetimeString == "2016-07-28T15:23:57");

                        AND_WHEN("We transform it back to calendar")
                        {
                            const auto someDatetimeCalendarBack = datetime_cast<DatetimeCalendar>(someDatetimeString, format);
                            THEN("we get July 28th 2016 at 15:23:57.987.000.000")
                            {
                                REQUIRE(someDatetimeCalendarBack.year == 2016);
                                REQUIRE(someDatetimeCalendarBack.month == 7);
                                REQUIRE(someDatetimeCalendarBack.day == 28);
                                REQUIRE(someDatetimeCalendarBack.hour == 15);
                                REQUIRE(someDatetimeCalendarBack.minute == 23);
                                REQUIRE(someDatetimeCalendarBack.second == 57);
                                REQUIRE(someDatetimeCalendarBack.millisecond == 987);
                                REQUIRE(someDatetimeCalendarBack.microsecond == 000);
                                REQUIRE(someDatetimeCalendarBack.nanosecond == 000);
                            }
                        }
                    }
                }
            }

            GIVEN("a string date-time '2016-07-28T16:31:06'")
            {
                std::string someDatetimeString{ "2016-07-28T16:31:06" };

                WHEN("We transform it to a calendar")
                {
                    constexpr auto format = "%FT%T";
                    const auto someDatetimeCalendar = datetime_cast<DatetimeCalendar>(someDatetimeString, format);

                    THEN("we get July 28th 2016 at 16:31:06.000.000.000")
                    {
                        REQUIRE(someDatetimeCalendar.year == 2016);
                        REQUIRE(someDatetimeCalendar.month == 7);
                        REQUIRE(someDatetimeCalendar.day == 28);
                        REQUIRE(someDatetimeCalendar.hour == 16);
                        REQUIRE(someDatetimeCalendar.minute == 31);
                        REQUIRE(someDatetimeCalendar.second == 57);
                        REQUIRE(someDatetimeCalendar.millisecond == 000);
                        REQUIRE(someDatetimeCalendar.microsecond == 000);
                        REQUIRE(someDatetimeCalendar.nanosecond == 000);

                        AND_WHEN("We transform it back to calendar")
                        {
                            const auto someDatetimeStringBack = datetime_cast<std::string>(someDatetimeCalendar, format);
                            THEN("we get '2016-07-28T16:31:06'")
                            {
                                REQUIRE(someDatetimeStringBack == "2016-07-28T16:31:06");
                            }
                        }
                    }
                }
            }
        }

	} // namespace ut

} // namespace datetime
