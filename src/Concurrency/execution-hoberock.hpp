
#pragma once

struct execute_customization_point
{
  template<class E, class F,
           P0443_REQUIRES(has_execute_member_function<E&&,F&&>::value)
          >
  constexpr auto operator()(E&& e, F&& f) const ->
    decltype(std::forward<E>(e).execute(std::forward<F>(f)))
  {
    return std::forward<E>(e).execute(std::forward<F>(f));
  }

  template<class E, class F,
           P0443_REQUIRES(!has_execute_member_function<E&&,F&&>::value and
                          has_execute_free_function<E&&,F&&>::value)
          >
  constexpr auto operator()(E&& e, F&& f) const ->
    decltype(execute(std::forward<E>(e), std::forward<F>(f)))
  {
    return execute(std::forward<E>(e), std::forward<F>(f));
  }

  template<class E, class F,
           P0443_REQUIRES(!has_execute_member_function<E&&,F&&>::value and
                          !has_execute_free_function<E&&,F&&>::value and
                          has_submit_member_function<E&&,invocable_as_receiver<F&&>>::value)
          >
  constexpr auto operator()(E&& e, F&& f) const ->
    decltype(std::forward<E>(e).submit(detail::as_receiver(std::forward<F>(f))))
  {
    // note that this overload does not use execution::submit to avoid circular dependency
    return std::forward<E>(e).submit(detail::as_receiver(std::forward<F>(f)));
  }

  template<class E, class F,
           P0443_REQUIRES(!has_execute_member_function<E&&,F&&>::value and
                          !has_execute_free_function<E&&,F&&>::value and
                          !has_submit_member_function<E&&,invocable_as_receiver<F&&>>::value and
                          has_submit_free_function<E&&,invocable_as_receiver<F&&>>::value)
          >
  constexpr auto operator()(E&& e, F&& f) const ->
    decltype(submit(std::forward<E>(e), detail::as_receiver(std::forward<F>(f))))
  {
    // note that this overload does not use execution::submit to avoid circular dependency
    return submit(std::forward<E>(e), detail::as_receiver(std::forward<F>(f)));
  }
};