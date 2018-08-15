
#include <catch.hpp>

TEST_CASE("lambda expressions", "[]")
{
    int somedata = 1234;

    // THIS
    auto myClosure = [somedata]()
    {
        return somedata * somedata;
    };

    // IS EQUIVALENT TO THIS
    class HiddenCompilerGeneratedFunctorClass
    {
    public:
        HiddenCompilerGeneratedFunctorClass(int somedata)
            : somedata{ somedata }
        {
        }

        int operator()() const
        {
            return somedata * somedata;
        }
    private:
        int somedata{};
    };
    HiddenCompilerGeneratedFunctorClass myFunctor(somedata);

    // WE CAN CALL BOTH
    int resultFromHandMadeFunctor = myFunctor();
    int resultFromLambda = myClosure();

    // AND GET SAME RESULT
    REQUIRE(resultFromHandMadeFunctor == resultFromLambda);
}
