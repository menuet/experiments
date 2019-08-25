
#pragma once

#include <SDL.h>

namespace sdlxx {

    class Initializer
    {
    public:
        explicit Initializer(std::uint32_t flags) noexcept
            : m_init_result(SDL_Init(flags))
        {
        }

        ~Initializer() noexcept
        {
            if (m_init_result != 0)
                return;
            SDL_Quit();
        }

        Initializer(const Initializer&) = delete;
        Initializer& operator=(const Initializer&) = delete;

        explicit operator bool() const noexcept { return m_init_result == 0; }

    private:
        int m_init_result;
    };

} // namespace sdlxx
