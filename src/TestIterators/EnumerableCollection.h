
#pragma once


#include "Item.h"
#include "Enumerable.h"
#include <list>
#include <cassert>


namespace api {

    class Enumerator : public dot_net_style::IEnumerator<Item>
    {
    public:

        Enumerator(const std::list<Item>& items)
            : m_items(items)
            , m_initialized(false)
            , m_iterCurrent()
        {
        }

        Enumerator(const Enumerator& enumerator)
            : m_items(enumerator.m_items)
            , m_initialized(enumerator.m_initialized)
            , m_iterCurrent(enumerator.m_iterCurrent)
        {
        }

        virtual const Item& Current() const override
        {
            assert(m_initialized);
            assert(m_iterCurrent != m_items.end());
            return *m_iterCurrent;
        }

        virtual bool MoveNext() override
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

        virtual void Reset() override
        {
            m_initialized = false;
        }

        virtual std::unique_ptr<IEnumerator<Item>> Clone() const override
        {
            return std::make_unique<Enumerator>(*this);
        }

        virtual bool Equal(const IEnumerator<Item>& comparable) const override
        {
            assert(dynamic_cast<const Enumerator*>(&comparable));
            const auto& enumerator = static_cast<const Enumerator&>(comparable);
            return &m_items == &enumerator.m_items && m_initialized == enumerator.m_initialized && m_iterCurrent == enumerator.m_iterCurrent;
        }

    private:

        const std::list<Item>& m_items;
        bool m_initialized;
        std::list<Item>::const_iterator m_iterCurrent;
    };

    class EnumerableCollection : public dot_net_style::IEnumerable<Item>
    {
    public:

        void addItem(const Item& item)
        {
            m_items.push_back(item);
        }

        virtual std::unique_ptr<dot_net_style::IEnumerator<Item>> GetEnumerator() const override
        {
            return std::make_unique<Enumerator>(m_items);
        }

    private:

        std::list<Item> m_items;
    };

} // namespace api
