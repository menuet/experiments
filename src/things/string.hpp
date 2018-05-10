
#pragma once

#include <cstddef>
#include <cstring>

class String
{
public:

    String(const char* data)
        : m_length{std::strlen(data)}
    {
        m_data = new char[m_length + 1];
        std::memcpy(m_data, data, m_length);
        m_data[m_length] = 0;
    }

    const char* c_str() const { return m_data; }

    std::size_t length() const { return m_length; }

private:

    char* m_data = nullptr;
    std::size_t m_length = 0;
};
