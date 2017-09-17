
project(test_iterators)

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

add_test(test_iterators "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_iterators")
