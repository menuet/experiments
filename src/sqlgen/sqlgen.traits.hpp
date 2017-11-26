
#pragma once


#include "sqlgen.expr.hpp"
#include <string>


namespace sqlgen {

    namespace v2 {

        namespace detail {

            template <typename T>
            struct IsOperandTrait : public std::false_type {
            };

            template <typename DataT, typename ClassT>
            struct IsOperandTrait<Column<DataT, ClassT>> : std::true_type {
            };

            template <typename DataT>
            struct IsOperandTrait<Value<DataT>> : std::true_type {
            };

            template <typename T, typename = void>
            struct IsDataTrait : public std::false_type {
            };

            template <typename T>
            struct IsDataTrait<T, std::enable_if_t<std::is_constructible_v<std::string, T>>> : std::true_type {
            };

            template <typename T>
            struct IsDataTrait<T, std::enable_if_t<std::is_arithmetic_v<T>>> : std::true_type {
            };

            template <typename T, typename = void>
            struct DataTypeTrait {
                using type = void;
            };

            template <typename DataT, typename ClassT>
            struct DataTypeTrait<Column<DataT, ClassT>> {
                using type = DataT;
            };

            template <typename DataT>
            struct DataTypeTrait<Value<DataT>> {
                using type = DataT;
            };

            template <typename T>
            struct DataTypeTrait<T, std::enable_if_t<std::is_constructible_v<std::string, T>>> {
                using type = std::string;
            };

            template <typename T>
            struct DataTypeTrait<T, std::enable_if_t<std::is_arithmetic_v<T>>> {
                using type = T;
            };

            template <typename T>
            struct IsNullTrait : std::false_type {
            };

            template <>
            struct IsNullTrait<DbNull> : std::true_type {
            };

            template <typename T>
            struct IsNullableTrait : std::false_type {
            };

            template <typename DataT, typename ClassT>
            struct IsNullableTrait<Column<DataT, ClassT>> : std::true_type {
            };

            template <typename T, typename = void>
            struct IsBooleanExpressionTrait : public std::false_type {
            };

            template <typename OperandT1, typename OperandT2>
            struct IsBooleanExpressionTrait<Comparison<OperandT1, OperandT2>> : public std::true_type {
            };

            template <typename BoolExprT1, typename BoolExprT2>
            struct IsBooleanExpressionTrait<LogicalOperation<BoolExprT1, BoolExprT2>> : public std::true_type {
            };

            template <typename BoolExprT>
            struct IsBooleanExpressionTrait<LogicalNot<BoolExprT>> : public std::true_type {
            };

            template <typename T, typename = void>
            struct IsLogicalOperationTrait : public std::false_type {
            };

            template <typename BoolExprT1, typename BoolExprT2>
            struct IsLogicalOperationTrait<LogicalOperation<BoolExprT1, BoolExprT2>> : public std::true_type {
            };

        }

        template <typename T>
        constexpr bool IsOperand = detail::IsOperandTrait<T>::value;

        template <typename T>
        constexpr bool IsData = detail::IsDataTrait<T>::value;

        template <typename T>
        using DataType = typename detail::DataTypeTrait<T>::type;

        template <typename T>
        constexpr bool IsNull = detail::IsNullTrait<T>::value;

        template <typename T>
        constexpr bool IsNullable = detail::IsNullableTrait<T>::value;

        template <typename OperandT1, typename OperandT2>
        constexpr bool AreComparable = (IsOperand<OperandT1> || IsOperand<OperandT2>) && std::is_same_v<DataType<OperandT1>, DataType<OperandT2>>;

        template <typename OperandT1, typename OperandT2>
        constexpr bool AreComparingToNull = (IsNullable<OperandT1> && IsNull<OperandT2>) || (IsNullable<OperandT2> && IsNull<OperandT1>);

        template <typename T>
        constexpr bool IsBooleanExpression = detail::IsBooleanExpressionTrait<T>::value;

        template <typename T>
        constexpr bool IsLogicalOperation = detail::IsLogicalOperationTrait<T>::value;

    }

} // namespace sqlgen
