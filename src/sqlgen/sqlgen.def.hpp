
#pragma once


#include <tuple>
#include <utility>


namespace sqlgen {

    template< typename ClassT, typename DataT >
    class ColumnDefinition {
    public:

        const char* const columnName;
        DataT ClassT::* pMember;
    };

    template< typename ClassT, typename... DataTs >
    using ColumnDefinitions = std::tuple<ColumnDefinition<ClassT, DataTs>...>;

    template< typename ClassT, typename... DataTs >
    class TableDefinition {
    public:

        const char* const tableName;
        const ColumnDefinitions<ClassT, DataTs...> columnDefinitions;
    };

    template< typename ClassT, typename DataT >
    struct ColumnFinder {
        DataT ClassT::* pMember;
        const char*& pColumnName;

        constexpr bool operator()(size_t, const ColumnDefinition<ClassT, DataT>& colDef) const {
            if (pMember != colDef.pMember)
                return false;
            pColumnName = colDef.columnName;
            return true;
        }

        template< typename AnyColDefT >
        constexpr bool operator()(size_t, const AnyColDefT&) const {
            return false;
        }
    };

    template< typename ClassT, typename DataT >
    constexpr ColumnDefinition<ClassT, DataT> makeColumnDefinition(const char* columnName, DataT ClassT::* pMember) {
        return ColumnDefinition<ClassT, DataT>{columnName, pMember};
    }

    template< typename ClassT, typename... DataTs >
    constexpr ColumnDefinitions<ClassT, DataTs...> makeColumnDefinitions(const ColumnDefinition<ClassT, DataTs>&... colDefs) {
        return ColumnDefinitions<ClassT, DataTs...>(colDefs...);
    }

    template< typename ClassT, typename... DataTs >
    constexpr TableDefinition<ClassT, DataTs...> makeTableDefinition(const char* tableName, const ColumnDefinition<ClassT, DataTs>&... colDefs) {
        return TableDefinition<ClassT, DataTs...>{tableName, makeColumnDefinitions(colDefs...)};
    }

    template< typename TupleT, typename FunctorT, std::size_t... Is >
    constexpr void tuple_for_each_impl(const TupleT& tpl, FunctorT functor, std::index_sequence<Is...>) {
        (void)std::initializer_list<int>{ (functor(Is, std::get<Is>(tpl)), 0)... };
    }

    template< typename... Ts, typename FunctorT >
    constexpr void tuple_for_each(const std::tuple<Ts...>& tpl, FunctorT functor) {
        tuple_for_each_impl(tpl, functor, std::index_sequence_for<Ts...>{});
    }

    template< typename ClassT, typename... DataTs, typename DataT >
    constexpr const char* findColumnName(const ColumnDefinitions<ClassT, DataTs...>& colDefs, DataT ClassT::* pMember) {
        const char* columnName = nullptr;
        ColumnFinder<ClassT, DataT> columnFinder{ pMember, columnName };
        tuple_for_each(colDefs, columnFinder);
        return columnName;
    }

} // namespace sqlgen
