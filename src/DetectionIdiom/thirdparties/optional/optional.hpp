
#pragma once


#if !defined(SCJ_OPTIONAL_USE_CUSTOM_CONTRACT)
    #define SCJ_OPTIONAL_USE_CUSTOM_CONTRACT 0
#endif

#if !SCJ_OPTIONAL_USE_CUSTOM_CONTRACT
    #define SCJ_OPTIONAL_CONTRACT_ASSERT(expr) optionalContractAssert(expr)
#endif

#if !defined(SCJ_OPTIONAL_OVERWRITE_NAMESPACE_STD_EXPERIMENTAL)
    #define SCJ_OPTIONAL_OVERWRITE_NAMESPACE_STD_EXPERIMENTAL 0
#endif

#if SCJ_OPTIONAL_OVERWRITE_NAMESPACE_STD_EXPERIMENTAL
    #define OPTIONAL_EXPERIMENTAL_NAMESPACE hidden_experimental
#else
    #define OPTIONAL_EXPERIMENTAL_NAMESPACE experimental
#endif

#include <optional/stdex_optional.hpp>
#include <type_traits>
#include <utility>
#include <stdexcept>
#include <cstring>


namespace scj {

    struct OptionalContractFailure : std::logic_error {
        using std::logic_error::logic_error;
    };

    inline void optionalContractAssert(bool expr) {
        if (expr)
            return;
        throw OptionalContractFailure("optional contract failure");
    }

    using std::OPTIONAL_EXPERIMENTAL_NAMESPACE::nullopt_t;
    using std::OPTIONAL_EXPERIMENTAL_NAMESPACE::nullopt;
    using std::OPTIONAL_EXPERIMENTAL_NAMESPACE::in_place_t;
    using std::OPTIONAL_EXPERIMENTAL_NAMESPACE::in_place;

    template <typename T>
    using stdex_optional = std::OPTIONAL_EXPERIMENTAL_NAMESPACE::optional<T>;

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

        const T* operator->() const {
            SCJ_OPTIONAL_CONTRACT_ASSERT(this->has_value());
            return Base::operator->();
        }

        T* operator->() {
            SCJ_OPTIONAL_CONTRACT_ASSERT(this->has_value());
            return Base::operator->();
        }

        const T& operator*() const & {
            SCJ_OPTIONAL_CONTRACT_ASSERT(this->has_value());
            return Base::operator*();
        }

#if 0 // TODO: fix it
        const T&& operator*() const && {
            SCJ_OPTIONAL_CONTRACT_ASSERT(this->has_value());
            return Base::operator*();
        }
#endif

        T& operator*() & {
            SCJ_OPTIONAL_CONTRACT_ASSERT(this->has_value());
            return Base::operator*();
        }

        T& operator*() && {
            SCJ_OPTIONAL_CONTRACT_ASSERT(this->has_value());
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

#if SCJ_OPTIONAL_OVERWRITE_NAMESPACE_STD_EXPERIMENTAL
namespace std { namespace experimental {
    using scj::nullopt_t;
    using scj::nullopt;
    using scj::in_place_t;
    using scj::in_place;
    using scj::swap;
    using scj::make_optional;
    using scj::optional;
} }
#endif
