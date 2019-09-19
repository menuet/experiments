
#pragma once

#include "pieces.hpp"
#include <platform/ranges.hpp>
#include <tuple>

namespace raf { namespace raf_v2 {

    using PieceInfo = std::tuple<Piece, Point, std::size_t>;

    inline auto piece_of(const PieceInfo& piece_info) noexcept
    {
        return std::get<0>(piece_info);
    }

    static const auto loc_of = [](const PieceInfo& piece_info) noexcept
    {
        return std::get<1>(piece_info);
    };

    inline auto index_of(const PieceInfo& piece_info) noexcept
    {
        return std::get<2>(piece_info);
    }

    inline auto is_of_type(Piece::Type piece_type) noexcept
    {
        return [=](const PieceInfo& piece_info) {
            return std::get<0>(piece_info).type() == piece_type;
        };
    }

    static const auto rect_of = [](const PieceInfo& piece_info) noexcept
    {
        return Rectangle{std::get<1>(piece_info),
                         std::get<0>(piece_info).size()};
    };

    static const auto is_in_odd_row_or_column =
        [](const PieceInfo& piece_info) noexcept
    {
        const auto location = loc_of(piece_info);
        switch (std::get<0>(piece_info).orientation())
        {
        case FoxFacet::Orientation::Horizontal:
            return location.y % 2 == 1;
        case FoxFacet::Orientation::Vertical:
            return location.x % 2 == 1;
        }
        return true;
    };

    inline auto has_this_index(std::size_t index) noexcept
    {
        return [index](const PieceInfo& piece_info) noexcept
        {
            return std::get<2>(piece_info) == index;
        };
    }

    inline auto merge_pieces_infos(const Pieces& pieces,
                                   const Points& pieces_locations)
    {
        return stdnext::ranges::views::zip(
            pieces, pieces_locations,
            stdnext::ranges::views::ints(0, stdnext::ranges::unreachable));
    }

    inline auto remove_holes()
    {
        return stdnext::ranges::views::filter(
            std::not_fn(is_of_type(Piece::Type::Hole)));
    }

    inline auto keep_holes_only()
    {
        return stdnext::ranges::views::filter(is_of_type(Piece::Type::Hole));
    }

    inline auto keep_rabbits_only()
    {
        return stdnext::ranges::views::filter(is_of_type(Piece::Type::Rabbit));
    }

    inline auto keep_foxes_only()
    {
        return stdnext::ranges::views::filter(is_of_type(Piece::Type::Fox));
    }

    inline auto keep_mushrooms_only()
    {
        return stdnext::ranges::views::filter(
            is_of_type(Piece::Type::Mushroom));
    }

    inline auto remove_this_piece(std::size_t piece_index)
    {
        return stdnext::ranges::views::filter(
            std::not_fn(has_this_index(piece_index)));
    }

    inline auto intersecting_with(const Rectangle& piece_rectangle)
    {
        return stdnext::ranges::views::filter([=](const auto& piece_info) {
            return are_intersecting(rect_of(piece_info), piece_rectangle);
        });
    }

}} // namespace raf::raf_v2
