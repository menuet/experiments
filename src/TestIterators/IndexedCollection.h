
#pragma once


#include "Item.h"
#include <vector>


namespace api {

    class IndexedCollection
    {
    public:

        void addItem(const Item& item)
        {
            m_items.push_back(item);
        }

        size_t getItemCount() const
        {
            return m_items.size();
        }

        const Item& getItem(size_t index) const
        {
            return m_items[index];
        }

    private:

        std::vector<Item> m_items;
    };

} // namespace api
