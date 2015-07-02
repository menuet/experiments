
#include "stdafx.h"
#include "../TestApiDll/Public/TestApi_cpp_ItemCollection.h"
#include "../TestApiDll/Public/TestApi_cpp_ItemIterator.h"
#include <iostream>


int main(int /*argc*/, char* /*argv*/[])
{
    testapi::ItemCollection itemCollection;
    itemCollection.addItem(testapi::Item(98, "bob"));
    itemCollection.addItem(testapi::Item(76, "joe"));
    itemCollection.addItem(testapi::Item(54, "rick"));
    itemCollection.addItem(testapi::Item(32, "jim"));
    itemCollection.addItem(testapi::Item(10, "jack"));
    itemCollection.addItem(testapi::Item(-123, "john"));

    std::cout << "Iterating on the collection:\n";
    for (const auto& item : itemCollection)
    {
        std::cout << "Item: i=" << item.getI() << ", s=" << item.getS() << '\n';
    }

    std::cout << "\n";

    return 0;
}
