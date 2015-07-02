
#pragma once


#include <stdint.h>


#ifdef TESTAPI_BUILDING_DLL
#define TESTAPI_IMPORTEXPORT __declspec(dllexport)
#else
#define TESTAPI_IMPORTEXPORT __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C" {
#endif

    typedef struct testapi_String_in
    {
        const char* data;
        uint32_t length;
    } testapi_String_in;

    typedef struct testapi_String_out
    {
        void(*setter)(void* object, const char* data, uint32_t length);
        void* object;
    } testapi_String_out;

    typedef struct testapi_String
    {
        testapi_String_in in;
        testapi_String_out out;
    } testapi_String;

    typedef struct testapi_ItemCollection testapi_ItemCollection;

    typedef struct testapi_Item
    {
        int32_t i;
        testapi_String s;
    } testapi_Item;

    typedef struct testapi_ItemEnumerator testapi_ItemEnumerator;

    TESTAPI_IMPORTEXPORT testapi_ItemCollection* testapi_ItemCollection_create();

    TESTAPI_IMPORTEXPORT void testapi_ItemCollection_destroy(testapi_ItemCollection* collection);

    TESTAPI_IMPORTEXPORT void testapi_ItemCollection_addItem(testapi_ItemCollection* collection, const testapi_Item* item);

    TESTAPI_IMPORTEXPORT testapi_ItemEnumerator* testapi_ItemCollection_getItemEnumerator(testapi_ItemCollection* collection);

    TESTAPI_IMPORTEXPORT void testapi_ItemEnumerator_destroy(testapi_ItemEnumerator* enumerator);

    TESTAPI_IMPORTEXPORT void testapi_ItemEnumerator_getCurrent(testapi_ItemEnumerator* enumerator, testapi_Item* item);

    TESTAPI_IMPORTEXPORT bool testapi_ItemEnumerator_moveNext(testapi_ItemEnumerator* enumerator);

    TESTAPI_IMPORTEXPORT void testapi_ItemEnumerator_reset(testapi_ItemEnumerator* enumerator);

    TESTAPI_IMPORTEXPORT testapi_ItemEnumerator* testapi_ItemEnumerator_clone(testapi_ItemEnumerator* enumerator);

    TESTAPI_IMPORTEXPORT bool testapi_ItemEnumerator_equal(testapi_ItemEnumerator* enumerator1, testapi_ItemEnumerator* enumerator2);

#ifdef __cplusplus
}
#endif
