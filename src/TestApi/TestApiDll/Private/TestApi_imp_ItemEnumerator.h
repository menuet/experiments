
#pragma once


#include "../Public/TestApi_c.h"
#include "TestApi_imp_Item.h"
#include <vector>
#include <cassert>
#include <memory>


namespace testapi { namespace imp {

    class ItemEnumerator
    {
    public:

        ItemEnumerator(const std::vector<Item>& items)
            : m_items(items)
            , m_initialized(false)
            , m_iterCurrent()
        {
        }

        ItemEnumerator(const ItemEnumerator& enumerator)
            : m_items(enumerator.m_items)
            , m_initialized(enumerator.m_initialized)
            , m_iterCurrent(enumerator.m_iterCurrent)
        {
        }

        ItemEnumerator& operator=(const ItemEnumerator& enumerator) = delete;

        void getCurrent(testapi_Item& c_item) const
        {
            assert(m_initialized);
            assert(m_iterCurrent != m_items.end());
            const auto& currentItem = *m_iterCurrent;
            c_item.i = currentItem.getI();
            c_item.s.out.setter(c_item.s.out.object, currentItem.getS().c_str(), currentItem.getS().length());
        }

        bool moveNext()
        {
            if (!m_initialized)
            {
                m_initialized = true;
                m_iterCurrent = m_items.begin();
                return m_iterCurrent != m_items.end();
            }

            assert(m_iterCurrent != m_items.end());
            ++m_iterCurrent;
            return m_iterCurrent != m_items.end();
        }

        void reset()
        {
            m_initialized = false;
        }

        bool equal(const ItemEnumerator& enumerator) const
        {
            return &m_items == &enumerator.m_items && m_initialized == enumerator.m_initialized && m_iterCurrent == enumerator.m_iterCurrent;
        }

    private:

        const std::vector<Item>& m_items;
        bool m_initialized;
        std::vector<Item>::const_iterator m_iterCurrent;
    };

} } // namespace testapi::imp
