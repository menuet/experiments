
#pragma once

#include <SDL.h>
#include <cstdint>

namespace sdlxx {

    class Point
    {
    public:
        constexpr Point(int x, int y) noexcept : m_x(x), m_y(y) {}

        constexpr int x() const noexcept { return m_x; }

        constexpr int y() const noexcept { return m_y; }

    private:
        int m_x;
        int m_y;
    };

    class Size
    {
    public:
        constexpr Size(int w, int h) noexcept : m_w(w), m_h(h) {}

        constexpr int w() const noexcept { return m_w; }

        constexpr int h() const noexcept { return m_h; }

    private:
        int m_w;
        int m_h;
    };

    class Rectangle
    {
    public:
        constexpr Rectangle(Point origin, Size size) noexcept
            : m_origin(origin), m_size(size)
        {
        }

        constexpr const Point& origin() const noexcept { return m_origin; }

        constexpr const Size& size() const noexcept { return m_size; }

        constexpr SDL_Rect to_sdl() const noexcept
        {
            return SDL_Rect{m_origin.x(), m_origin.y(), m_size.w(), m_size.h()};
        }

    private:
        Point m_origin;
        Size m_size;
    };

    class Color
    {
    public:
        constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b) noexcept
            : m_r(r), m_g(g), m_b(b)
        {
        }

        constexpr std::uint8_t r() const noexcept { return m_r; }

        constexpr std::uint8_t g() const noexcept { return m_g; }

        constexpr std::uint8_t b() const noexcept { return m_b; }

        constexpr SDL_Color to_sdl() const noexcept
        {
            return SDL_Color{m_r, m_g, m_b, 255};
        }

    private:
        std::uint8_t m_r;
        std::uint8_t m_g;
        std::uint8_t m_b;
    };

    class ColorAlpha : public Color
    {
    public:
        constexpr ColorAlpha(Color color, std::uint8_t a) noexcept
            : Color(color), m_a(a)
        {
        }

        constexpr ColorAlpha(std::uint8_t r, std::uint8_t g, std::uint8_t b,
                             std::uint8_t a) noexcept
            : Color(r, g, b), m_a(a)
        {
        }

        constexpr std::uint8_t a() const noexcept { return m_a; }

        constexpr SDL_Color to_sdl() const noexcept
        {
            return SDL_Color{r(), g(), b(), m_a};
        }

    private:
        std::uint8_t m_a;
    };

    inline constexpr bool are_colliding(const Rectangle& r1,
                                        const Rectangle& r2) noexcept
    {
        if (r1.origin().x() > r2.origin().x() + r2.size().w())
            return false;
        if (r1.origin().x() + r1.size().w() < r2.origin().x())
            return false;
        if (r1.origin().y() > r2.origin().y() + r2.size().h())
            return false;
        if (r1.origin().y() + r1.size().h() < r2.origin().y())
            return false;
        return true;
    }

} // namespace sdlxx
