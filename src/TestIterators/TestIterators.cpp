
#include "IndexedCollection.h"
#include "IndexedIterator.h"
#include "EnumerableCollection.h"
#include "EnumerableIterator.h"
#include <iostream>


int main(int argc, char* argv[])
{
    api::IndexedCollection indexedCollection;
    indexedCollection.addItem(api::Item(12, "one"));
    indexedCollection.addItem(api::Item(34, "two"));
    indexedCollection.addItem(api::Item(56, "three"));
    indexedCollection.addItem(api::Item(78, "four"));
    indexedCollection.addItem(api::Item(91, "five"));
    indexedCollection.addItem(api::Item(12, "six"));

    std::cout << "Iterating on an indexed collection:\n";
    for (const auto& item : indexedCollection)
    {
        std::cout << "Item: i=" << item.getI() << ", s=" << item.getS() << '\n';
    }

    std::cout << "\n";

    api::EnumerableCollection enumerableCollection;
    enumerableCollection.addItem(api::Item(98, "bob"));
    enumerableCollection.addItem(api::Item(76, "joe"));
    enumerableCollection.addItem(api::Item(54, "rick"));
    enumerableCollection.addItem(api::Item(32, "jim"));
    enumerableCollection.addItem(api::Item(10, "jack"));
    enumerableCollection.addItem(api::Item(-123, "john"));

    std::cout << "Iterating on an .NET-IEnumerable-like collection:\n";
    for (const auto& item : enumerableCollection)
    {
        std::cout << "Item: i=" << item.getI() << ", s=" << item.getS() << '\n';
    }

    std::cout << "\n";

    return 0;
}
