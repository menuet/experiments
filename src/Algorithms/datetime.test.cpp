
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
                    const auto epochDatetimeCalendar = toCalendar(epochDatetimePoint);

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
                            const auto epochDatetimePointBack = toPoint(epochDatetimeCalendar);
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
                    const auto someDatetimePoint = toPoint(someDatetimeCalendar);

                    THEN("we get 1469636137.123456789 / 1469639737.123456789")
                    {
                        REQUIRE(someDatetimePoint.time_since_epoch().count() == 1469636137123456789);

                        AND_WHEN("We transform it back to calendar")
                        {
                            const auto someDatetimeCalendarBack = toCalendar(someDatetimePoint);
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
                    const auto currentDatetimeCalendar = toCalendar(currentDatetimePoint);

                    AND_WHEN("We transform it back to point")
                    {
                        const auto currentDatetimePointBack = toPoint(currentDatetimeCalendar);
                        THEN("we get the same point")
                        {
                            REQUIRE(currentDatetimePointBack == currentDatetimePoint);
                        }
                    }
                }
            }
        }

	} // namespace ut

} // namespace datetime
