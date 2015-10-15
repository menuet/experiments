
#pragma once


namespace exprtemp {

    template< typename DerivedT >
    class ExpressionBase
    {
    public:

        DerivedT& self()
        {
            return static_cast<DerivedT&>(*this);
        }

        const DerivedT& self() const
        {
            return static_cast<const DerivedT&>(*this);
        }

        auto operator()() const
        {
            const auto& l_self = self();
            return l_self.eval();
        }

    private:
    };

    template< typename OperationT, typename OperandT >
    class UnaryOperation : public ExpressionBase<UnaryOperation<OperationT, OperandT>>
    {
    public:

        UnaryOperation(const OperandT& a_operand)
            : m_operand(a_operand)
        {
        }

        auto eval() const
        {
            return OperationT().apply(m_operand());
        }

    private:

        OperandT m_operand;
    };

    template< typename OperationT, typename DerivedT >
    inline auto makeOperation(const ExpressionBase<DerivedT>& a_operand)
    {
        return UnaryOperation<OperationT, DerivedT>(a_operand.self());
    }

    template< typename OperationT, typename OperandT1, typename OperandT2 >
    class BinaryOperation : public ExpressionBase<BinaryOperation<OperationT, OperandT1, OperandT2>>
    {
    public:

        BinaryOperation(const OperandT1& a_operand1, const OperandT2& a_operand2)
            : m_operand1(a_operand1)
            , m_operand2(a_operand2)
        {
        }

        auto eval() const
        {
            return OperationT().apply(m_operand1(), m_operand2());
        }

    private:

        OperandT1 m_operand1;
        OperandT2 m_operand2;
    };

    template< typename OperationT, typename DerivedT1, typename DerivedT2 >
    inline auto makeOperation(const ExpressionBase<DerivedT1>& a_operand1, const ExpressionBase<DerivedT2>& a_operand2)
    {
        return BinaryOperation<OperationT, DerivedT1, DerivedT2>(a_operand1.self(), a_operand2.self());
    }

    template< typename ValueT >
    class Terminal : public ExpressionBase<Terminal<ValueT>>
    {
    public:

        Terminal(const ValueT& a_value)
            : m_value(a_value)
        {
        }

        auto eval() const
        {
            return m_value;
        }

    private:

        ValueT m_value;
    };

    template< typename ValueT >
    inline auto val(const ValueT& a_value)
    {
        return Terminal<ValueT>(a_value);
    }

    class OperationMinus
    {
    public:

        template< typename OperandT >
        auto apply(const OperandT& a_operand) const
        {
            return -a_operand;
        }

        template< typename OperandT1, typename OperandT2 >
        auto apply(const OperandT1& a_operand1, const OperandT2& a_operand2) const
        {
            return a_operand1 - a_operand2;
        }
    };

    template< typename DerivedT >
    inline auto operator-(const ExpressionBase<DerivedT>& a_expr)
    {
        return makeOperation<OperationMinus>(a_expr);
    }

    template< typename DerivedT1, typename DerivedT2 >
    inline auto operator-(const ExpressionBase<DerivedT1>& a_expr1, const ExpressionBase<DerivedT2>& a_expr2)
    {
        return makeOperation<OperationMinus>(a_expr1, a_expr2);
    }

    class OperationPlus
    {
    public:

        template< typename OperandT1, typename OperandT2 >
        auto apply(const OperandT1& a_operand1, const OperandT2& a_operand2) const
        {
            return a_operand1 + a_operand2;
        }
    };

    template< typename DerivedT1, typename DerivedT2 >
    inline auto operator+(const ExpressionBase<DerivedT1>& a_expr1, const ExpressionBase<DerivedT2>& a_expr2)
    {
        return makeOperation<OperationPlus>(a_expr1, a_expr2);
    }

    class OperationTimes
    {
    public:

        template< typename OperandT1, typename OperandT2 >
        auto apply(const OperandT1& a_operand1, const OperandT2& a_operand2) const
        {
            return a_operand1 * a_operand2;
        }
    };

    template< typename DerivedT1, typename DerivedT2 >
    inline auto operator*(const ExpressionBase<DerivedT1>& a_expr1, const ExpressionBase<DerivedT2>& a_expr2)
    {
        return makeOperation<OperationTimes>(a_expr1, a_expr2);
    }

    class OperationDivision
    {
    public:

        template< typename OperandT1, typename OperandT2 >
        auto apply(const OperandT1& a_operand1, const OperandT2& a_operand2) const
        {
            return a_operand1 / a_operand2;
        }
    };

    template< typename DerivedT1, typename DerivedT2 >
    inline auto operator/(const ExpressionBase<DerivedT1>& a_expr1, const ExpressionBase<DerivedT2>& a_expr2)
    {
        return makeOperation<OperationDivision>(a_expr1, a_expr2);
    }

} // namespace exprtemp
