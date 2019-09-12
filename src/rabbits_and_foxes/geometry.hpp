
#pragma once

#include <vector>

namespace raf {

    class BoardLandscape;
    class BoardState;

    class Point
    {
    public:
        int x{0};
        int y{0};
    };

    constexpr inline bool operator==(Point lhs, Point rhs) noexcept
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    constexpr inline bool operator!=(Point lhs, Point rhs) noexcept
    {
        return !(lhs == rhs);
    }

    using Points = std::vector<Point>;

    class Size
    {
    public:
        int w{0};
        int h{0};
    };

    constexpr inline bool operator==(Size lhs, Size rhs) noexcept
    {
        return lhs.w == rhs.w && lhs.h == rhs.h;
    }

    constexpr inline bool operator!=(Size lhs, Size rhs) noexcept
    {
        return !(lhs == rhs);
    }

    constexpr inline Point operator+(Point p, Size s) noexcept
    {
        return Point{p.x + s.w, p.y + s.h};
    }

    constexpr inline Size operator-(Point dst, Point src) noexcept
    {
        return Size{dst.x - src.x, dst.y - src.y};
    }

    using Sizes = std::vector<Size>;

    class Rectangle
    {
    public:
        constexpr Rectangle(Point location, Size size) noexcept
            : m_location(location), m_size(size)
        {
        }

        constexpr const Point& location() const noexcept { return m_location; }

        constexpr const Size& size() const noexcept { return m_size; }

    private:
        Point m_location;
        Size m_size;
    };

    constexpr inline bool operator==(const Rectangle& lhs,
                                     const Rectangle& rhs) noexcept
    {
        return lhs.location() == rhs.location() && lhs.size() == rhs.size();
    }

    constexpr inline bool operator!=(const Rectangle& lhs,
                                     const Rectangle& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    constexpr inline bool is_included(const Rectangle& small,
                                      const Rectangle& big) noexcept
    {
        if (small.location().x < big.location().x)
            return false;
        if (small.location().x + small.size().w >
            big.location().x + big.size().w)
            return false;
        if (small.location().y < big.location().y)
            return false;
        if (small.location().y + small.size().h >
            big.location().y + big.size().h)
            return false;
        return true;
    }

    constexpr inline bool are_intersecting(const Rectangle& r1,
                                           const Rectangle& r2) noexcept
    {
        if (r1.location().x + r1.size().w <= r2.location().x)
            return false;
        if (r2.location().x + r2.size().w <= r1.location().x)
            return false;
        if (r1.location().y + r1.size().h <= r2.location().y)
            return false;
        if (r2.location().y + r2.size().h <= r1.location().y)
            return false;
        return true;
    }

    using Rectangles = std::vector<Rectangle>;

} // namespace raf
