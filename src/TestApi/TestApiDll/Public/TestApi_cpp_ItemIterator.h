
#pragma once


#include "TestApi_c.h"
#include "TestApi_cpp_Item.h"
#include "TestApi_cpp_ItemCollection.h"
#include <memory>
#include <iterator>
#include <cassert>


namespace testapi {

    class ItemIterator : public std::iterator<std::forward_iterator_tag, Item>
    {
    public:

        ItemIterator() = default;

        ItemIterator(const ItemIterator& iterator)
            : m_spEnumerator(iterator.m_spEnumerator ? testapi_ItemEnumerator_clone(m_spEnumerator.get()) : nullptr)
        {
        }

        ItemIterator(const ItemCollection& collection)
            : m_spEnumerator(testapi_ItemCollection_getItemEnumerator(collection.m_spCollection.get()))
        {
            moveNext();
        }

        ItemIterator& operator=(const ItemIterator& iterator)
        {
            ItemIterator iter(iterator);
            iter.swap(*this);
            return *this;
        }

        void swap(ItemIterator& iterator)
        {
            using std::swap;
            swap(m_spEnumerator, iterator.m_spEnumerator);
            swap(m_item, iterator.m_item);
        }

        friend bool operator==(const ItemIterator& iter1, const ItemIterator& iter2);

        const Item& operator*() const
        {
            assert(m_spEnumerator);
            return m_item;
        }

        const Item* operator->() const
        {
            assert(m_spEnumerator);
            return &m_item;
        }

        ItemIterator& operator++()
        {
            moveNext();
            return *this;
        }

        ItemIterator operator++(int)
        {
            ItemIterator iter(*this);
            moveNext();
            return iter;
        }

    private:

        void moveNext()
        {
            assert(m_spEnumerator);
            if (!testapi_ItemEnumerator_moveNext(m_spEnumerator.get()))
                m_spEnumerator.reset();
            else
            {
                testapi_Item c_item{ 0 };
                c_item.s.out.object = &m_item.m_s;
                c_item.s.out.setter = &string_setter;
                testapi_ItemEnumerator_getCurrent(m_spEnumerator.get(), &c_item);
                m_item.m_i = c_item.i;
            }
        }

        static void string_setter(void* object, const char* data, size_t length)
        {
            assert(object);
            std::string& s = *static_cast<std::string*>(object);
            s.assign(data, length);
        }

        struct EnumeratorDestroyer
        {
            void operator()(testapi_ItemEnumerator* pEnumerator) { testapi_ItemEnumerator_destroy(pEnumerator); }
        };

        std::unique_ptr<testapi_ItemEnumerator, EnumeratorDestroyer> m_spEnumerator;
        Item m_item;
    };

    inline bool operator==(const ItemIterator& iter1, const ItemIterator& iter2)
    {
        if (iter1.m_spEnumerator == iter2.m_spEnumerator)
            return true;
        if (iter1.m_spEnumerator == nullptr || iter2.m_spEnumerator == nullptr)
            return false;
        return testapi_ItemEnumerator_equal(iter1.m_spEnumerator.get(), iter2.m_spEnumerator.get());
    }

    inline bool operator!=(const ItemIterator& iter1, const ItemIterator& iter2)
    {
        return !(iter1 == iter2);
    }

    inline ItemIterator begin(const ItemCollection& collection)
    {
        return ItemIterator(collection);
    }

    inline ItemIterator end(const ItemCollection& /*collection*/)
    {
        return ItemIterator();
    }

} // namespace testapi
