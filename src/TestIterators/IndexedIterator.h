
#pragma once


#include "IndexedCollection.h"
#include <iterator>
#include <cassert>


namespace api {

    class IndexedIterator : public std::iterator<std::forward_iterator_tag, Item>
    {
    public:

        IndexedIterator() = default;

        IndexedIterator(const IndexedIterator&) = default;

        IndexedIterator(const IndexedCollection& collection, size_t index)
            : m_pCollection(&collection)
            , m_index(index)
        {
        }

        IndexedIterator& operator=(const IndexedIterator&) = default;

        void swap(IndexedIterator& iterator)
        {
            std::swap(m_pCollection, iterator.m_pCollection);
            std::swap(m_index, iterator.m_index);
        }

        friend bool operator==(const IndexedIterator& iter1, const IndexedIterator& iter2);

        const Item& operator*() const
        {
            assert(m_pCollection);
            assert(m_index < m_pCollection->getItemCount());
            return m_pCollection->getItem(m_index);
        }

        const Item* operator->() const
        {
            assert(m_pCollection);
            assert(m_index < m_pCollection->getItemCount());
            return &m_pCollection->getItem(m_index);
        }

        IndexedIterator& operator++()
        {
            assert(m_pCollection);
            assert(m_index < m_pCollection->getItemCount());
            ++m_index;
            return *this;
        }

        IndexedIterator operator++(int)
        {
            IndexedIterator iter(*this);
            assert(m_pCollection);
            assert(m_index < m_pCollection->getItemCount());
            ++m_index;
            return iter;
        }

    private:

        const IndexedCollection* m_pCollection = nullptr;
        size_t m_index = 0;
    };

    inline bool operator==(const IndexedIterator& iter1, const IndexedIterator& iter2)
    {
        return iter1.m_pCollection == iter2.m_pCollection && iter1.m_index == iter2.m_index;
    }

    inline bool operator!=(const IndexedIterator& iter1, const IndexedIterator& iter2)
    {
        return !(iter1 == iter2);
    }

    inline IndexedIterator begin(const IndexedCollection& collection)
    {
        return IndexedIterator(collection, 0);
    }

    inline IndexedIterator end(const IndexedCollection& collection)
    {
        return IndexedIterator(collection, collection.getItemCount());
    }

} // namespace api
