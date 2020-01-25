/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdexcept>
#include <type_traits>
#include <functional>

#if __has_include(<concepts>)
#include <concepts>
#else
namespace std {
  // C++20 concepts
  template<class A, class B>
    concept derived_from =
      is_base_of_v<B, A> &&
      is_convertible_v<const volatile A*, const volatile B*>;

  template<class From, class To>
    concept convertible_to =
      is_convertible_v<From, To> &&
      requires(From (&f)()) {
        static_cast<To>(f());
      };

  template<class T>
    concept equality_comparable =
      requires(const remove_reference_t<T>& t) {
        { t == t } -> convertible_to<bool>;
        { t != t } -> convertible_to<bool>;
      };

  template<class T>
    concept destructible = is_nothrow_destructible_v<T>;

  template<class T, class... As>
    concept constructible_from =
      destructible<T> && is_constructible_v<T, As...>;

  template<class T>
    concept move_constructible = constructible_from<T, T>;

  template<class T>
    concept copy_constructible =
      move_constructible<T> &&
      constructible_from<T, T const&>;

  template<class F, class... As>
    concept invocable = requires {
      typename invoke_result_t<F, As...>;
    };
}
#endif

namespace std::execution {
  // Exception types:
  extern runtime_error const __invocation_error; // exposition only
  struct receiver_invocation_error : runtime_error, nested_exception {
    receiver_invocation_error() noexcept
      : runtime_error(__invocation_error), nested_exception() {}
  };

  // Invocable archetype
  using invocable_archetype = struct __invocable_archetype {
      void operator()() & {}
  };

  template<template<template<class...> class, template<class...> class> class>
    struct __test_has_values;

  template<template<template<class...> class> class>
    struct __test_has_errors;

  template<class T>
    concept __has_sender_types = requires {
      typename __test_has_values<T::template value_types>;
      typename __test_has_errors<T::template error_types>;
      typename bool_constant<T::sends_done>;
    };

  using sender_base = struct __sender_base {};

  struct __no_sender_traits {
    using __unspecialized = void;
  };

  template<class>
    inline constexpr bool __is_executor = false;

  struct __void_sender {
    template<template<class...> class Tuple, template<class...> class Variant>
      using value_types = Variant<Tuple<>>;
    template<template<class...> class Variant>
      using error_types = Variant<std::exception_ptr>;
    static constexpr bool sends_done = true;
  };

  template<class S>
    struct __typed_sender {
      template<template<class...> class Tuple, template<class...> class Variant>
        using value_types = typename S::template value_types<Tuple, Variant>;
      template<template<class...> class Variant>
        using error_types = typename S::template error_types<Variant>;
      static constexpr bool sends_done = S::sends_done;
    };

  template<class S>
  auto __sender_traits_base_fn() {
    if constexpr (__has_sender_types<S>) {
      return __typed_sender<S>{};
    } else if constexpr (__is_executor<S>) {
      return __void_sender{};
    } else if constexpr (derived_from<S, sender_base>) {
      return sender_base{};
    } else {
      return __no_sender_traits{};
    }
  }

  template<class S>
  struct sender_traits
    : decltype(__sender_traits_base_fn<S>()) {};

  // Customization points:
  template<bool Valid, bool Noexcept>
    struct __result {
      static constexpr bool __valid_niebler = Valid;
      static constexpr bool __noexcept = Noexcept;
    };
  template<class F, class...As>
    concept __valid_niebler = invoke_result_t<F, As...>::__valid_niebler;
  template<class F, class...As>
    inline constexpr bool __noexcept = invoke_result_t<F, As...>::__noexcept;

  namespace __set_value {
    void set_value();

    struct __fn {
      struct __impl {
        template<class R, class... As>
        auto operator()(R&& r, As&&... as) const {
          if constexpr (requires {((R&&)r).set_value((As&&) as...);}) {
            ((R&&)r).set_value((As&&) as...);
            return __result<true, noexcept(((R&&)r).set_value((As&&) as...))>{};
          } else if constexpr (requires {set_value((R&&)r, (As&&) as...);}) {
            set_value((R&&)r, (As&&) as...);
            return __result<true, noexcept(set_value((R&&)r, (As&&) as...))>{};
          } else {
            return __result<false, true>{};
          }
        }
      };
      template<class R, class... As>
        requires __valid_niebler<__impl, R, As...>
      void operator()(R&& r, As&&... as) const noexcept(__noexcept<__impl, R, As...>) {
        (void) __impl{}((R&&) r, (As&&) as...);
      }
    };
  }

  namespace __set_error {
    void set_error();

    struct __fn {
      struct __impl {
        template<class R, class E>
        auto operator()(R&& r, E&& e) const {
          if constexpr (requires {((R&&)r).set_error((E&&) e);}) {
            ((R&&)r).set_error((E&&) e);
            return __result<true, noexcept(((R&&)r).set_error((E&&) e))>{};
          } else if constexpr (requires {set_error((R&&)r, (E&&) e);}) {
            set_error((R&&)r, (E&&) e);
            return __result<true, noexcept(set_error((R&&)r, (E&&) e))>{};
          } else {
            return __result<false, true>{};
          }
        }
      };
      template<class R, class E>
        requires __valid_niebler<__impl, R, E>
      void operator()(R&& r, E&& e) const noexcept(__noexcept<__impl, R, E>) {
        (void) __impl{}((R&&) r, (E&&) e);
      }
    };
  }

  namespace __set_done {
    void set_done();

    struct __fn {
      struct __impl {
        template<class R>
        auto operator()(R&& r) const {
          if constexpr (requires {((R&&)r).set_done();}) {
            ((R&&)r).set_done();
            return __result<true, noexcept(((R&&)r).set_done())>{};
          } else if constexpr (requires {set_done((R&&)r);}) {
            set_done((R&&)r);
            return __result<true, noexcept(set_done((R&&)r))>{};
          } else {
            return __result<false, true>{};
          }
        }
      };
      template<class R>
        requires __valid_niebler<__impl, R>
      void operator()(R&& r) const noexcept(__noexcept<__impl, R>) {
        (void) __impl{}((R&&) r);
      }
    };
  }

  inline namespace __unspecified {
    inline constexpr __set_value::__fn set_value{};
    inline constexpr __set_error::__fn set_error{};
    inline constexpr __set_done::__fn set_done{};
  }

  // Concepts:
  template<class R, class E = exception_ptr>
    concept receiver =
      move_constructible<remove_cvref_t<R>> &&
      constructible_from<remove_cvref_t<R>, R> &&
      requires(remove_cvref_t<R>&& r, E&& e) {
        { execution::set_done(std::move(r)) } noexcept;
        { execution::set_error(std::move(r), (E&&) e) } noexcept;
      };

  template<class R, class... An>
    concept receiver_of =
      receiver<R> &&
      requires(remove_cvref_t<R>&& r, An&&... an) {
        execution::set_value(std::move(r), (An&&) an...);
      };

  template<class R, class...As>
    inline constexpr bool is_nothrow_receiver_of_v =
      receiver_of<R, As...> &&
      is_nothrow_invocable_v<__set_value::__fn, R, As...>;

  template<class R, class E>
    struct __as_invocable;

  template<class F, class S>
    struct __as_receiver;

  template<class S>
    concept sender =
      move_constructible<remove_cvref_t<S>> &&
      !requires {
        typename sender_traits<remove_cvref_t<S>>::__unspecialized;
      };

  static_assert(!sender<int>);

  template<class S>
    concept typed_sender =
      sender<S> &&
      __has_sender_types<sender_traits<remove_cvref_t<S>>>;

  namespace __start {
    void start();

    struct __fn {
      struct __impl {
        template<class O>
        auto operator()(O& o) const noexcept {
          if constexpr (requires {o.start();}) {
            o.start();
            return __result<true, noexcept(o.start())>{};
          } else if constexpr (requires {start(o);}) {
            start(o);
            return __result<true, noexcept(start(o))>{};
          } else {
            return __result<false, true>{};
          }
        }
      };
      template<class O>
        requires __valid_niebler<__impl, O&>
      void operator()(O& o) const noexcept(__noexcept<__impl, O&>) {
        (void) __impl{}(o);
      }
    };
  }

  inline namespace __unspecified {
    inline constexpr __start::__fn start {};
  }

  template<class O>
  concept operation_state =
    destructible<O> &&
    is_object_v<O> &&
    requires (O& o) {
      {execution::start(o)} noexcept;
    };

  namespace __execute {
    void execute();

    struct __fn {
      struct __impl; // defined below
      template<class E, class F, class Impl = __impl>
        requires invocable<remove_cvref_t<F>&> &&
            constructible_from<remove_cvref_t<F>, F> &&
            move_constructible<remove_cvref_t<F>> &&
            __valid_niebler<Impl, E, F>
      void operator()(E&& e, F&& f) const noexcept(__noexcept<Impl, E, F>) {
        (void) Impl{}((E&&) e, (F&&) f);
      }
    };
  }

  inline namespace __unspecified {
    inline constexpr __execute::__fn execute {};
  }

  template<class E, class F>
  concept __executor_of_impl =
    invocable<remove_cvref_t<F>&> &&
    constructible_from<remove_cvref_t<F>, F> &&
    move_constructible<remove_cvref_t<F>> &&
    copy_constructible<E> &&
    equality_comparable<E> &&
    is_nothrow_copy_constructible_v<E> &&
    requires(const E& e, F&& f) {
      execution::execute(e, (F&&) f);
    };

  template<class E>
  concept executor = __executor_of_impl<E, execution::invocable_archetype>;

  template<class E, class F>
  concept executor_of = executor<E> && __executor_of_impl<E, F>;

  struct __void_receiver {
    void set_value() noexcept;
    void set_error(std::exception_ptr) noexcept;
    void set_done() noexcept;
  };

  template<class E>
    requires __executor_of_impl<E, __as_invocable<__void_receiver, E>>
  inline constexpr bool __is_executor<E> = true;

  template<class R, class E>
  struct __as_invocable {
    R* r_ ;
    explicit __as_invocable(R& r) noexcept
      : r_(std::addressof(r)) {}
    __as_invocable(__as_invocable&& other) noexcept
      : r_(std::exchange(other.r_, nullptr)) {}
    ~__as_invocable() {
      if(r_)
        execution::set_done((R&&) *r_);
    }
    void operator()() & noexcept try {
      execution::set_value((R&&) *r_);
      r_ = nullptr;
    } catch(...) {
      execution::set_error((R&&) *r_, current_exception());
      r_ = nullptr;
    }
  };

  namespace __connect {
    void connect();

    template<class E, class R>
      inline constexpr bool __can_execute =
        receiver_of<R> &&
        __executor_of_impl<E, __as_invocable<remove_cvref_t<R>, E>>;
    template<class E, class F>
      inline constexpr bool __can_execute<E, __as_receiver<F, E>> =
        false;

    struct __fn {
      struct __impl {
        template<class S, class R>
        auto operator()(S&& s, R&& r) const {
          if constexpr (sender<S> && requires {((S&&) s).connect((R&&) r);}) {
            return ((S&&) s).connect((R&&) r);
          } else if constexpr (sender<S> && requires {connect((S&&) s, (R&&) r);}) {
            return connect((S&&) s, (R&&) r);
          } else if constexpr (__can_execute<S, R>) {
            struct __op {
              remove_cvref_t<S> s_;
              remove_cvref_t<R> r_;
              void start() noexcept try {
                execution::execute(std::move(s_), __as_invocable<remove_cvref_t<R>, S>{r_});
              } catch(...) {
                execution::set_error(std::move(r_), current_exception());
              }
            };
            return __op{(S&&) s, (R&&) r};
          }
        }
      };
      template<class S, receiver R>
        requires operation_state<invoke_result_t<__impl, S, R>>
      auto operator()(S&& s, R&& r) const /*noexcept TODO*/ {
        return __impl{}((S&&) s, (R&&) r);
      }
    };
  }
  inline namespace __unspecified {
    inline constexpr __connect::__fn connect {};
  }

  template<class S, class R>
    concept sender_to =
      sender<S> &&
      receiver<R> &&
      requires (S&& s, R&& r) {
        execution::connect((S&&) s, (R&&) r);
      };

  template<class S, class R>
    using connect_result_t = invoke_result_t<__connect::__fn, S, R>;

  namespace __submit {
    template<class S, class R>
    struct __rec {
      struct __wrap {
        __rec* __this;
        template<class...As>
          requires receiver_of<R, As...>
        void set_value(As&&... as) && noexcept(
            is_nothrow_receiver_of_v<R, As...>) {
          execution::set_value(std::move(__this->__r), (As&&) as...);
          delete __this;
        }
        template<class E>
          requires receiver<R, E>
        void set_error(E&& e) && noexcept {
          execution::set_error(std::move(__this->__r), (E&&) e);
          delete __this;
        }
        void set_done() && noexcept {
          execution::set_done(std::move(__this->__r));
          delete __this;
        }
      };
      remove_cvref_t<R> __r;
      connect_result_t<S, __wrap> __state;
      __rec(S&& s, R&& r)
        : __r((R&&) r)
        , __state(execution::connect((S&&) s, __wrap{this}))
      {}
    };

    void submit();

    struct __fn {
      struct __impl {
        template<class S, class R>
        auto operator()(S&& s, R&& r) const {
          if constexpr (requires {((S&&) s).submit((R&&) r);}) {
            ((S&&) s).submit((R&&) r);
            return __result<true, noexcept(((S&&) s).submit((R&&) r))>{};
          } else if constexpr (requires {submit((S&&) s, (R&&) r);}) {
            submit((S&&) s, (R&&) r);
            return __result<true, noexcept(submit((S&&) s, (R&&) r))>{};
          } else {
            execution::start((new __rec<S, R>{(S&&) s, (R&&) r})->__state);
            return __result<true, false>{};
          }
        }
      };
      template<receiver R, sender_to<R> S>
        void operator()(S&& s, R&& r) const noexcept(__noexcept<__impl, S, R>) {
          (void) __impl{}((S&&) s, (R&&) r);
        }
    };
  }
  inline namespace __unspecified {
    inline constexpr __submit::__fn submit {};
  }

  template<class F, class S>
    struct __as_receiver {
      F f_;
      void set_value() noexcept(is_nothrow_invocable_v<F&>) {
        f_();
      }
      [[noreturn]]
      void set_error(std::exception_ptr) noexcept {
        terminate();
      }
      void set_done() noexcept {}
    };

  namespace __execute {
    template<class S, class F>
      inline constexpr bool __can_submit =
        sender_to<S, __as_receiver<remove_cvref_t<F>, S>>;
    template<class S, class R>
      inline constexpr bool __can_submit<S, __as_invocable<R, S>> =
        false;

    struct __fn::__impl {
      template<class E, class F>
      auto operator()(E&& e, F&& f) const {
        if constexpr (requires {((E&&) e).execute((F&&) f);}) {
          ((E&&) e).execute((F&&) f);
          return __result<true, noexcept(((E&&) e).execute((F&&) f))>{};
        } else if constexpr (requires {execute((E&&) e, (F&&) f);}) {
          execute((E&&) e, (F&&) f);
          return __result<true, noexcept(execute((E&&) e, (F&&) f))>{};
        } else if constexpr (__can_submit<E, F>) {
          using R = __as_receiver<remove_cvref_t<F>, E>;
          execution::submit((E&&) e, R{(F&&) f});
          return __result<true, noexcept(execution::submit((E&&) e, R{(F&&) f}))>{};
        } else {
          return __result<false, true>{};
        }
      }
    };
  }

  template<class E>
    struct __as_sender {
    private:
      E ex_;
    public:
      template<template<class...> class Tuple, template<class...> class Variant>
        using value_types = Variant<Tuple<>>;
      template<template<class...> class Variant>
        using error_types = Variant<std::exception_ptr>;
      static constexpr bool sends_done = true;
      explicit __as_sender(E e)
        : ex_((E&&) e) {}
      template<class R>
        requires receiver_of<R>
      connect_result_t<E, R> connect(R&& r) && {
        return execution::connect((E&&) ex_, (R&&) r);
      }
      template<class R>
        requires receiver_of<R>
      connect_result_t<const E &, R> connect(R&& r) const & {
        return execution::connect(ex_, (R&&) r);
      }
    };

  namespace __schedule {
    void schedule();

    struct __fn {
      struct __impl {
        template<class S>
        auto operator()(S&& s) const {
          if constexpr (requires {((S&&)s).schedule();}){
            return ((S&&)s).schedule();
          } else if constexpr (requires {schedule((S&&)s);}) {
            return schedule((S&&)s);
          } else if constexpr (executor<S>) {
            return __as_sender<remove_cvref_t<S>>{(S&&) s};
          }
        }
      };
      template<class S>
        requires sender<invoke_result_t<__impl, S>>
      auto operator()(S&& s) const { //noexcept(TODO) {
        return __impl{}((S&&) s);
      }
    };
  }
  inline namespace __unspecified {
    inline constexpr __schedule::__fn schedule{};
  }

  template<class S>
    concept scheduler =
      copy_constructible<remove_cvref_t<S>> &&
      equality_comparable<remove_cvref_t<S>> &&
      requires(S&& s) {
        execution::schedule((S&&) s);
      };

} // namespace std::execution