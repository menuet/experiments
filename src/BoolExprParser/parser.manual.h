
#pragma once


#include <memory>
#include <cassert>
#include <cctype>



enum FieldType
{
    FieldType_Channel = 0,
    FieldType_Uuid,
    FieldType_Guid,
};

class ParseContext
{
public:

    ParseContext(const char* a_data, size_t a_length)
        : m_begin(a_data)
        , m_end(a_data + a_length)
        , m_current(a_data)
    {
    }

    bool isAtEnd() const
    {
        return m_current == m_end;
    }

    const char* getCurrentPointer() const
    {
        return m_current;
    }

    char getCurrentChar() const
    {
        return *m_current;
    }

    void moveNext()
    {
        assert(m_current != m_end);
        ++m_current;
    }

    void setCurrentPointer(const char* a_current)
    {
        m_current = a_current;
    }

private:

    const char* m_begin;
    const char* m_end;
    const char* m_current;
};


inline void parse_ignoreSpaces(ParseContext& a_context)
{
    while (!a_context.isAtEnd() && std::isspace(a_context.getCurrentChar()))
        a_context.moveNext();
}

inline bool parse_expectString(ParseContext& a_context, const char* a_expectedBegin, const char* a_expectedEnd)
{
    parse_ignoreSpaces(a_context);

    const auto l_savedPointer = a_context.getCurrentPointer();

    while (!a_context.isAtEnd() && a_expectedBegin != a_expectedEnd)
    {
        if (a_context.getCurrentChar() != *a_expectedBegin)
        {
            a_context.setCurrentPointer(l_savedPointer);
            return false;
        }
        a_context.moveNext();
        ++a_expectedBegin;
    }

    if (a_expectedBegin != a_expectedEnd)
    {
        a_context.setCurrentPointer(l_savedPointer);
        return false;
    }

    return true;
}

template< size_t ArraySizeV >
inline bool parse_expectString(ParseContext& a_context, const char(&a_expected)[ArraySizeV])
{
    return parse_expectString(a_context, a_expected, a_expected + ArraySizeV - 1);
}

inline bool parse_expectFieldName(ParseContext& a_context, FieldType& a_fieldType)
{
    if (parse_expectString(a_context, "CHANNEL"))
    {
        a_fieldType = FieldType_Channel;
        return true;
    }
    if (parse_expectString(a_context, "UUID"))
    {
        a_fieldType = FieldType_Uuid;
        return true;
    }
    if (parse_expectString(a_context, "GUID"))
    {
        a_fieldType = FieldType_Guid;
        return true;
    }
    return false;
}

inline bool parse_expectFieldValue(ParseContext& a_context, std::string& a_fieldValue)
{
    parse_ignoreSpaces(a_context);

    const auto l_savedPointer = a_context.getCurrentPointer();

    while (!a_context.isAtEnd())
    {
        const auto l_currentChar = a_context.getCurrentChar();
        if (std::isspace(l_currentChar) || l_currentChar == ')' || l_currentChar == ',')
        {
            if (!a_fieldValue.empty())
                return true;
            a_context.setCurrentPointer(l_savedPointer);
            return false;
        }
        a_fieldValue += l_currentChar;
        a_context.moveNext();
    }

    if (!a_fieldValue.empty())
        return true;
    a_context.setCurrentPointer(l_savedPointer);
    return false;
}

class Comparison
{
public:

    Comparison()
        : m_opType(OpType_Equal)
        , m_fieldType(FieldType_Channel)
    {
    }

    // Comparison ::= FieldName "=" FieldValue | FieldName "!=" FieldValue | FieldName "like" FieldValueRegex | FieldName "in" "(" FieldValueList ")"
    bool parse(ParseContext& a_context)
    {
        if (!parse_expectFieldName(a_context, m_fieldType))
            return false;

        if (parse_expectString(a_context, "==") || parse_expectString(a_context, "="))
        {
            if (!parse_expectFieldValue(a_context, m_fieldValue))
                return false;
            m_opType = OpType_Equal;
            return true;
        }

        if (parse_expectString(a_context, "!="))
        {
            if (!parse_expectFieldValue(a_context, m_fieldValue))
                return false;
            m_opType = OpType_NotEqual;
            return true;
        }

        if (parse_expectString(a_context, "like"))
        {
            // TODO
            return false;
        }

        if (parse_expectString(a_context, "in"))
        {
            // TODO
            return false;
        }

        return false;
    }

    template< typename EvalContextT >
    bool eval(const EvalContextT& a_context) const
    {
        switch (m_opType)
        {
        case OpType_Equal:
        {
            const auto& l_fieldValue = a_context.getFieldValue(m_fieldType);
            return m_fieldValue == l_fieldValue;
        }
        case OpType_NotEqual:
        {
            const auto& l_fieldValue = a_context.getFieldValue(m_fieldType);
            return m_fieldValue != l_fieldValue;
        }
        case OpType_Like:
            // TODO
            return false;
        case OpType_In:
            // TODO
            return false;
        }
        assert(!"m_opType does not have a valid value!!!");
        return false;
    }

private:

    enum OpType
    {
        OpType_Equal = 0,
        OpType_NotEqual,
        OpType_Like,
        OpType_In,
    };

    OpType m_opType;
    FieldType m_fieldType;
    std::string m_fieldValue;
};

class Expression;

class Term
{
public:

    Term()
        : m_opType(OpType_Unknown)
    {
    }

    // Term ::= Comparison | "(" Expression ")" | "not" "(" Expression ")"
    bool parse(ParseContext& a_context);

    template< typename EvalContextT >
    bool eval(const EvalContextT& a_context) const;

private:

    enum OpType
    {
        OpType_Unknown = 0,
        OpType_Comparison,
        OpType_Expression,
        OpType_NotExpression,
    };

    OpType m_opType;
    Comparison m_comparison;
    std::unique_ptr<Expression> m_expression;
};

class Expression
{
public:

    Expression()
        : m_opType(OpType_Unknown)
    {
    }

    // Expression ::= Term | Term "and" Expression | Term "or" Expression
    bool parse(ParseContext& a_context)
    {
        if (!m_left.parse(a_context))
            return false;

        if (parse_expectString(a_context, "and"))
        {
            m_right = std::make_unique<Expression>();
            if (!m_right->parse(a_context))
                return false;
            m_opType = OpType_And;
            return true;
        }

        if (parse_expectString(a_context, "or"))
        {
            m_right = std::make_unique<Expression>();
            if (!m_right->parse(a_context))
                return false;
            m_opType = OpType_Or;
            return true;
        }

        m_opType = OpType_None;
        return true;
    }

    template< typename EvalContextT >
    bool eval(const EvalContextT& a_context) const
    {
        switch (m_opType)
        {
        case OpType_None:
            if (!m_left.eval(a_context))
                return false;
            return true;
        case OpType_And:
            if (!m_left.eval(a_context))
                return false;
            assert(m_right);
            if (!m_right->eval(a_context))
                return false;
            return true;
        case OpType_Or:
            if (m_left.eval(a_context))
                return true;
            assert(m_right);
            if (m_right->eval(a_context))
                return true;
            return false;
        }
        assert(!"m_opType does not have a valid value!!!");
        return false;
    }

private:

    enum OpType
    {
        OpType_Unknown = 0,
        OpType_None,
        OpType_And,
        OpType_Or,
    };

    Term m_left;
    OpType m_opType;
    std::unique_ptr<Expression> m_right;
};

inline bool Term::parse(ParseContext& a_context)
{
    if (m_comparison.parse(a_context))
    {
        m_opType = OpType_Comparison;
        return true;
    }

    if (parse_expectString(a_context, "("))
    {
        m_expression = std::make_unique<Expression>();
        if (!m_expression->parse(a_context))
            return false;
        if (!parse_expectString(a_context, ")"))
            return false;
        m_opType = OpType_Expression;
        return true;
    }

    if (parse_expectString(a_context, "not"))
    {
        if (!parse_expectString(a_context, "("))
            return false;
        m_expression = std::make_unique<Expression>();
        if (!m_expression->parse(a_context))
            return false;
        if (!parse_expectString(a_context, ")"))
            return false;
        m_opType = OpType_NotExpression;
        return true;
    }

    return false;
}

template< typename EvalContextT >
inline bool Term::eval(const EvalContextT& a_context) const
{
    switch (m_opType)
    {
    case OpType_Comparison:
        if (!m_comparison.eval(a_context))
            return false;
        return true;
    case OpType_Expression:
        assert(m_expression);
        if (!m_expression->eval(a_context))
            return false;
        return true;
    case OpType_NotExpression:
        assert(m_expression);
        if (m_expression->eval(a_context))
            return false;
        return true;
    }
    assert(!"m_opType does not have a valid value!!!");
    return false;
}

class Parser
{
public:

    bool parse(const char* a_expr, size_t a_length)
    {
        ParseContext l_context(a_expr, a_length);
        m_expression = std::make_unique<Expression>();
        if (m_expression->parse(l_context))
            return true;
        m_expression.reset();
        return false;
    }

    template< size_t ArraySizeV >
    bool parse(const char(&a_expr)[ArraySizeV])
    {
        return parse(a_expr, ArraySizeV - 1);
    }

    template< typename EvalContextT >
    bool eval(const EvalContextT& a_context) const
    {
        assert(m_expression);
        return m_expression->eval(a_context);
    }

private:

    std::unique_ptr<Expression> m_expression;
};
