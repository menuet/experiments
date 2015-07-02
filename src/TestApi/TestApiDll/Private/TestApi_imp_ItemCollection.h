
#pragma once


#include "../Public/TestApi_c.h"
#include "TestApi_imp_Item.h"
#include <vector>
#include <memory>


namespace testapi { namespace imp {

    class ItemEnumerator;

    class ItemCollection
    {
    public:

        void addItem(const testapi_Item& c_item)
        {
            m_items.push_back(Item(c_item.i, c_item.s.in.data, c_item.s.in.length));
        }

        ItemEnumerator getEnumerator() const;

    private:

        std::vector<Item> m_items;
    };

} } // namespace testapi::imp
