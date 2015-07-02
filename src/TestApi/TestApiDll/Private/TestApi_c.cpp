
#include "stdafx.h"
#include "../Public/TestApi_c.h"
#include "TestApi_imp_ItemCollection.h"
#include "TestApi_imp_ItemEnumerator.h"


struct testapi_ItemCollection
{
    testapi::imp::ItemCollection imp;
};

struct testapi_ItemEnumerator
{
    testapi::imp::ItemEnumerator imp;
};


#ifdef __cplusplus
extern "C" {
#endif

    testapi_ItemCollection* testapi_ItemCollection_create()
    {
        return new testapi_ItemCollection;
    }

    void testapi_ItemCollection_destroy(testapi_ItemCollection* collection)
    {
        delete collection;
    }

    void testapi_ItemCollection_addItem(testapi_ItemCollection* collection, const testapi_Item* item)
    {
        assert(collection);
        assert(item);
        collection->imp.addItem(*item);
    }

    testapi_ItemEnumerator* testapi_ItemCollection_getItemEnumerator(testapi_ItemCollection* collection)
    {
        assert(collection);
        return new testapi_ItemEnumerator{ collection->imp.getEnumerator() };
    }

    void testapi_ItemEnumerator_destroy(testapi_ItemEnumerator* enumerator)
    {
        delete enumerator;
    }

    void testapi_ItemEnumerator_getCurrent(testapi_ItemEnumerator* enumerator, testapi_Item* item)
    {
        assert(enumerator);
        assert(item);
        enumerator->imp.getCurrent(*item);
    }

    bool testapi_ItemEnumerator_moveNext(testapi_ItemEnumerator* enumerator)
    {
        assert(enumerator);
        return enumerator->imp.moveNext();
    }

    void testapi_ItemEnumerator_reset(testapi_ItemEnumerator* enumerator)
    {
        assert(enumerator);
        return enumerator->imp.reset();
    }

    testapi_ItemEnumerator* testapi_ItemEnumerator_clone(testapi_ItemEnumerator* enumerator)
    {
        assert(enumerator);
        return new testapi_ItemEnumerator{ enumerator->imp };
    }

    bool testapi_ItemEnumerator_equal(testapi_ItemEnumerator* enumerator1, testapi_ItemEnumerator* enumerator2)
    {
        assert(enumerator1);
        assert(enumerator2);
        return enumerator1->imp.equal(enumerator2->imp);
    }

#ifdef __cplusplus
}
#endif
