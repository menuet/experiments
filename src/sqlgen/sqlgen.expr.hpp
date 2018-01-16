
#pragma once


#include <string>
#include <sstream>
#include <type_traits>


namespace sqlgen {

    namespace v1 {

        struct State {
            std::size_t dataIndex{};
        };

        template< typename DerivedT >
        class Base
        {
        public:

            template< typename OStreamT >
            constexpr OStreamT& generateSql(OStreamT& os, State& state) const
            {
                const auto& l_self = self();
                return l_self.generateSql(os, state);
            }

        protected:

            constexpr DerivedT& self()
            {
                return static_cast<DerivedT&>(*this);
            }

            constexpr const DerivedT& self() const
            {
                return static_cast<const DerivedT&>(*this);
            }
        };

        template< typename DataT >
        class Operand {
        public:
            using DataType = DataT;
        };

        template< typename DataT, typename ClassT >
        class Column : public Base<Column<DataT, ClassT>>, public Operand<DataT>
        {
        public:

            constexpr Column(DataT ClassT::* pMember)
                : m_pMember(pMember)
            {
            }

            template< typename OStreamT >
            constexpr OStreamT& generateSql(OStreamT& os, State& state) const
            {
                constexpr auto tableDef = getTableDefinition(static_cast<ClassT*>(nullptr));
                constexpr auto colDefs = tableDef.columnDefinitions;
                const auto columnName = findColumnName(colDefs, m_pMember);
                os << "`" << tableDef.tableName << "`.`" << columnName << "`";
                return os;
            }

        private:

            DataT ClassT::* m_pMember;
        };

        template< typename DataT >
        class Value : public Base<Value<DataT>>, public Operand<DataT>
        {
        public:

            template< typename DataT2, typename = std::enable_if_t<std::is_convertible_v<DataT2, DataT> && !std::is_same_v<DataT2, Value>> >
            constexpr explicit Value(const DataT2& data)
                : m_data(data)
            {
            }

            template< typename OStreamT >
            constexpr OStreamT& generateSql(OStreamT& os, State& state) const
            {
                const auto dataIndex = ++state.dataIndex;
                os << '?' << dataIndex;
                return os;
            }

        private:

            DataT m_data;
        };

        enum ComparisonOperator {
            Eq,
            Lt,
            Le,
            Gt,
            Ge,
        };

        template< typename OperandT1, typename OperandT2 >
        class Comparison : public Base<Comparison<OperandT1, OperandT2>>
        {
        public:

            constexpr Comparison(ComparisonOperator oper, const OperandT1& a_operand1, const OperandT2& a_operand2)
                : m_oper(oper)
                , m_operand1(a_operand1)
                , m_operand2(a_operand2)
            {
            }

            template< typename OStreamT >
            constexpr OStreamT& generateSql(OStreamT& os, State& state) const
            {
                m_operand1.generateSql(os, state);
                os << ' ';
                switch (m_oper) {
                case Eq: os << '='; break;
                case Lt: os << '<'; break;
                case Le: os << "<="; break;
                case Gt: os << '>'; break;
                case Ge: os << ">="; break;
                }
                os << ' ';
                m_operand2.generateSql(os, state);
                return os;
            }

        private:

            ComparisonOperator m_oper;
            OperandT1 m_operand1;
            OperandT2 m_operand2;
        };

        enum LogicalOperationOperator {
            Or,
            And,
        };

        template< typename OperandT1, typename OperandT2 >
        class LogicalOperation : public Base<LogicalOperation<OperandT1, OperandT2>>
        {
        public:

            constexpr LogicalOperation(LogicalOperationOperator oper, const OperandT1& a_operand1, const OperandT2& a_operand2)
                : m_oper(oper)
                , m_operand1(a_operand1)
                , m_operand2(a_operand2)
            {
            }

            template< typename OStreamT >
            constexpr OStreamT& generateSql(OStreamT& os, State& state) const
            {
                m_operand1.generateSql(os, state);
                os << ' ';
                switch (m_oper) {
                case Or: os << "OR"; break;
                case And: os << "AND"; break;
                }
                os << ' ';
                m_operand2.generateSql(os, state);
                return os;
            }

        private:

            LogicalOperationOperator m_oper;
            OperandT1 m_operand1;
            OperandT2 m_operand2;
        };

        template< typename DerivedT >
        inline std::string generateSql(const Base<DerivedT>& expr) {
            State state;
            std::stringstream ss;
            expr.generateSql(ss, state);
            return ss.str();
        }

    }

    namespace v2 {

        template <typename DataT, typename ClassT>
        struct Column
        {
        public:
            DataT ClassT::* m_pMember;
        };

        template <typename DataT>
        struct Value
        {
            DataT m_data;
        };

        struct DbNull {
        };

        constexpr DbNull dbNull{};

        enum ComparisonOperator {
            Eq,
            Ne,
            Lt,
            Le,
            Gt,
            Ge,
        };

        template <typename OperandT1, typename OperandT2>
        struct Comparison
        {
            ComparisonOperator m_oper;
            OperandT1 m_operand1;
            OperandT2 m_operand2;
        };

        enum LogicalOperationOperator {
            Or,
            And,
        };

        template <typename BoolExprT1, typename BoolExprT2>
        struct LogicalOperation
        {
            LogicalOperationOperator m_oper;
            BoolExprT1 m_boolExpr1;
            BoolExprT2 m_boolExpr2;
        };

        template <typename BoolExprT>
        struct LogicalNot
        {
            BoolExprT m_boolExpr;
        };

    }

} // namespace sqlgen
