
#include <cstddef>

#define CONSTEXPR_ASSERT(pred)                                                 \
    if (!(pred))                                                               \
        throw 0;

namespace maze {

    enum class Direction
    {
        Left,
        Right,
        Up,
        Down,
    };

    struct Loc
    {
        std::size_t col = 0;
        std::size_t row = 0;

        constexpr Loc(const std::size_t t_col, const std::size_t t_row) noexcept
            : col(t_col), row(t_row)
        {
        }

        constexpr Loc() noexcept = default;

        constexpr Loc left() const noexcept { return {col - 1, row}; }

        constexpr Loc right() const noexcept { return {col + 1, row}; }

        constexpr Loc up() const noexcept { return {col, row - 1}; }

        constexpr Loc down() const noexcept { return {col, row + 1}; }

        constexpr bool operator==(const Loc& other) const noexcept
        {
            return col == other.col && row == other.row;
        }

        constexpr bool operator!=(const Loc& other) const noexcept
        {
            return !operator==(other);
        }
    };

    template <typename Data, std::size_t Cols, std::size_t Rows>
    struct Matrix
    {
        constexpr static auto rows() noexcept { return Rows; }
        constexpr static auto cols() noexcept { return Cols; }

        constexpr Data& operator()(const std::size_t col,
                                   const std::size_t row) noexcept
        {
            return m_data[col + (row * Cols)];
        }

        constexpr const Data& operator()(const std::size_t col,
                                         const std::size_t row) const noexcept
        {
            return m_data[col + (row * Cols)];
        }

        constexpr Data& operator()(Loc loc) noexcept
        {
            return m_data[loc.col + (loc.row * Cols)];
        }

        constexpr const Data& operator()(Loc loc) const noexcept
        {
            return m_data[loc.col + (loc.row * Cols)];
        }

        constexpr Data& at(Loc loc)
        {
            CONSTEXPR_ASSERT(loc.col < Cols);
            CONSTEXPR_ASSERT(loc.row < Rows);
            return m_data[loc.col + (loc.row * Cols)];
        }

        constexpr const Data& at(Loc loc) const
        {
            CONSTEXPR_ASSERT(loc.col < Cols);
            CONSTEXPR_ASSERT(loc.row < Rows);
            return m_data[loc.col + (loc.row * Cols)];
        }

        Data m_data[Cols * Rows] = {};
    };

} // namespace maze
