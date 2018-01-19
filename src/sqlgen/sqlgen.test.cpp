
#include "sqlgen.hpp"
#include <catch.hpp>
#include <sstream>


namespace sample {

    struct Person {
        std::string id{};
        double weight{};
        int age{};
    };

    constexpr auto TableDefinition_for_Person = sqlgen::makeTableDefinition<Person>(
        "person",
        sqlgen::makeColumnDefinition("id", &Person::id),
        sqlgen::makeColumnDefinition("weight", &Person::weight),
        sqlgen::makeColumnDefinition("age", &Person::age)
        );

    constexpr auto getTableDefinition(Person*) -> decltype(TableDefinition_for_Person) {
        return TableDefinition_for_Person;
    }

}

namespace sqlgen {

constexpr auto SELECT_STATEMENT =
R"sql(
    SELECT `Person`.`id`, `Person`.`name`, `Person`.`age`
    FROM `Person`
    WHERE `Person`.`name` = ?1 OR `Person`.`age` > ?2 ORDER BY `Person`.`id`
)sql";


TEST_CASE("sqlgen", "[where]") {

    SECTION("DEFINITIONS") {

        sample::Person person{ "some id", 65.5, 42 };

        SECTION("makeColumnDefinitions") {
            const auto tableDef = sample::TableDefinition_for_Person;
            REQUIRE(tableDef.tableName == std::string("person"));
            const auto& colDefs = tableDef.columnDefinitions;
            {
                const auto colDef = std::get<0>(colDefs);
                REQUIRE(colDef.columnName == std::string("id"));
                REQUIRE(&(person.*(colDef.pMember)) == &person.id);
                person.*(colDef.pMember) = "other id";
                REQUIRE((person.*(colDef.pMember)) == "other id");
            }
            {
                const auto colDef = std::get<1>(colDefs);
                REQUIRE(colDef.columnName == std::string("weight"));
                REQUIRE(&(person.*(colDef.pMember)) == &person.weight);
                person.*(colDef.pMember) = 78.2;
                REQUIRE((person.*(colDef.pMember)) == 78.2);
            }
            {
                const auto colDef = std::get<2>(colDefs);
                REQUIRE(colDef.columnName == std::string("age"));
                REQUIRE(&(person.*(colDef.pMember)) == &person.age);
                person.*(colDef.pMember) = 35;
                REQUIRE((person.*(colDef.pMember)) == 35);
            }
        }
    }

    SECTION("TRAITS") {
        SECTION("IsOperand") {
            {
                static constexpr auto result = v2::IsOperand<std::string>;
                REQUIRE(!result);
            }
            {
                static constexpr auto result = v2::IsOperand<v2::Value<int>>;
                REQUIRE(result);
            }
            {
                static constexpr auto result = v2::IsOperand<v2::Column<double, sample::Person>>;
                REQUIRE(result);
            }
            {
                static constexpr auto result = v2::IsOperand<v2::DbNull>;
                REQUIRE(!result);
            }
        }
        SECTION("IsData") {
            {
                static constexpr auto result = v2::IsData<std::string>;
                REQUIRE(result);
            }
            {
                static constexpr auto result = v2::IsData<v2::Value<int>>;
                REQUIRE(!result);
            }
            {
                static constexpr auto result = v2::IsData<v2::Column<double, sample::Person>>;
                REQUIRE(!result);
            }
            {
                static constexpr auto result = v2::IsData<v2::DbNull>;
                REQUIRE(!result);
            }
        }
        SECTION("DataType") {
            {
                static constexpr auto result = std::is_same_v<void, v2::DataType<sample::Person>>;
                REQUIRE(result);
            }
            {
                static constexpr auto result = std::is_same_v<std::string, v2::DataType<std::string>>;
                REQUIRE(result);
            }
            {
                static constexpr auto result = std::is_same_v<int, v2::DataType<v2::Value<int>>>;
                REQUIRE(result);
            }
            {
                static constexpr auto result = std::is_same_v<double, v2::DataType<v2::Column<double, sample::Person>>>;
                REQUIRE(result);
            }
        }
        SECTION("IsBooleanExpression") {
            {
                static constexpr auto result = v2::IsBooleanExpression<std::string>;
                REQUIRE(!result);
            }
            {
                static constexpr auto result = v2::IsBooleanExpression<v2::Value<int>>;
                REQUIRE(!result);
            }
            {
                static constexpr auto result = v2::IsBooleanExpression<v2::Column<double, sample::Person>>;
                REQUIRE(!result);
            }
            {
                using Comp = v2::Comparison<v2::Value<int>, v2::Column<int, sample::Person>>;
                static constexpr auto result = v2::IsBooleanExpression<Comp>;
                REQUIRE(result);
            }
            {
                using Comp1 = v2::Comparison<v2::Value<int>, v2::Column<int, sample::Person>>;
                using Comp2 = v2::Comparison<v2::Value<std::string>, v2::Column<std::string, sample::Person>>;
                using BoolExpr = v2::LogicalOperation<Comp1, Comp2>;
                static constexpr auto result = v2::IsBooleanExpression<BoolExpr>;
                REQUIRE(result);
            }
        }
        SECTION("wrapData") {
            {
                const auto& wrapped = v2::detail::wrapData(123);
                static constexpr auto result = std::is_same_v<std::decay_t<decltype(wrapped)>, v2::Value<int>>;
                REQUIRE(result);
            }
            {
                const auto& wrapped = v2::detail::wrapData(std::string("hello"));
                static constexpr auto result = std::is_same_v<std::decay_t<decltype(wrapped)>, v2::Value<std::string>>;
                REQUIRE(result);
            }
            {
                const auto& wrapped = v2::detail::wrapData(v2::Value<int>{123});
                static constexpr auto result = std::is_same_v<std::decay_t<decltype(wrapped)>, v2::Value<int>>;
                REQUIRE(result);
            }
            {
                const auto& wrapped = v2::detail::wrapData(v2::Column<double, sample::Person>{&sample::Person::weight});
                static constexpr auto result = std::is_same_v<std::decay_t<decltype(wrapped)>, v2::Column<double, sample::Person>>;
                REQUIRE(result);
            }
            {
                const auto& wrapped = v2::detail::wrapData(v2::dbNull);
                static constexpr auto result = std::is_same_v<std::decay_t<decltype(wrapped)>, v2::DbNull>;
                REQUIRE(result);
            }
        }
    }

    SECTION("COL") {
        SECTION("col(std::string)") {
            const auto pMember = &sample::Person::id;
            const auto column = v1::col(pMember);

            const auto generatedSql = v1::generateSql(column);

            REQUIRE(generatedSql == "`person`.`id`");
        }

        SECTION("col(double)") {
            const auto pMember = &sample::Person::weight;
            const auto column = v1::col(pMember);

            const auto generatedSql = v1::generateSql(column);

            REQUIRE(generatedSql == "`person`.`weight`");
        }

        SECTION("col(int)") {
            const auto pMember = &sample::Person::age;
            const auto column = v1::col(pMember);

            const auto generatedSql = v1::generateSql(column);

            REQUIRE(generatedSql == "`person`.`age`");
        }

        SECTION("col(std::string)") {
            const auto pMember = &sample::Person::id;
            const auto column = v2::col(pMember);

            const auto generatedSql = v2::generateSqlWhere(column);

            REQUIRE(generatedSql == "`person`.`id`");
        }

        SECTION("col(double)") {
            const auto pMember = &sample::Person::weight;
            const auto column = v2::col(pMember);

            const auto generatedSql = v2::generateSqlWhere(column);

            REQUIRE(generatedSql == "`person`.`weight`");
        }

        SECTION("col(int)") {
            const auto pMember = &sample::Person::age;
            const auto column = v2::col(pMember);

            const auto generatedSql = v2::generateSqlWhere(column);

            REQUIRE(generatedSql == "`person`.`age`");
        }
    }

    SECTION("COMPARISON") {
        SECTION("col(std::string) == std::string") {
            const auto cmp = v1::col(&sample::Person::id) == std::string("some id");

            const auto generatedSql = v1::generateSql(cmp);

            REQUIRE(generatedSql == "`person`.`id` = ?1");
        }

        SECTION("col(std::string) == const char*") {
            const auto cmp = v1::col(&sample::Person::id) == "some id";

            const auto generatedSql = v1::generateSql(cmp);

            REQUIRE(generatedSql == "`person`.`id` = ?1");
        }

        SECTION("col(std::string) < std::string") {
            const auto cmp = v1::col(&sample::Person::id) < std::string("some id");

            const auto generatedSql = v1::generateSql(cmp);

            REQUIRE(generatedSql == "`person`.`id` < ?1");
        }

        SECTION("col(std::string) < const char*") {
            const auto cmp = v1::col(&sample::Person::id) < "some id";

            const auto generatedSql = v1::generateSql(cmp);

            REQUIRE(generatedSql == "`person`.`id` < ?1");
        }

        SECTION("col(std::string) <= std::string") {
            const auto cmp = v1::col(&sample::Person::id) <= std::string("some id");

            const auto generatedSql = v1::generateSql(cmp);

            REQUIRE(generatedSql == "`person`.`id` <= ?1");
        }

        SECTION("col(std::string) <= const char*") {
            const auto cmp = v1::col(&sample::Person::id) <= "some id";

            const auto generatedSql = v1::generateSql(cmp);

            REQUIRE(generatedSql == "`person`.`id` <= ?1");
        }

        SECTION("col(std::string) > std::string") {
            const auto cmp = v1::col(&sample::Person::id) > std::string("some id");

            const auto generatedSql = v1::generateSql(cmp);

            REQUIRE(generatedSql == "`person`.`id` > ?1");
        }

        SECTION("col(std::string) > const char*") {
            const auto cmp = v1::col(&sample::Person::id) > "some id";

            const auto generatedSql = v1::generateSql(cmp);

            REQUIRE(generatedSql == "`person`.`id` > ?1");
        }

        SECTION("col(std::string) >= std::string") {
            const auto cmp = v1::col(&sample::Person::id) >= std::string("some id");

            const auto generatedSql = v1::generateSql(cmp);

            REQUIRE(generatedSql == "`person`.`id` >= ?1");
        }

        SECTION("col(std::string) >= const char*") {
            const auto cmp = v1::col(&sample::Person::id) >= "some id";

            const auto generatedSql = v1::generateSql(cmp);

            REQUIRE(generatedSql == "`person`.`id` >= ?1");
        }

        SECTION("col(std::string) == std::string") {
            const auto cmp = v2::col(&sample::Person::id) == std::string("some id");
            static_assert(std::is_same_v<decltype(cmp), const v2::Comparison<v2::Column<std::string, sample::Person>, v2::Value<std::string>>>, "bad type");

            const auto generatedSql = v2::generateSqlWhere(cmp);

            REQUIRE(generatedSql == "`person`.`id` = ?1");
        }

        SECTION("col(std::string) == const char*") {
            const auto cmp = v2::col(&sample::Person::id) == "some id";
            static_assert(std::is_same_v<decltype(cmp), const v2::Comparison<v2::Column<std::string, sample::Person>, v2::Value<std::string>>>, "bad type");

            const auto generatedSql = v2::generateSqlWhere(cmp);

            REQUIRE(generatedSql == "`person`.`id` = ?1");
        }

        SECTION("col(std::string) < std::string") {
            const auto cmp = v2::col(&sample::Person::id) < std::string("some id");
            static_assert(std::is_same_v<decltype(cmp), const v2::Comparison<v2::Column<std::string, sample::Person>, v2::Value<std::string>>>, "bad type");

            const auto generatedSql = v2::generateSqlWhere(cmp);

            REQUIRE(generatedSql == "`person`.`id` < ?1");
        }

        SECTION("col(std::string) < const char*") {
            const auto cmp = v2::col(&sample::Person::id) < "some id";
            static_assert(std::is_same_v<decltype(cmp), const v2::Comparison<v2::Column<std::string, sample::Person>, v2::Value<std::string>>>, "bad type");

            const auto generatedSql = v2::generateSqlWhere(cmp);

            REQUIRE(generatedSql == "`person`.`id` < ?1");
        }

        SECTION("col(std::string) <= std::string") {
            const auto cmp = v2::col(&sample::Person::id) <= std::string("some id");
            static_assert(std::is_same_v<decltype(cmp), const v2::Comparison<v2::Column<std::string, sample::Person>, v2::Value<std::string>>>, "bad type");

            const auto generatedSql = v2::generateSqlWhere(cmp);

            REQUIRE(generatedSql == "`person`.`id` <= ?1");
        }

        SECTION("col(std::string) <= const char*") {
            const auto cmp = v2::col(&sample::Person::id) <= "some id";
            static_assert(std::is_same_v<decltype(cmp), const v2::Comparison<v2::Column<std::string, sample::Person>, v2::Value<std::string>>>, "bad type");

            const auto generatedSql = v2::generateSqlWhere(cmp);

            REQUIRE(generatedSql == "`person`.`id` <= ?1");
        }

        SECTION("col(std::string) > std::string") {
            const auto cmp = v2::col(&sample::Person::id) > std::string("some id");
            static_assert(std::is_same_v<decltype(cmp), const v2::Comparison<v2::Column<std::string, sample::Person>, v2::Value<std::string>>>, "bad type");

            const auto generatedSql = v2::generateSqlWhere(cmp);

            REQUIRE(generatedSql == "`person`.`id` > ?1");
        }

        SECTION("col(std::string) > const char*") {
            const auto cmp = v2::col(&sample::Person::id) > "some id";
            static_assert(std::is_same_v<decltype(cmp), const v2::Comparison<v2::Column<std::string, sample::Person>, v2::Value<std::string>>>, "bad type");

            const auto generatedSql = v2::generateSqlWhere(cmp);

            REQUIRE(generatedSql == "`person`.`id` > ?1");
        }

        SECTION("col(std::string) >= std::string") {
            const auto cmp = v2::col(&sample::Person::id) >= std::string("some id");
            static_assert(std::is_same_v<decltype(cmp), const v2::Comparison<v2::Column<std::string, sample::Person>, v2::Value<std::string>>>, "bad type");

            const auto generatedSql = v2::generateSqlWhere(cmp);

            REQUIRE(generatedSql == "`person`.`id` >= ?1");
        }

        SECTION("col(std::string) >= const char*") {
            const auto cmp = v2::col(&sample::Person::id) >= "some id";
            static_assert(std::is_same_v<decltype(cmp), const v2::Comparison<v2::Column<std::string, sample::Person>, v2::Value<std::string>>>, "bad type");

            const auto generatedSql = v2::generateSqlWhere(cmp);

            REQUIRE(generatedSql == "`person`.`id` >= ?1");
        }

        SECTION("col(std::string) != std::string") {
            const auto cmp = v2::col(&sample::Person::id) != std::string("some id");
            static_assert(std::is_same_v<decltype(cmp), const v2::Comparison<v2::Column<std::string, sample::Person>, v2::Value<std::string>>>, "bad type");

            const auto generatedSql = v2::generateSqlWhere(cmp);

            REQUIRE(generatedSql == "`person`.`id` <> ?1");
        }

        SECTION("col(std::string) != const char*") {
            const auto cmp = v2::col(&sample::Person::id) != "some id";
            static_assert(std::is_same_v<decltype(cmp), const v2::Comparison<v2::Column<std::string, sample::Person>, v2::Value<std::string>>>, "bad type");

            const auto generatedSql = v2::generateSqlWhere(cmp);

            REQUIRE(generatedSql == "`person`.`id` <> ?1");
        }

        SECTION("col(std::string) == dbNull") {
            const auto cmp = v2::col(&sample::Person::id) == v2::dbNull;
            static_assert(std::is_same_v<decltype(cmp), const v2::Comparison<v2::Column<std::string, sample::Person>, v2::DbNull>>, "bad type");

            const auto generatedSql = v2::generateSqlWhere(cmp);

            REQUIRE(generatedSql == "`person`.`id` IS NULL");
        }

        SECTION("col(std::string) != dbNull") {
            const auto cmp = v2::col(&sample::Person::id) != v2::dbNull;
            static_assert(std::is_same_v<decltype(cmp), const v2::Comparison<v2::Column<std::string, sample::Person>, v2::DbNull>>, "bad type");

            const auto generatedSql = v2::generateSqlWhere(cmp);

            REQUIRE(generatedSql == "`person`.`id` IS NOT NULL");
        }
    }

    SECTION("LOGIC") {
        SECTION("COMP AND COMP") {
            const auto logic = v1::col(&sample::Person::id) == std::string("some id") && v1::col(&sample::Person::age) == 12;

            const auto generatedSql = v1::generateSql(logic);

            REQUIRE(generatedSql == "`person`.`id` = ?1 AND `person`.`age` = ?2");
        }

        SECTION("COMP AND COMP AND COMP") {
            const auto logic = v1::col(&sample::Person::id) == std::string("some id") && v1::col(&sample::Person::age) == 12 && v1::col(&sample::Person::weight) == 65.5;

            const auto generatedSql = v1::generateSql(logic);

            REQUIRE(generatedSql == "`person`.`id` = ?1 AND `person`.`age` = ?2 AND `person`.`weight` = ?3");
        }

        SECTION("COMP OR COMP") {
            const auto logic = v1::col(&sample::Person::id) == "some id" || v1::col(&sample::Person::age) == 12;

            const auto generatedSql = v1::generateSql(logic);

            REQUIRE(generatedSql == "`person`.`id` = ?1 OR `person`.`age` = ?2");
        }

        SECTION("COMP OR COMP OR COMP") {
            const auto logic = v1::col(&sample::Person::id) == std::string("some id") || v1::col(&sample::Person::age) == 12 || v1::col(&sample::Person::weight) == 65.5;

            const auto generatedSql = v1::generateSql(logic);

            REQUIRE(generatedSql == "`person`.`id` = ?1 OR `person`.`age` = ?2 OR `person`.`weight` = ?3");
        }

        SECTION("COMP AND COMP OR COMP") {
            const auto logic = v1::col(&sample::Person::id) == std::string("some id") && v1::col(&sample::Person::age) == 12 || v1::col(&sample::Person::weight) == 65.5;

            const auto generatedSql = v1::generateSql(logic);

            REQUIRE(generatedSql == "`person`.`id` = ?1 AND `person`.`age` = ?2 OR `person`.`weight` = ?3");
        }

        SECTION("COMP AND COMP") {
            const auto logic = v2::col(&sample::Person::id) == std::string("some id") && v2::col(&sample::Person::age) == 12;

            const auto generatedSql = v2::generateSqlWhere(logic);

            REQUIRE(generatedSql == "`person`.`id` = ?1 AND `person`.`age` = ?2");
        }

        SECTION("COMP AND COMP AND COMP") {
            const auto logic = v2::col(&sample::Person::id) == std::string("some id") && v2::col(&sample::Person::age) == 12 && v2::col(&sample::Person::weight) == 65.5;

            const auto generatedSql = v2::generateSqlWhere(logic);

            REQUIRE(generatedSql == "(`person`.`id` = ?1 AND `person`.`age` = ?2) AND `person`.`weight` = ?3");
        }

        SECTION("COMP OR COMP") {
            const auto logic = v2::col(&sample::Person::id) == "some id" || v2::col(&sample::Person::age) == 12;

            const auto generatedSql = v2::generateSqlWhere(logic);

            REQUIRE(generatedSql == "`person`.`id` = ?1 OR `person`.`age` = ?2");
        }

        SECTION("COMP OR COMP OR COMP") {
            const auto logic = v2::col(&sample::Person::id) == std::string("some id") || v2::col(&sample::Person::age) == 12 || v2::col(&sample::Person::weight) == 65.5;

            const auto generatedSql = v2::generateSqlWhere(logic);

            REQUIRE(generatedSql == "(`person`.`id` = ?1 OR `person`.`age` = ?2) OR `person`.`weight` = ?3");
        }

        SECTION("COMP AND COMP OR COMP") {
            const auto logic = v2::col(&sample::Person::id) == std::string("some id") && v2::col(&sample::Person::age) == 12 || v2::col(&sample::Person::weight) == 65.5;

            const auto generatedSql = v2::generateSqlWhere(logic);

            REQUIRE(generatedSql == "(`person`.`id` = ?1 AND `person`.`age` = ?2) OR `person`.`weight` = ?3");
        }

        SECTION("COMP OR COMP AND COMP OR COMP") {
            const auto logic =
                v2::col(&sample::Person::id) == std::string("some id") ||
                v2::col(&sample::Person::age) == 12 &&
                v2::col(&sample::Person::weight) == 65.5 ||
                v2::col(&sample::Person::id) == "some id";

            const auto generatedSql = v2::generateSqlWhere(logic);

            REQUIRE(generatedSql == "(`person`.`id` = ?1 OR (`person`.`age` = ?2 AND `person`.`weight` = ?3)) OR `person`.`id` = ?4");
        }

        SECTION("(COMP OR COMP) AND (COMP OR COMP)") {
            const auto logic =
                (v2::col(&sample::Person::id) == std::string("some id") ||
                v2::col(&sample::Person::age) == 12)
                &&
                (v2::col(&sample::Person::weight) == 65.5 ||
                v2::col(&sample::Person::id) == "some id");

            const auto generatedSql = v2::generateSqlWhere(logic);

            REQUIRE(generatedSql == "(`person`.`id` = ?1 OR `person`.`age` = ?2) AND (`person`.`weight` = ?3 OR `person`.`id` = ?4)");
        }

        SECTION("NOT COMP") {
            const auto logic = ! (v2::col(&sample::Person::id) == "some id");

            const auto generatedSql = v2::generateSqlWhere(logic);

            REQUIRE(generatedSql == "NOT (`person`.`id` = ?1)");
        }

        SECTION("(COMP OR NOT COMP) AND NOT (COMP OR COMP)") {
            const auto logic =
                (v2::col(&sample::Person::id) == std::string("some id") ||
                ! (v2::col(&sample::Person::age) == 12))
                &&
                ! (v2::col(&sample::Person::weight) == 65.5 ||
                v2::col(&sample::Person::id) == "some id");

            const auto generatedSql = v2::generateSqlWhere(logic);

            REQUIRE(generatedSql == "(`person`.`id` = ?1 OR NOT (`person`.`age` = ?2)) AND NOT (`person`.`weight` = ?3 OR `person`.`id` = ?4)");
        }
    }

}

} // namespace sqlgen
