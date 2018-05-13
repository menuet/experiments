
#include "string.hpp"
#include <catch.hpp>

#if 0
template <typename T>
class shared_ptr
{
    public:
        T* operator->() const
        {
            return m_p;
        }

    private:
        T* m_p;
};

template <typename T>
class weak_ptr
{
    public:
        shared_ptr<T> lock() const
        {
            if (exist encore)
                return shared_ptr<m_p>;
            else
                return nullptr;
        }

    private:
        T* m_p;
};

class Parent
{
public:

    shared_ptr<Child> m_spChild;
};

class Child
{
public:

    weak_ptr<Parent> m_wpParent;
};

std::vector<int> toto()
{
    return std::vector<int>{ 1, 2, 3 };
}

template <typename T>
class vector
{
    public:
        vector(const vector& v)
        {
            m_size = v.m_size;
            m_data = new T[v.m_size];
            copy(m_data, v.m_data, m_size);
        }

        vector(vector&& v)
        {
            m_size = v.m_size;
            m_data = v.m_data;
            v.m_data = nullptr;
        }

        vector& operator=(const vector& v)
        {
            if (&v != this)
            {
                delete m_data;

                m_size = v.m_size;
                m_data = new T[v.m_size];
                copy(m_data, v.m_data, m_size);
            }
            return *this;
        }

        vector& operator=(vector&& v)
        {
            delete m_data;
            m_size = v.m_size;
            m_data = v.m_data;
            v.m_data = nullptr;
        }

    constexpr size_t size() const
    {
        return Size;
    }

private:
    T* m_data;
    size m_size;
};

void test()
{
    std::vector<int> v1 { 3, 4 };
    std::vector<int> v2 = v1;
    v2 = v1;

    std::swap(v1, v2);

//    v2 = toto();
    std::vector<int> v3 = toto();

    int a = 1;
    int* p2 = &a;
    *p2 = 2; // a == 2
    shared_ptr<Child> spChild = make_shared<Child>(); // strong ref count child = 1
    shared_ptr<Parent> spParent = make_shared<Parent>(); // strong ref count parent = 1
    spChild->m_wpParent = spParent; // strong ref count parent = 1, weak ref count parent = 1
    spParent->m_spChild = spChild; // strong ref count child = 2
    shared_ptr<Parent> c = spChild->m_wpParent.lock();
}

template <typename T, size_t Size>
class Array
{
public:

    constexpr size_t size() const
    {
        return Size;
    }

    operator[]...;

private:
    T m_arr[Size];
};

template <typename Container>
void test1(const Container& c)
{
    const auto n = c.size();
    std::cout << n;
}

void test2()
{
    test1<Array<int, 5>>();
}
#endif

static String create_string(const char* str)
{
    return String(str);
}

TEST_CASE("String", "[]")
{
    SECTION("Constructor")
    {
        SECTION("Default")
        {
            #if 0
            String s;
            REQUIRE(std::strcmp(s.c_str(), "") == 0);
            REQUIRE(s.length() == 0);
            #endif
        }

        SECTION("const char*")
        {
            String s("hello");
            REQUIRE(std::strcmp(s.c_str(), "hello") == 0);
            REQUIRE(s.length() == std::strlen("hello"));
        }

        SECTION("copy")
        {
            String s0("hello");
            String s(s0);
            REQUIRE(std::strcmp(s.c_str(), "hello") == 0);
            REQUIRE(s.length() == std::strlen("hello"));
        }

        SECTION("from function")
        {
            String s(create_string("hello"));
            REQUIRE(std::strcmp(s.c_str(), "hello") == 0);
            REQUIRE(s.length() == std::strlen("hello"));
        }
    }

    SECTION("Assignment")
    {
        SECTION("copy")
        {
            #if 0
            String s0("hello");
            String s;
            s = s0;
            REQUIRE(std::strcmp(s.c_str(), "hello") == 0);
            REQUIRE(s.length() == std::strlen("hello"));
            #endif
        }

        SECTION("from function")
        {
            #if 0
            String s;
            s = create_string("hello");
            REQUIRE(std::strcmp(s.c_str(), "hello") == 0);
            REQUIRE(s.length() == std::strlen("hello"));
            #endif
        }
    }
}

// Are there some compilation errors?
// If yes, how can we fix them?
// Are there some bugs?
// If yes, how can we fix them?
// How can we optimize the construction from a function's result?
// How can we optimize the assignment from a function's result?
