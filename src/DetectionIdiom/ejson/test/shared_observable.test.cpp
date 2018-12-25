
#include <platform/platform.h>

#if !(EXP_PLATFORM_CPL_IS_CLANG || EXP_PLATFORM_CPL_IS_CLANG_APPLE)
// Note: clang emits errors about ambiguous calls - too lazy to understand and fix
 
#include "shared_observable.hpp"
#include <catch/catch.hpp>


namespace {

    struct Executor {
        template< typename Task >
        void post(Task&& task) { std::forward<Task>(task)(); }
    };

    struct Observer {
        int count{};
        void onEvent() { ++count; };
    };

    class SharedObservableOnUiThreadModel : public SharedObservableOnUiThread<SharedObservableOnUiThreadModel> {
        friend class SharedObservableOnUiThread<SharedObservableOnUiThreadModel>;
    public:

        void doSomethingThenObserve() {
            // ... do something ...
            observe([](Observer& observer) {
                observer.onEvent();
            });
        }

        void doSomethingThenObserveAndAgain() {
            // ... do something ...
            observe([](const auto& observable, auto& observer) {
                observer.onEvent();
                observable->doSomethingThenObserve();
            });
        }

        auto getCount() const { return mObserver->count; }

    private:
        std::shared_ptr<Executor> mUiLoop{ std::make_shared<Executor>() };
        std::shared_ptr<Observer> mObserver{ std::make_shared<Observer>() };
    };

    class SharedObservableVmModel : public SharedObservableVm<SharedObservableVmModel, Observer, Executor> {
    public:

        SharedObservableVmModel(std::shared_ptr<Executor> uiLoop) : SharedObservableVm(std::move(uiLoop)) {
        }

        void start(std::shared_ptr<Observer> observer) {
            setObserver(std::move(observer));
        }

        void stop() {
            setObserver(nullptr);
        }

        void doSomethingThenObserve() {
            // ... do something ...
            observe([](Observer& observer) {
                observer.onEvent();
            });
        }

        void doSomethingThenObserveAndAgain() {
            // ... do something ...
            observe([](const auto& observable, auto& observer) {
                observer.onEvent();
                observable->doSomethingThenObserve();
            });
        }
    };
}

TEST_CASE("SharedObservableOnUiThread", "") {

    SECTION("doSomethingThenObserve") {
        const auto observable = std::make_shared<SharedObservableOnUiThreadModel>();
        observable->doSomethingThenObserve();
        REQUIRE(observable->getCount() == 1);
    }

    SECTION("doSomethingThenObserveAndAgain") {
        const auto observable = std::make_shared<SharedObservableOnUiThreadModel>();
        observable->doSomethingThenObserveAndAgain();
        REQUIRE(observable->getCount() == 2);
    }

}

TEST_CASE("SharedObservableVm", "") {

    SECTION("doSomethingThenObserve") {
        const auto observable = std::make_shared<SharedObservableVmModel>(std::make_shared<Executor>());
        const auto observer = std::make_shared<Observer>();
        observable->start(observer);
        observable->doSomethingThenObserve();
        REQUIRE(observer->count == 1);
    }

    SECTION("doSomethingThenObserveAndAgain") {
        const auto observable = std::make_shared<SharedObservableVmModel>(std::make_shared<Executor>());
        const auto observer = std::make_shared<Observer>();
        observable->start(observer);
        observable->doSomethingThenObserveAndAgain();
        REQUIRE(observer->count == 2);
    }

}

#endif
