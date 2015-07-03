
#pragma once


#include <memory>
#include <cassert>
#include <cctype>



enum class FieldType
{
    Channel = 0,
    Uuid,
    Guid,
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

template< size_t ArraySizeV >
inline bool parse_expectString(ParseContext& a_context, const char(&a_expected)[ArraySizeV])
{
    return parse_expectString(a_context, a_expected, a_expected + ArraySizeV - 1);
}

class FieldValue
{
public:

    const std::string& getValue() const
    {
        return m_value;
    }

    bool parse(ParseContext& a_context)
    {
        return parse_expectFieldValue(a_context, m_value);
    }

private:

    std::string m_value;
};

class FieldName
{
public:

    FieldType getType() const
    {
        return m_type;
    }

    bool parse(ParseContext& a_context)
    {
        if (parse_expectString(a_context, "CHANNEL"))
        {
            m_type = FieldType::Channel;
            return true;
        }
        if (parse_expectString(a_context, "UUID"))
        {
            m_type = FieldType::Uuid;
            return true;
        }
        if (parse_expectString(a_context, "GUID"))
        {
            m_type = FieldType::Guid;
            return true;
        }
        return false;
    }

private:

    FieldType m_type = FieldType::Channel;
};

class Comparison
{
public:

    // Comparison ::= FieldName "=" FieldValue | FieldName "!=" FieldValue | FieldName "like" FieldValueRegex | FieldName "in" "(" FieldValueList ")"
    bool parse(ParseContext& a_context)
    {
        if (!m_fieldName.parse(a_context))
            return false;

        if (parse_expectString(a_context, "==") || parse_expectString(a_context, "="))
        {
            m_opType = OpType::Equal;
            return m_fieldValue.parse(a_context);
        }

        if (parse_expectString(a_context, "!="))
        {
            m_opType = OpType::NotEqual;
            return m_fieldValue.parse(a_context);
        }

        // TODO like, in

        return false;
    }

    template< typename EvalContextT >
    bool eval(const EvalContextT& a_context) const
    {
        switch (m_opType)
        {
        case OpType::Equal:
        {
            const auto& l_fieldValue = a_context.getFieldValue(m_fieldName.getType());
            return m_fieldValue.getValue() == l_fieldValue;
        }
        case OpType::NotEqual:
        {
            const auto& l_fieldValue = a_context.getFieldValue(m_fieldName.getType());
            return m_fieldValue.getValue() != l_fieldValue;
        }
        case OpType::Like:
            // TODO
            return false;
        case OpType::In:
            // TODO
            return false;
        }
        return false;
    }

private:

    enum class OpType
    {
        Equal = 0,
        NotEqual,
        Like,
        In,
    };

    OpType m_opType = OpType::Equal;
    FieldName m_fieldName;
    FieldValue m_fieldValue;
};

class Expression;

class Term
{
public:

    // Term ::= Comparison | "(" Expression ")" | "not" "(" Expression ")"
    bool parse(ParseContext& a_context);

    template< typename EvalContextT >
    bool eval(const EvalContextT& a_context) const;

private:

    enum class OpType
    {
        Comparison = 0,
        Expression,
        NotExpression,
    };

    OpType m_opType = OpType::Comparison;
    Comparison m_comparison;
    std::unique_ptr<Expression> m_expression;
};

class Expression
{
public:

    // Expression ::= Term | Term "and" Expression | Term "or" Expression
    bool parse(ParseContext& a_context)
    {
        if (!m_left.parse(a_context))
            return false;

        if (parse_expectString(a_context, "and"))
        {
            m_opType = OpType::And;
            m_right = std::make_unique<Expression>();
            return m_right->parse(a_context);
        }

        if (parse_expectString(a_context, "or"))
        {
            m_opType = OpType::Or;
            m_right = std::make_unique<Expression>();
            return m_right->parse(a_context);
        }

        return true;
    }

    template< typename EvalContextT >
    bool eval(const EvalContextT& a_context) const
    {
        if (!m_left.eval(a_context))
            return false;

        switch (m_opType)
        {
        case OpType::And:
            assert(m_right);
            return m_right->eval(a_context);
        case OpType::Or:
            return true;
        }

        return true;
    }

private:

    enum class OpType
    {
        None = 0,
        And,
        Or,
    };

    Term m_left;
    OpType m_opType = OpType::None;
    std::unique_ptr<Expression> m_right;
};

inline bool Term::parse(ParseContext& a_context)
{
    if (m_comparison.parse(a_context))
    {
        m_opType = OpType::Comparison;
        return true;
    }

    if (parse_expectString(a_context, "("))
    {
        m_opType = OpType::Expression;
        m_expression = std::make_unique<Expression>();
        if (!m_expression->parse(a_context))
            return false;
        return parse_expectString(a_context, ")");
    }

    if (parse_expectString(a_context, "not"))
    {
        m_opType = OpType::NotExpression;
        if (!parse_expectString(a_context, "("))
            return false;
        m_expression = std::make_unique<Expression>();
        if (!m_expression->parse(a_context))
            return false;
        return parse_expectString(a_context, ")");
    }

    return false;
}

template< typename EvalContextT >
inline bool Term::eval(const EvalContextT& a_context) const
{
    switch (m_opType)
    {
    case OpType::Expression:
        assert(m_expression);
        return m_expression->eval(a_context);
    case OpType::NotExpression:
        assert(m_expression);
        return !m_expression->eval(a_context);
    }
    return m_comparison.eval(a_context);
}

class Parser
{
public:

    bool parse(const char* a_expr, size_t a_length)
    {
        ParseContext l_context(a_expr, a_length);
        return m_expression.parse(l_context);
    }

    template< size_t ArraySizeV >
    bool parse(const char(&a_expr)[ArraySizeV])
    {
        return parse(a_expr, ArraySizeV - 1);
    }

    template< typename EvalContextT >
    bool eval(const EvalContextT& a_context) const
    {
        return m_expression.eval(a_context);
    }

private:

    Expression m_expression;
};
