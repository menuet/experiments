
#include "stdafx.h"
#include "TestApi_imp_ItemCollection.h"
#include "TestApi_imp_ItemEnumerator.h"


namespace testapi { namespace imp {

    ItemEnumerator ItemCollection::getEnumerator() const
    {
        return ItemEnumerator(m_items);
    }

} } // namespace testapi::imp
