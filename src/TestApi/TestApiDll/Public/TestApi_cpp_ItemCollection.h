
#pragma once


#include "TestApi_c.h"
#include "TestApi_cpp_Item.h"
#include <memory>
#include <cassert>


namespace testapi {

    class ItemIterator;

    class ItemCollection
    {
        friend class ItemIterator;

    public:

        ItemCollection()
            : m_spCollection(testapi_ItemCollection_create())
        {
            assert(m_spCollection);
        }

        void addItem(const Item& item)
        {
            assert(m_spCollection);
            testapi_Item c_item{ 0 };
            c_item.i = item.getI();
            c_item.s.in.data = item.getS().c_str();
            c_item.s.in.length = item.getS().length();
            testapi_ItemCollection_addItem(m_spCollection.get(), &c_item);
        }

    private:

        struct CollectionDestroyer
        {
            void operator()(testapi_ItemCollection* pCollection) { testapi_ItemCollection_destroy(pCollection); }
        };

        std::unique_ptr<testapi_ItemCollection, CollectionDestroyer> m_spCollection;
    };

} // namespace testapi
