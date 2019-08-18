
#include <catch2/catch.hpp>
#include "unique_ptr98.h"


namespace ut {

    namespace {

        struct NoopDeleter
        {
            template< typename ResourceT >
            void operator()(ResourceT* a_pResource) const
            {
            }
        };

        bool gs_isOperatorCalled = false;

        struct Deleter
        {
            Deleter(bool& a_isOperatorCalled = gs_isOperatorCalled)
                : m_isOperatorCalled(a_isOperatorCalled)
            {
            }

            template< typename ResourceT >
            void operator()(ResourceT* a_pResource) const
            {
                delete a_pResource;
                m_isOperatorCalled = true;
            }

            Deleter& operator=(const Deleter& a_deleter)
            {
                return *this;
            }

        private:

            bool& m_isOperatorCalled;
        };

        struct Object
        {
            Object(bool& a_isDestructorCalled)
                : m_isDestructorCalled(a_isDestructorCalled)
                , m_isFooCalled(false)
            {
            }

            ~Object()
            {
                m_isDestructorCalled = true;
            }

            void foo()
            {
                m_isFooCalled = true;
            }

            bool m_isFooCalled;

        private:

            Object(const Object&); // = delete
            Object& operator=(const Object&); // = delete

            bool& m_isDestructorCalled;
        };

        std_ex::unique_ptr98<Object, Deleter> makeObject(bool& a_isDestructorCalled)
        {
            std_ex::unique_ptr98<Object, Deleter> l_uniquePtr(new Object(a_isDestructorCalled));
            return std_ex::move98(l_uniquePtr);
        }
    }

    SCENARIO("The default deleter deletes", "[unique_ptr98]")
    {
        GIVEN("A default deleter")
        {
            std_ex::detail::DefaultResourceDeleter l_defaultDeleter;

            WHEN("We call its operator() on a new object")
            {
                bool l_isDestructorCalled = false;
                l_defaultDeleter(new Object(l_isDestructorCalled));

                THEN("The object is deleted")
                {
                    REQUIRE(l_isDestructorCalled);
                }
            }
        }
    }

    SCENARIO("The unique pointer size is restricted", "[unique_ptr98]")
    {
        GIVEN("A unique pointer with default deleter")
        {
            std_ex::unique_ptr98<int> l_uniquePtr;

            THEN("It has the size of a pointer")
            {
                REQUIRE(sizeof(l_uniquePtr) == sizeof(int*));
            }
        }

        GIVEN("A unique pointer with a user-defined deleter")
        {
            std_ex::unique_ptr98<int, NoopDeleter> l_uniquePtr;

            THEN("It has a size greater than the size of a pointer")
            {
                REQUIRE(sizeof(l_uniquePtr) > sizeof(int*));
            }
        }
    }

    SCENARIO("A unique pointer deletes its non-null owned pointer at destruction or reset", "[unique_ptr98]")
    {
        GIVEN("A default-constructed unique pointer with special deleter")
        {
            gs_isOperatorCalled = false;
            std_ex::unique_ptr98<Object, Deleter>* l_pUniquePtr = new std_ex::unique_ptr98<Object, Deleter>();

            THEN("the deleter's operator() is not called")
            {
                REQUIRE(!gs_isOperatorCalled);

                AND_WHEN("We destroy the unique pointer")
                {
                    gs_isOperatorCalled = false;
                    delete l_pUniquePtr;

                    THEN("The operator() of the deleter is not called")
                    {
                        REQUIRE(!gs_isOperatorCalled);
                    }
                }

                AND_WHEN("We call reset")
                {
                    gs_isOperatorCalled = false;
                    l_pUniquePtr->reset();

                    THEN("The operator() of the deleter is not called")
                    {
                        REQUIRE(!gs_isOperatorCalled);

                        AND_WHEN("We destroy the unique pointer")
                        {
                            gs_isOperatorCalled = false;
                            delete l_pUniquePtr;

                            THEN("The operator() of the deleter is not called")
                            {
                                REQUIRE(!gs_isOperatorCalled);
                            }
                        }
                    }
                }

                AND_WHEN("We call reset with an new object")
                {
                    bool l_isDestructorCalled = false;
                    Object* l_pObject = new Object(l_isDestructorCalled);

                    gs_isOperatorCalled = false;
                    l_pUniquePtr->reset(l_pObject);

                    THEN("The operator() of the deleter is not called")
                    {
                        REQUIRE(!gs_isOperatorCalled);

                        AND_WHEN("We destroy the unique pointer")
                        {
                            delete l_pUniquePtr;

                            THEN("The operator() of the deleter is called and the object is destroyed")
                            {
                                REQUIRE(gs_isOperatorCalled);
                                REQUIRE(l_isDestructorCalled);
                            }
                        }
                    }
                }

                AND_WHEN("We swap with another default-constructed unique pointer")
                {
                    gs_isOperatorCalled = false;
                    std_ex::unique_ptr98<Object, Deleter>* l_pUniquePtr2 = new std_ex::unique_ptr98<Object, Deleter>();
                    REQUIRE(!gs_isOperatorCalled);

                    l_pUniquePtr2->swap(*l_pUniquePtr);

                    THEN("The deleters' operator() are not called")
                    {
                        REQUIRE(!gs_isOperatorCalled);

                        AND_WHEN("We destroy the first unique pointer")
                        {
                            delete l_pUniquePtr;

                            THEN("The deleters'operator() is not called")
                            {
                                REQUIRE(!gs_isOperatorCalled);

                                AND_WHEN("We destroy the second unique pointer")
                                {
                                    delete l_pUniquePtr2;

                                    THEN("The deleters'operator() is not called")
                                    {
                                        REQUIRE(!gs_isOperatorCalled);
                                    }
                                }
                            }
                        }
                    }
                }

                AND_WHEN("We swap with another unique pointer constructed from a new object")
                {
                    bool l_isDestructorCalled2 = false;
                    Object* l_pObject2 = new Object(l_isDestructorCalled2);
                    gs_isOperatorCalled = false;
                    std_ex::unique_ptr98<Object, Deleter>* l_pUniquePtr2 = new std_ex::unique_ptr98<Object, Deleter>(l_pObject2);
                    REQUIRE(!gs_isOperatorCalled);

                    l_pUniquePtr2->swap(*l_pUniquePtr);

                    THEN("The deleters' operator() are not called")
                    {
                        REQUIRE(!gs_isOperatorCalled);

                        AND_WHEN("We destroy the first unique pointer")
                        {
                            delete l_pUniquePtr;

                            THEN("The deleters'operator() is called and the object is destroyed")
                            {
                                REQUIRE(gs_isOperatorCalled);
                                gs_isOperatorCalled = false;
                                REQUIRE(l_isDestructorCalled2);
                                l_isDestructorCalled2 = false;

                                AND_WHEN("We destroy the second unique pointer")
                                {
                                    delete l_pUniquePtr2;

                                    THEN("The deleters'operator() is not called")
                                    {
                                        REQUIRE(!gs_isOperatorCalled);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        GIVEN("A unique pointer with special deleter constructed from a new object")
        {
            bool l_isDestructorCalled = false;
            Object* l_pObject = new Object(l_isDestructorCalled);
            gs_isOperatorCalled = false;
            std_ex::unique_ptr98<Object, Deleter>* l_pUniquePtr = new std_ex::unique_ptr98<Object, Deleter>(l_pObject);

            THEN("the deleter's operator() is not called and the object is not destroyed")
            {
                REQUIRE(!gs_isOperatorCalled);
                REQUIRE(!l_isDestructorCalled);

                AND_WHEN("We destroy the unique pointer")
                {
                    gs_isOperatorCalled = false;
                    delete l_pUniquePtr;

                    THEN("The deleter's operator() is called and the object is destroyed")
                    {
                        REQUIRE(gs_isOperatorCalled);
                        REQUIRE(l_isDestructorCalled);
                    }
                }

                AND_WHEN("We call reset")
                {
                    gs_isOperatorCalled = false;
                    l_pUniquePtr->reset();

                    THEN("The deleter's operator() is called and the object is destroyed")
                    {
                        REQUIRE(gs_isOperatorCalled);
                        REQUIRE(l_isDestructorCalled);

                        AND_WHEN("We destroy the unique pointer")
                        {
                            gs_isOperatorCalled = false;
                            delete l_pUniquePtr;

                            THEN("The deleter's operator() is not called")
                            {
                                REQUIRE(!gs_isOperatorCalled);
                            }
                        }
                    }
                }

                AND_WHEN("We call reset with another new object")
                {
                    bool l_isDestructorCalled2 = false;
                    Object* l_pObject2 = new Object(l_isDestructorCalled2);
                    gs_isOperatorCalled = false;
                    l_pUniquePtr->reset(l_pObject2);

                    THEN("The deleter's operator() is called and the first object is destroyed")
                    {
                        REQUIRE(gs_isOperatorCalled);
                        REQUIRE(l_isDestructorCalled);

                        AND_WHEN("We destroy the unique pointer")
                        {
                            gs_isOperatorCalled = false;
                            delete l_pUniquePtr;

                            THEN("The deleter's operator() is called and the second object is destroyed")
                            {
                                REQUIRE(gs_isOperatorCalled);
                                REQUIRE(l_isDestructorCalled2);
                            }
                        }
                    }
                }

                AND_WHEN("We swap with a default-constructed unique pointer")
                {
                    gs_isOperatorCalled = false;
                    std_ex::unique_ptr98<Object, Deleter>* l_pUniquePtr2 = new std_ex::unique_ptr98<Object, Deleter>();
                    REQUIRE(!gs_isOperatorCalled);

                    l_pUniquePtr2->swap(*l_pUniquePtr);

                    THEN("The deleters' operator() are not called")
                    {
                        REQUIRE(!gs_isOperatorCalled);

                        AND_WHEN("We destroy the first unique pointer")
                        {
                            delete l_pUniquePtr;

                            THEN("The deleters'operator() is not called")
                            {
                                REQUIRE(!gs_isOperatorCalled);

                                AND_WHEN("We destroy the second unique pointer")
                                {
                                    delete l_pUniquePtr2;

                                    THEN("The deleters'operator() is not called")
                                    {
                                        REQUIRE(gs_isOperatorCalled);
                                        REQUIRE(l_isDestructorCalled);
                                    }
                                }
                            }
                        }
                    }
                }

                AND_WHEN("We swap with another unique pointer constructed from a new object")
                {
                    bool l_isDestructorCalled2 = false;
                    Object* l_pObject2 = new Object(l_isDestructorCalled2);
                    gs_isOperatorCalled = false;
                    std_ex::unique_ptr98<Object, Deleter>* l_pUniquePtr2 = new std_ex::unique_ptr98<Object, Deleter>(l_pObject2);
                    REQUIRE(!gs_isOperatorCalled);

                    l_pUniquePtr2->swap(*l_pUniquePtr);

                    THEN("The deleters' operator() are not called")
                    {
                        REQUIRE(!gs_isOperatorCalled);

                        AND_WHEN("We destroy the first unique pointer")
                        {
                            delete l_pUniquePtr;

                            THEN("The deleters'operator() is called and the second object is destroyed")
                            {
                                REQUIRE(gs_isOperatorCalled);
                                gs_isOperatorCalled = false;
                                REQUIRE(l_isDestructorCalled2);
                                l_isDestructorCalled2 = false;

                                AND_WHEN("We destroy the second unique pointer")
                                {
                                    delete l_pUniquePtr2;

                                    THEN("The deleters'operator() is called and the first object is destroyed")
                                    {
                                        REQUIRE(gs_isOperatorCalled);
                                        REQUIRE(l_isDestructorCalled);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    SCENARIO("Unique pointers can move", "[unique_ptr98]")
    {
        GIVEN("A default-constructed unique pointer")
        {
            std_ex::unique_ptr98<Object, Deleter>* l_pUniquePtr1 = new std_ex::unique_ptr98<Object, Deleter>();

            WHEN("We move from another default-constructed unique pointer")
            {
                std_ex::unique_ptr98<Object, Deleter>* l_pUniquePtr2 = new std_ex::unique_ptr98<Object, Deleter>();

                gs_isOperatorCalled = false;
                *l_pUniquePtr1 = std_ex::move98(*l_pUniquePtr2);

                THEN("the deleters' operator() are not called and the owned pointers are null")
                {
                    REQUIRE(!gs_isOperatorCalled);
                    REQUIRE(!l_pUniquePtr1->get());
                    REQUIRE(!l_pUniquePtr2->get());

                    gs_isOperatorCalled = false;

                    AND_WHEN("We destroy the first unique")
                    {
                        delete l_pUniquePtr1;

                        THEN("the deleter's operator() is not called")
                        {
                            REQUIRE(!gs_isOperatorCalled);

                            AND_WHEN("We destroy the second unique")
                            {
                                delete l_pUniquePtr2;

                                THEN("the deleter's operator() is not called")
                                {
                                    REQUIRE(!gs_isOperatorCalled);
                                }
                            }
                        }
                    }
                }
            }

            WHEN("We move from another unique pointer constructed from a new object")
            {
                bool l_isDestructorCalled2 = false;
                Object* l_pObject2 = new Object(l_isDestructorCalled2);
                std_ex::unique_ptr98<Object, Deleter>* l_pUniquePtr2 = new std_ex::unique_ptr98<Object, Deleter>(l_pObject2);

                gs_isOperatorCalled = false;
                *l_pUniquePtr1 = std_ex::move98(*l_pUniquePtr2);

                THEN("the deleters' operator() are not called and the owned pointer is moved")
                {
                    REQUIRE(!gs_isOperatorCalled);
                    REQUIRE(l_pUniquePtr1->get() == l_pObject2);
                    REQUIRE(!l_pUniquePtr2->get());

                    gs_isOperatorCalled = false;

                    AND_WHEN("We destroy the first unique")
                    {
                        delete l_pUniquePtr1;

                        THEN("the deleter's operator() is called and the object is destroyed")
                        {
                            REQUIRE(gs_isOperatorCalled);
                            REQUIRE(l_isDestructorCalled2);

                            AND_WHEN("We destroy the second unique")
                            {
                                gs_isOperatorCalled = false;
                                delete l_pUniquePtr2;

                                THEN("the deleter's operator() is not called")
                                {
                                    REQUIRE(!gs_isOperatorCalled);
                                }
                            }
                        }
                    }
                }
            }

            WHEN("We assign from a function-created unique pointer")
            {
                bool l_isDestructorCalled2 = false;
                std_ex::unique_ptr98<Object, Deleter>* l_pUniquePtr2 = new std_ex::unique_ptr98<Object, Deleter>(makeObject(l_isDestructorCalled2));
                Object* l_pObject2 = l_pUniquePtr2->get();

                gs_isOperatorCalled = false;
                *l_pUniquePtr1 = std_ex::move98(*l_pUniquePtr2);

                THEN("the deleters' operator() are not called and the owned pointer is moved")
                {
                    REQUIRE(!gs_isOperatorCalled);
                    REQUIRE(l_pUniquePtr1->get() == l_pObject2);
                    REQUIRE(!l_pUniquePtr2->get());

                    gs_isOperatorCalled = false;

                    AND_WHEN("We destroy the first unique")
                    {
                        delete l_pUniquePtr1;

                        THEN("the deleter's operator() is called and the object is destroyed")
                        {
                            REQUIRE(gs_isOperatorCalled);
                            REQUIRE(l_isDestructorCalled2);

                            AND_WHEN("We destroy the second unique")
                            {
                                gs_isOperatorCalled = false;
                                delete l_pUniquePtr2;

                                THEN("the deleter's operator() is not called")
                                {
                                    REQUIRE(!gs_isOperatorCalled);
                                }
                            }
                        }
                    }
                }
            }
        }

        GIVEN("A unique pointer constructed from a new object")
        {
            bool l_isDestructorCalled1 = false;
            Object* l_pObject1 = new Object(l_isDestructorCalled1);
            std_ex::unique_ptr98<Object, Deleter>* l_pUniquePtr1 = new std_ex::unique_ptr98<Object, Deleter>(l_pObject1);

            WHEN("We move from another default-constructed unique pointer")
            {
                std_ex::unique_ptr98<Object, Deleter>* l_pUniquePtr2 = new std_ex::unique_ptr98<Object, Deleter>();

                gs_isOperatorCalled = false;
                *l_pUniquePtr1 = std_ex::move98(*l_pUniquePtr2);

                THEN("the deleter's operator() is called and object is deleted and the owned pointers are null")
                {
                    REQUIRE(l_isDestructorCalled1);
                    REQUIRE(gs_isOperatorCalled);
                    REQUIRE(!l_pUniquePtr1->get());
                    REQUIRE(!l_pUniquePtr2->get());

                    AND_WHEN("We destroy the first unique")
                    {
                        gs_isOperatorCalled = false;
                        delete l_pUniquePtr1;

                        THEN("the deleter's operator() is not called")
                        {
                            REQUIRE(!gs_isOperatorCalled);

                            AND_WHEN("We destroy the second unique")
                            {
                                delete l_pUniquePtr2;

                                THEN("the deleter's operator() is not called")
                                {
                                    REQUIRE(!gs_isOperatorCalled);
                                }
                            }
                        }
                    }
                }
            }

            WHEN("We move from another unique pointer constructed from a new object")
            {
                bool l_isDestructorCalled2 = false;
                Object* l_pObject2 = new Object(l_isDestructorCalled2);
                std_ex::unique_ptr98<Object, Deleter>* l_pUniquePtr2 = new std_ex::unique_ptr98<Object, Deleter>(l_pObject2);

                gs_isOperatorCalled = false;
                *l_pUniquePtr1 = std_ex::move98(*l_pUniquePtr2);

                THEN("the deleter's operator() is called and the first object is delete and the owned pointer is moved")
                {
                    REQUIRE(l_isDestructorCalled1);
                    REQUIRE(gs_isOperatorCalled);
                    REQUIRE(l_pUniquePtr1->get() == l_pObject2);
                    REQUIRE(!l_pUniquePtr2->get());

                    AND_WHEN("We destroy the first unique")
                    {
                        gs_isOperatorCalled = false;
                        delete l_pUniquePtr1;

                        THEN("the deleter's operator() is called and the object is destroyed")
                        {
                            REQUIRE(gs_isOperatorCalled);
                            REQUIRE(l_isDestructorCalled2);

                            AND_WHEN("We destroy the second unique")
                            {
                                gs_isOperatorCalled = false;
                                delete l_pUniquePtr2;

                                THEN("the deleter's operator() is not called")
                                {
                                    REQUIRE(!gs_isOperatorCalled);
                                }
                            }
                        }
                    }
                }
            }

            WHEN("We assign from a function-created unique pointer")
            {
                bool l_isDestructorCalled2 = false;
                std_ex::unique_ptr98<Object, Deleter>* l_pUniquePtr2 = new std_ex::unique_ptr98<Object, Deleter>();
                *l_pUniquePtr2 = makeObject(l_isDestructorCalled2);
                Object* l_pObject2 = l_pUniquePtr2->get();

                gs_isOperatorCalled = false;
                *l_pUniquePtr1 = std_ex::move98(*l_pUniquePtr2);

                THEN("the deleter's operator() is called and the first object is delete and the owned pointer is moved")
                {
                    REQUIRE(l_isDestructorCalled1);
                    REQUIRE(gs_isOperatorCalled);
                    REQUIRE(l_pUniquePtr1->get() == l_pObject2);
                    REQUIRE(!l_pUniquePtr2->get());

                    AND_WHEN("We destroy the first unique")
                    {
                        gs_isOperatorCalled = false;
                        delete l_pUniquePtr1;

                        THEN("the deleter's operator() is called and the object is destroyed")
                        {
                            REQUIRE(gs_isOperatorCalled);
                            REQUIRE(l_isDestructorCalled2);

                            AND_WHEN("We destroy the second unique")
                            {
                                gs_isOperatorCalled = false;
                                delete l_pUniquePtr2;

                                THEN("the deleter's operator() is not called")
                                {
                                    REQUIRE(!gs_isOperatorCalled);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    SCENARIO("Unique pointers have accessors", "[unique_ptr98]")
    {
        GIVEN("A default-constructed unique pointer")
        {
            std_ex::unique_ptr98<Object> l_uniquePtr;

            WHEN("We get its owned pointer it is null")
            {
                REQUIRE(!l_uniquePtr.get());
            }

            WHEN("We convert to bool it is false")
            {
                REQUIRE(!l_uniquePtr);
            }
        }

        GIVEN("A unique pointer constructed from a new object")
        {
            bool l_isDestructorCalled = false;
            Object* l_pObject = new Object(l_isDestructorCalled);
            std_ex::unique_ptr98<Object> l_uniquePtr(l_pObject);

            WHEN("We get its owned pointer it points to the new object")
            {
                REQUIRE(l_uniquePtr.get() == l_pObject);
            }

            WHEN("We convert to bool it is true")
            {
                REQUIRE(l_uniquePtr);
            }

            WHEN("We call operator* it gives a reference to the new object")
            {
                REQUIRE(&*l_uniquePtr == l_pObject);
            }

            WHEN("We call operator-> it forwrds to the new object")
            {
                REQUIRE(!l_pObject->m_isFooCalled);
                l_uniquePtr->foo();
                REQUIRE(l_pObject->m_isFooCalled);
            }
        }
    }

}
