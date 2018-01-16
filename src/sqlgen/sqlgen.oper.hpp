
#pragma once


#include "sqlgen.expr.hpp"
#include "sqlgen.traits.hpp"


namespace sqlgen {

    namespace v1 {

        namespace detail {

            template <typename DataT, template <typename...> class OperandT1, typename... Ts1, template <typename...> class OperandT2, typename... Ts2,
                typename = std::enable_if_t<
                std::is_base_of_v<Operand<DataT>, OperandT1<DataT, Ts1...>> && std::is_base_of_v<Operand<DataT>, OperandT2<DataT, Ts2...>>
                >
            >
                constexpr auto makeComparison(ComparisonOperator comparisonOperator, const OperandT1<DataT, Ts1...>& operand1, const OperandT2<DataT, Ts2...>& operand2)
            {
                return Comparison<OperandT1<DataT, Ts1...>, OperandT2<DataT, Ts2...>>(comparisonOperator, operand1, operand2);
            }

            template <typename DataT1, template <typename...> class OperandT1, typename... Ts1, typename DataT2,
                typename = std::enable_if_t<
                std::is_base_of_v<Operand<DataT1>, OperandT1<DataT1, Ts1...>> && std::is_convertible_v<DataT2, DataT1>
                >
            >
                constexpr auto makeComparison(ComparisonOperator comparisonOperator, const OperandT1<DataT1, Ts1...>& operand1, const DataT2& operand2)
            {
                return makeComparison(comparisonOperator, operand1, Value<DataT1>(operand2));
            }

            template <typename... OperandTs1, typename... OperandTs2 >
            constexpr auto makeLogicalOperation(LogicalOperationOperator oper, const Comparison<OperandTs1...>& cmp1, const Comparison<OperandTs2...>& cmp2)
            {
                return LogicalOperation<Comparison<OperandTs1...>, Comparison<OperandTs2...>>(oper, cmp1, cmp2);
            }

            template <typename... ComparisonTs, typename... OperandTs >
            constexpr auto makeLogicalOperation(LogicalOperationOperator oper, const LogicalOperation<ComparisonTs...>& logical1, const Comparison<OperandTs...>& cmp2)
            {
                return LogicalOperation<LogicalOperation<ComparisonTs...>, Comparison<OperandTs...>>(oper, logical1, cmp2);
            }

        }

        template <typename DataT, typename ClassT >
        constexpr auto col(DataT ClassT::* pMember)
        {
            return Column<DataT, ClassT>(pMember);
        }

        template <typename OperandT1, typename OperandT2>
        constexpr auto operator==(const OperandT1& operand1, const OperandT2& operand2)
        {
            return detail::makeComparison(ComparisonOperator::Eq, operand1, operand2);
        }

        template <typename OperandT1, typename OperandT2>
        constexpr auto operator<(const OperandT1& operand1, const OperandT2& operand2)
        {
            return detail::makeComparison(ComparisonOperator::Lt, operand1, operand2);
        }

        template <typename OperandT1, typename OperandT2>
        constexpr auto operator<=(const OperandT1& operand1, const OperandT2& operand2)
        {
            return detail::makeComparison(ComparisonOperator::Le, operand1, operand2);
        }

        template <typename OperandT1, typename OperandT2>
        constexpr auto operator>(const OperandT1& operand1, const OperandT2& operand2)
        {
            return detail::makeComparison(ComparisonOperator::Gt, operand1, operand2);
        }

        template <typename OperandT1, typename OperandT2>
        constexpr auto operator>=(const OperandT1& operand1, const OperandT2& operand2)
        {
            return detail::makeComparison(ComparisonOperator::Ge, operand1, operand2);
        }

        template <typename ComparisonT1, typename ComparisonT2 >
        constexpr auto operator ||(const ComparisonT1& cmp1, const ComparisonT2& cmp2)
        {
            return detail::makeLogicalOperation(LogicalOperationOperator::Or, cmp1, cmp2);
        }

        template <typename ComparisonT1, typename ComparisonT2 >
        constexpr auto operator &&(const ComparisonT1& cmp1, const ComparisonT2& cmp2)
        {
            return detail::makeLogicalOperation(LogicalOperationOperator::And, cmp1, cmp2);
        }

    }

    namespace v2 {

        namespace detail {

            constexpr const DbNull& wrapData(const DbNull& operand) {
                return operand;
            }

            template <typename OperandT>
            constexpr const OperandT& wrapData(const OperandT& operand, std::enable_if_t<IsOperand<OperandT>>* = nullptr) {
                return operand;
            }

            template <typename OperandT>
            constexpr auto wrapData(const OperandT& operand, std::enable_if_t<IsData<OperandT>>* = nullptr) {
                using OperandDataType = DataType<OperandT>;
                return Value<OperandDataType>{operand};
            }

            template <typename OperandT1, typename OperandT2>
            constexpr auto makeComparison(ComparisonOperator comparisonOperator, const OperandT1& operand1, const OperandT2& operand2) {
                const auto& wrappedOperand1 = wrapData(operand1);
                using OperandType1 = std::decay_t<decltype(wrappedOperand1)>;
                const auto& wrappedOperand2 = wrapData(operand2);
                using OperandType2 = std::decay_t<decltype(wrappedOperand2)>;
                return Comparison<OperandType1, OperandType2>{comparisonOperator, wrappedOperand1, wrappedOperand2};
            }

            template <typename BoolExprT1, typename BoolExprT2>
            constexpr auto makeLogicalOperation(LogicalOperationOperator logicalOperationOperator, const BoolExprT1& boolExpr1, const BoolExprT2& boolExpr2) {
                return LogicalOperation<BoolExprT1, BoolExprT2>{logicalOperationOperator, boolExpr1, boolExpr2};
            }

            template <typename BoolExprT>
            constexpr auto makeLogicalNot(const BoolExprT& boolExpr) {
                return LogicalNot<BoolExprT>{boolExpr};
            }

        }

        template <typename DataT, typename ClassT >
        constexpr auto col(DataT ClassT::* pMember) {
            return Column<DataT, ClassT>{pMember};
        }

        template <typename OperandT1, typename OperandT2,
            typename = std::enable_if_t<AreComparable<OperandT1, OperandT2> || AreComparingToNull<OperandT1, OperandT2>>
        >
        constexpr auto operator==(const OperandT1& operand1, const OperandT2& operand2) {
            return detail::makeComparison(ComparisonOperator::Eq, operand1, operand2);
        }

        template <typename OperandT1, typename OperandT2,
            typename = std::enable_if_t<AreComparable<OperandT1, OperandT2> || AreComparingToNull<OperandT1, OperandT2>>
        >
        constexpr auto operator!=(const OperandT1& operand1, const OperandT2& operand2) {
            return detail::makeComparison(ComparisonOperator::Ne, operand1, operand2);
        }

        template <typename OperandT1, typename OperandT2,
            typename = std::enable_if_t<AreComparable<OperandT1, OperandT2>>
        >
        constexpr auto operator<=(const OperandT1& operand1, const OperandT2& operand2) {
            return detail::makeComparison(ComparisonOperator::Le, operand1, operand2);
        }

        template <typename OperandT1, typename OperandT2,
            typename = std::enable_if_t<AreComparable<OperandT1, OperandT2>>
        >
        constexpr auto operator<(const OperandT1& operand1, const OperandT2& operand2) {
            return detail::makeComparison(ComparisonOperator::Lt, operand1, operand2);
        }

        template <typename OperandT1, typename OperandT2,
            typename = std::enable_if_t<AreComparable<OperandT1, OperandT2>>
        >
        constexpr auto operator>=(const OperandT1& operand1, const OperandT2& operand2) {
            return detail::makeComparison(ComparisonOperator::Ge, operand1, operand2);
        }

        template <typename OperandT1, typename OperandT2,
            typename = std::enable_if_t<AreComparable<OperandT1, OperandT2>>
        >
        constexpr auto operator>(const OperandT1& operand1, const OperandT2& operand2) {
            return detail::makeComparison(ComparisonOperator::Gt, operand1, operand2);
        }

        template <typename BoolExprT1, typename BoolExprT2,
            typename = std::enable_if_t<IsBooleanExpression<BoolExprT1> && IsBooleanExpression<BoolExprT2>>
        >
        constexpr auto operator&&(const BoolExprT1& boolExpr1, const BoolExprT2& boolExpr2)
        {
            return detail::makeLogicalOperation(LogicalOperationOperator::And, boolExpr1, boolExpr2);
        }

        template <typename BoolExprT1, typename BoolExprT2,
            typename = std::enable_if_t<IsBooleanExpression<BoolExprT1> && IsBooleanExpression<BoolExprT2>>
        >
        constexpr auto operator||(const BoolExprT1& boolExpr1, const BoolExprT2& boolExpr2)
        {
            return detail::makeLogicalOperation(LogicalOperationOperator::Or, boolExpr1, boolExpr2);
        }

        template <typename BoolExprT,
            typename = std::enable_if_t<IsBooleanExpression<BoolExprT>>
        >
        constexpr auto operator!(const BoolExprT& boolExpr)
        {
            return detail::makeLogicalNot(boolExpr);
        }

    }

} // namespace sqlgen
