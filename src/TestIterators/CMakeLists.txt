
add_executable(test_iterators
    Enumerable.h
    EnumerableCollection.h
    EnumerableIterator.h
    IndexedCollection.h
    IndexedIterator.h
    Item.h
    TestIterators.cpp
    )
exp_setup_common_options(test_iterators)
target_link_libraries(test_iterators PRIVATE platform)

add_test(NAME test_iterators COMMAND test_iterators)
