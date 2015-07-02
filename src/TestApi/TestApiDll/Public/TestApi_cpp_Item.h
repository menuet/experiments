
#pragma once


#include "TestApi_c.h"
#include <string>
#include <utility>


namespace testapi {

    class ItemIterator;

    class Item
    {
        friend class ItemIterator;

    public:

        Item() = default;

        Item(const Item&) = default;

        Item(int i, const std::string& s)
            : m_i(i)
            , m_s(s)
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

        void swap(Item& item)
        {
            using std::swap;
            swap(m_i, item.m_i);
            swap(m_s, item.m_s);
        }

    private:

        int m_i = 0;
        std::string m_s;
    };

} // namespace testapi
