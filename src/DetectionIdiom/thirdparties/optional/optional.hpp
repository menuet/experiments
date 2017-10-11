
#pragma once

#if defined(SCJ_OPTIONAL_DO_NOT_USE_CONTRACT)
#define SCJ_CONTRACT_ASSERT(expr)
#elif !defined(SCJ_CONTRACT_ASSERT)
#ifdef _MSC_VER
#pragma message("error: You should define the macro SCJ_CONTRACT_ASSERT in order to check the contract expression and handle the failure")
#else
#pragma error "You should define the macro SCJ_CONTRACT_ASSERT in order to check the contract expression and handle the failure"
#endif
#endif

#include <optional/stdex_optional.hpp>
#include <type_traits>
#include <utility>

namespace scj {

    using std::experimental::nullopt_t;
    using std::experimental::nullopt;
    using std::experimental::in_place_t;
    using std::experimental::in_place;

    template <typename T>
    using stdex_optional = std::experimental::optional<T>;

    template <typename T>
    class optional : public stdex_optional<T> {

        using Base = stdex_optional<T>;

    public:

        constexpr optional() noexcept = default;

        constexpr optional(nullopt_t) : Base(nullopt) {}

        optional(const optional& opt) : Base(opt) {}

        optional(optional&& opt) : Base(std::move(opt)) {}

        template< typename... Args >
        explicit optional(in_place_t, Args&&... args) : Base(in_place_t{}, std::forward<Args>(args)...) {}

        template< typename U, typename... Args >
        explicit optional(in_place_t, std::initializer_list<U> ilist, Args&&... args)
        : Base(in_place_t{}, ilist, std::forward<Args>(args)...) {}

        optional(const T& value) : Base(value) {}

        optional(T&& value) : Base(std::move(value)) {}

#if 0 // TODO: fix it
        template< typename U = T>
        explicit optional(U&& value) : Base(std::forward<U>(value)) {}

        template< typename U >
        optional(const optional<U>& other) : Base(other) {}

        template< typename U >
        optional(optional<U>&& other) : Base(std::move(other)) {}
#endif

        optional& operator=(nullopt_t) { Base::operator=(nullopt); return *this; }

        optional& operator=(const optional& opt) { Base::operator=(opt); return *this; }

        optional& operator=(optional&& opt) { Base::operator=(std::move(opt)); return *this; }

        optional& operator=(const T& value) { Base::operator=(value); return *this; }

        optional& operator=(T&& value) { Base::operator=(std::move(value)); return *this; }

#if 0 // TODO: fix it
        template< typename U = T >
        optional& operator=(U&& value) { Base::operator=(std::forward<U>(value)); return *this; }

        template< typename U >
        optional& operator=(const optional<U>& other) { Base::operator=(other); return *this; }

        template< typename U >
        optional& operator=(optional<U>&& other ) { Base::operator=(std::move(other)); return *this; }
#endif

        void swap(optional& rhs) noexcept { Base::swap(rhs); }

        T* operator->() {
            SCJ_CONTRACT_ASSERT(this->has_value());
            return Base::operator->();
        }

        T const& operator*() const & {
            SCJ_CONTRACT_ASSERT(this->has_value());
            return Base::operator*();
        }

        T& operator*() & {
            SCJ_CONTRACT_ASSERT(this->has_value());
            return Base::operator*();
        }

        T& operator*() && {
            SCJ_CONTRACT_ASSERT(this->has_value());
            return std::move(*this).Base::operator*();
        }

        // Forbid usage of member function value
        T const& value() const& = delete;
        T& value() & = delete;
        T&& value() && = delete;
    };

    template< typename T>
    inline void swap(optional<T>& x, optional<T>& y) noexcept(noexcept(x.swap(y))) {
        x.swap(y);
    }

    template< typename T >
    inline auto make_optional(T&& value) {
        return optional<typename std::decay<T>::type>(std::forward<T>(value));
    }

    template< typename T, typename... Args >
    inline auto make_optional(Args&&... args) {
        return optional<T>(std::forward<Args>(args)...);
    }

} // namespace scj
