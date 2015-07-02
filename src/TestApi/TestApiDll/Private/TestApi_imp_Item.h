
#pragma once


#include <string>
#include <utility>


namespace testapi { namespace imp {

    class Item
    {
    public:

        Item() = default;

        Item(const Item&) = default;

        Item(int i, const char* sData, size_t sLength)
            : m_i(i)
            , m_s(sData, sLength)
        {
        }

        int getI() const
        {
            return m_i;
        }

        const std::string& getS() const
        {
            return m_s;
        }

    private:

        int m_i = 0;
        std::string m_s;
    };

} } // namespace testapi::imp
