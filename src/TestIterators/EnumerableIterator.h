
#pragma once


#include "EnumerableCollection.h"
#include <iterator>
#include <cassert>


namespace api {

    class EnumerableIterator : public std::iterator<std::forward_iterator_tag, Item>
    {
    public:

        EnumerableIterator() = default;

        EnumerableIterator(const EnumerableIterator& iterator)
            : m_spEnumerator(iterator.m_spEnumerator ? m_spEnumerator->Clone() : nullptr)
        {
        }

        EnumerableIterator(std::unique_ptr<dot_net_style::IEnumerator<Item>>&& spEnumerator)
            : m_spEnumerator(std::move(spEnumerator))
        {
            assert(m_spEnumerator);
            if (!m_spEnumerator->MoveNext())
                m_spEnumerator.reset();
        }

        EnumerableIterator& operator=(const EnumerableIterator& iterator)
        {
            EnumerableIterator iter(iterator);
            iter.swap(*this);
            return *this;
        }

        void swap(EnumerableIterator& iterator)
        {
            m_spEnumerator.swap(iterator.m_spEnumerator);
        }

        friend bool operator==(const EnumerableIterator& iter1, const EnumerableIterator& iter2);

        const Item& operator*() const
        {
            assert(m_spEnumerator);
            return m_spEnumerator->Current();
        }

        const Item* operator->() const
        {
            assert(m_spEnumerator);
            return &m_spEnumerator->Current();
        }

        EnumerableIterator& operator++()
        {
            assert(m_spEnumerator);
            if (!m_spEnumerator->MoveNext())
                m_spEnumerator.reset();
            return *this;
        }

        EnumerableIterator operator++(int)
        {
            EnumerableIterator iter(*this);
            assert(m_spEnumerator);
            if (!m_spEnumerator->MoveNext())
                m_spEnumerator.reset();
            return iter;
        }

    private:

        std::unique_ptr<dot_net_style::IEnumerator<Item>> m_spEnumerator;
    };

    inline bool operator==(const EnumerableIterator& iter1, const EnumerableIterator& iter2)
    {
        if (iter1.m_spEnumerator == iter2.m_spEnumerator)
            return true;
        if (iter1.m_spEnumerator == nullptr || iter2.m_spEnumerator == nullptr)
            return false;
        return iter1.m_spEnumerator->Equal(*iter2.m_spEnumerator);
    }

    inline bool operator!=(const EnumerableIterator& iter1, const EnumerableIterator& iter2)
    {
        return !(iter1 == iter2);
    }

    inline EnumerableIterator begin(const EnumerableCollection& collection)
    {
        return EnumerableIterator(collection.GetEnumerator());
    }

    inline EnumerableIterator end(const EnumerableCollection& collection)
    {
        return EnumerableIterator();
    }

} // namespace api
