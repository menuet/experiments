
#pragma once

#include <memory>

namespace sdlxx {

    template <typename T, void (*F)(T*)>
    class Raii
    {
    public:
        Raii(T& t) noexcept : m_up(&t) {}

        Raii(Raii&&) = default;
        Raii& operator=(Raii&&) = default;

        Raii(const Raii&) = delete;
        Raii& operator=(const Raii&) = delete;

        friend inline T* to_sdl(const Raii<T, F>& raii) noexcept
        {
            return raii.m_up.get();
        }

    private:
        struct Destroyer
        {
            void operator()(T* t) const noexcept { F(t); }
        };

        using UP = std::unique_ptr<T, Destroyer>;

        UP m_up;
    };

} // namespace sdlxx
