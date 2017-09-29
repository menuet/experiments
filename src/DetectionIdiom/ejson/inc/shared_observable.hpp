
#pragma once


#include <memory>
#include <utility>
#include <type_traits>


template< typename Observable >
inline const auto& getObserver(const Observable& observable) {
    return observable->getObserver();
}

template< typename Observable >
inline const auto& getUiThreadExecutor(const Observable& observable) {
    return observable->getUiThreadExecutor();
}

template<typename Function, typename Observable, typename Observer>
inline auto callFunction(Function&& function, Observable&& observable, Observer&& observer)
    -> decltype(std::forward<Function>(function)(std::forward<Observable>(observable), std::forward<Observer>(observer)))
{
    std::forward<Function>(function)(std::forward<Observable>(observable), std::forward<Observer>(observer));
}

template<typename Function, typename Observable, typename Observer>
inline auto callFunction(Function&& function, Observable&& observable, Observer&& observer)
    -> decltype(std::forward<Function>(function)(std::forward<Observer>(observer)))
{
    std::forward<Function>(function)(std::forward<Observer>(observer));
}

template< typename Executor, typename Observable, typename Function >
inline void observeOnSomeExecutor(Executor& executor, std::shared_ptr<Observable> sharedObservable, Function&& function) {
    executor.post([sharedObservable = std::move(sharedObservable), function = std::forward<Function>(function)]() {
        const auto& observer = getObserver(sharedObservable);
        if (observer)
            callFunction(function, sharedObservable, *observer);
    });
}

template< typename ObservablePtr, typename Function >
inline void observeOnUiThread(ObservablePtr&& observablePtr, Function&& function) {
    const auto& uiThreadExecutor = getUiThreadExecutor(std::forward<ObservablePtr>(observablePtr));
    observeOnSomeExecutor(*uiThreadExecutor, std::forward<ObservablePtr>(observablePtr), std::forward<Function>(function));
}

template< typename Derived >
class SharedObservableOnUiThread : public std::enable_shared_from_this<Derived> {
protected:

    template< typename Observable >
    friend const auto& getObserver(const Observable& observable);

    template< typename Observable >
    friend const auto& getUiThreadExecutor(const Observable& observable);

    const auto& getObserver() const {
        return static_cast<const Derived*>(this)->mObserver;
    }

    const auto& getUiThreadExecutor() const {
        return static_cast<const Derived*>(this)->mUiLoop;
    }

    template< typename Function >
    void observe(Function&& function) {
        observeOnUiThread(this->shared_from_this(), std::forward<Function>(function));
    }
};

template< typename DerivedVm, typename Observer, typename UiThreadExecutor >
class SharedObservableVm : public std::enable_shared_from_this<DerivedVm> {
protected:

    SharedObservableVm(std::shared_ptr<UiThreadExecutor> uiThreadExecutor)
        : mUiThreadExecutor{std::move(uiThreadExecutor)} {
    }

    void setObserver(std::shared_ptr<Observer> observer) {
        mObserver = std::move(observer);
    }

    template< typename Function >
    void observe(Function&& function) {
        observeOnUiThread(this->shared_from_this(), std::forward<Function>(function));
    }

private:

    template< typename Observable >
    friend const auto& getObserver(const Observable& observable);

    template< typename Observable >
    friend const auto& getUiThreadExecutor(const Observable& observable);

    const auto& getObserver() const {
        return mObserver;
    }

    const auto& getUiThreadExecutor() const {
        return mUiThreadExecutor;
    }

    std::shared_ptr<UiThreadExecutor> mUiThreadExecutor {};
    std::shared_ptr<Observer> mObserver {};
};
