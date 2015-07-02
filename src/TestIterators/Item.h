
#pragma once


#include <string>


namespace api {

    class Item
    {
    public:

        Item() = default;

        Item(int i, const std::string& s)
            : m_i(i)
            , m_s(s)
        {
        }

        int getI() const
        {
            return m_i;
        }

        std::string getS() const
        {
            return m_s;
        }

    private:

        int m_i = 0;
        std::string m_s = {};
    };

} // namespace api
