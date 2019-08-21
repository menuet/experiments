
#pragma once


#include "sqlgen.expr.hpp"
#include "sqlgen.traits.hpp"
#include <string>
#include <sstream>


namespace sqlgen {

    namespace v2 {

        namespace detail {

            struct SqlWhereState {
                std::size_t m_dataIndex{};
                std::stringstream m_os{};
            };

            template <typename StateT, typename DataT>
            inline void generateSqlWhere(StateT& state, const Value<DataT>&) {
                const auto dataIndex = ++state.m_dataIndex;
                state.m_os << '?' << dataIndex;
            }

            template <typename StateT, typename DataT, typename ClassT>
            inline void generateSqlWhere(StateT& state, const Column<DataT, ClassT>& column) {
                constexpr auto tableDef = getTableDefinition(static_cast<ClassT*>(nullptr));
                constexpr auto colDefs = tableDef.columnDefinitions;
                const auto columnName = findColumnName(colDefs, column.m_pMember);
                state.m_os << "`" << tableDef.tableName << "`.`" << columnName << "`";
            }

            template <typename StateT, typename OperandT1, typename OperandT2>
            inline void generateSqlWhere(StateT& state, const Comparison<OperandT1, OperandT2>& comparison) {
                generateSqlWhere(state, comparison.m_operand1);
                state.m_os << ' ';
                switch (comparison.m_oper) {
                case Eq: state.m_os << '='; break;
                case Ne: state.m_os << "<>"; break;
                case Lt: state.m_os << '<'; break;
                case Le: state.m_os << "<="; break;
                case Gt: state.m_os << '>'; break;
                case Ge: state.m_os << ">="; break;
                }
                state.m_os << ' ';
                generateSqlWhere(state, comparison.m_operand2);
            }

            template <typename StateT, typename OperandT1>
            inline void generateSqlWhere(StateT& state, const Comparison<OperandT1, DbNull>& comparison) {
                generateSqlWhere(state, comparison.m_operand1);
                state.m_os << ' ';
                switch (comparison.m_oper) {
                case Eq: state.m_os << "IS"; break;
                case Ne: state.m_os << "IS NOT"; break;
                case Lt: break; // TODO
                case Le: break; // TODO
                case Gt: break; // TODO
                case Ge: break; // TODO
                }
                state.m_os << " NULL";
            }

            template <typename BoolExprT, typename StateT>
            inline std::enable_if_t<!IsLogicalOperation<BoolExprT>>
                writeIfLogicalOperation(StateT&, char) {
            }

            template <typename BoolExprT, typename StateT>
            inline std::enable_if_t<IsLogicalOperation<BoolExprT>>
                writeIfLogicalOperation(StateT& state, char c) {
                state.m_os << c;
            }

            template <typename StateT, typename BoolExprT1, typename BoolExprT2>
            inline void generateSqlWhere(StateT& state, const LogicalOperation<BoolExprT1, BoolExprT2>& logical) {
                writeIfLogicalOperation<BoolExprT1>(state, '(');
                generateSqlWhere(state, logical.m_boolExpr1);
                writeIfLogicalOperation<BoolExprT1>(state, ')');
                state.m_os << ' ';
                switch (logical.m_oper) {
                case Or: state.m_os << "OR"; break;
                case And: state.m_os << "AND"; break;
                }
                state.m_os << ' ';
                writeIfLogicalOperation<BoolExprT2>(state, '(');
                generateSqlWhere(state, logical.m_boolExpr2);
                writeIfLogicalOperation<BoolExprT2>(state, ')');
            }

            template <typename StateT, typename BoolExprT>
            inline void generateSqlWhere(StateT& state, const LogicalNot<BoolExprT>& logicalNot) {
                state.m_os << "NOT (";
                generateSqlWhere(state, logicalNot.m_boolExpr);
                state.m_os << ')';
            }

        }

        template< typename Expr >
        inline std::string generateSqlWhere(const Expr& expr) {
            detail::SqlWhereState state{};
            detail::generateSqlWhere(state, expr);
            return state.m_os.str();
        }

    }

} // namespace sqlgen
