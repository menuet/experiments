
#pragma once

#include <memory>

namespace sdlxx {

    namespace detail {

        template <typename T, void (*F)(T*)>
        struct Destroyer
        {
            void operator()(T* t) const noexcept { F(t); }
        };

    } // namespace detail

    template <typename T, void (*F)(T*)>
    using Raii = std::unique_ptr<T, detail::Destroyer<T, F>>;

} // namespace sdlxx
