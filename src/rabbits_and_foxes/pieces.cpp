
#include "pieces.hpp"
#include "board.hpp"
#include <cassert>
#include <cmath>

namespace raf {

    static std::optional<Size> compute_step(Point src, Point dst,
                                            int min_offset) noexcept
    {
        const auto offset = dst - src;
        if (offset.w != 0)
        {
            if (offset.h != 0)
                return std::nullopt;
            if (std::abs(offset.w) < min_offset)
                return std::nullopt;
            return Size{offset.w > 0 ? 1 : -1, 0};
        }
        else
        {
            if (offset.h == 0)
                return std::nullopt;
            if (std::abs(offset.h) < min_offset)
                return std::nullopt;
            return Size{0, offset.h > 0 ? 1 : -1};
        }
    }

    template <typename PieceT>
    static bool is_above_something(const PieceT& piece,
                                   const Rectangles& things_rectangles) noexcept
    {
        return std::any_of(things_rectangles.begin(), things_rectangles.end(),
                           [&](const auto& thing_rectangle) {
                               return are_intersecting(piece.rectangle(),
                                                       thing_rectangle);
                           });
    }

    static void add_possible_move(Rabbit rabbit, Size step,
                                  const Rectangle& landscape_rectangle,
                                  const Rectangles& things_rectangles,
                                  Points& possible_moves) noexcept
    {
        rabbit = rabbit.move_to(rabbit.location() + step);
        if (!is_included(rabbit.rectangle(), landscape_rectangle))
            return;
        if (!is_above_something(rabbit, things_rectangles))
            return;
        do
        {
            rabbit = rabbit.move_to(rabbit.location() + step);
            if (!is_included(rabbit.rectangle(), landscape_rectangle))
                return;
        } while (is_above_something(rabbit, things_rectangles));
        possible_moves.push_back(rabbit.location());
    }

    static void add_possible_moves(Fox fox, Size step,
                                   const Rectangle& landscape_rectangle,
                                   const Rectangles& things_rectangles,
                                   Points& possible_moves) noexcept
    {
        fox = fox.move_to(fox.location() + step);
        while (is_included(fox.rectangle(), landscape_rectangle) &&
               !is_above_something(fox, things_rectangles))
        {
            possible_moves.push_back(fox.location());
            fox = fox.move_to(fox.location() + step);
        }
    }

    bool Rabbit::can_move_to(Point final_location,
                             const BoardLandscape& landscape,
                             const BoardState& state) const noexcept
    {
        auto rabbit = move_to(final_location);
        if (!is_included(rabbit.rectangle(), landscape.rectangle()))
            return false;

        auto intermediate_location = this->location();
        const auto step_opt =
            compute_step(intermediate_location, final_location, 2);
        if (!step_opt)
            return false;
        const auto& step = *step_opt;
        assert((std::abs(step.w) == 1 && step.h == 0) ||
               (std::abs(step.h) == 1 && step.w == 0));

        const auto things_rectangles =
            collect_rectangles_without(landscape, state, this->rectangle());

        // All intermediate locations must contain a rabbit or a fox or a
        // mushroom
        for (intermediate_location = intermediate_location + step;
             intermediate_location != final_location;
             intermediate_location = intermediate_location + step)
        {
            rabbit = rabbit.move_to(intermediate_location);
            if (!is_above_something(rabbit, things_rectangles))
                return false;
        }

        // Final location must NOT contain a rabbit or a fox or a mushroom
        rabbit = rabbit.move_to(final_location);
        if (is_above_something(rabbit, things_rectangles))
            return false;

        return true;
    }

    Points Rabbit::all_possible_moves(const BoardLandscape& landscape,
                                      const BoardState& state) const
    {
        const auto landscape_rectangle = landscape.rectangle();
        const auto things_rectangles =
            collect_rectangles_without(landscape, state, this->rectangle());
        Points possible_moves;
        const auto possible_steps = Sizes{{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        for (const auto& step : possible_steps)
        {
            add_possible_move(*this, step, landscape_rectangle,
                              things_rectangles, possible_moves);
        }
        return possible_moves;
    }

    bool Fox::can_move_to(Point final_location, const BoardLandscape& landscape,
                          const BoardState& state) const noexcept
    {
        auto fox = move_to(final_location);
        if (!is_included(fox.rectangle(), landscape.rectangle()))
            return false;

        auto intermediate_location = this->location();
        const auto step_opt =
            compute_step(intermediate_location, final_location, 1);
        if (!step_opt)
            return false;
        const auto& step = *step_opt;
        assert((std::abs(step.w) == 1 && step.h == 0) ||
               (std::abs(step.h) == 1 && step.w == 0));

        const auto things_rectangles =
            collect_rectangles_without(landscape, state, this->rectangle());

        // Orientation of movement must be the same as the fox's orientation
        if (step.w != 0 && fox.orientation() != FoxOrientation::Horizontal)
            return false;
        if (step.h != 0 && fox.orientation() != FoxOrientation::Vertical)
            return false;

        // All locations must contain nothing
        for (intermediate_location = intermediate_location + step;
             intermediate_location != final_location;
             intermediate_location = intermediate_location + step)
        {
            fox = fox.move_to(intermediate_location);
            if (is_above_something(fox, things_rectangles))
                return false;
        }

        fox = fox.move_to(final_location);
        if (is_above_something(fox, things_rectangles))
            return false;

        return true;
    }

    Points Fox::all_possible_moves(const BoardLandscape& landscape,
                                   const BoardState& state) const
    {
        const auto landscape_rectangle = landscape.rectangle();
        const auto things_rectangles =
            collect_rectangles_without(landscape, state, this->rectangle());
        Points possible_moves;
        const auto possible_steps = m_orientation == FoxOrientation::Horizontal
                                        ? Sizes{{-1, 0}, {1, 0}}
                                        : Sizes{{0, -1}, {0, 1}};
        for (const auto& step : possible_steps)
        {
            add_possible_moves(*this, step, landscape_rectangle,
                               things_rectangles, possible_moves);
        }
        return possible_moves;
    }

} // namespace raf
