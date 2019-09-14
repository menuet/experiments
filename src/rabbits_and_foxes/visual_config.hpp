
#pragma once

#include "board.hpp"
#include <map>
#include <platform/filesystem.hpp>
#include <sdlxx/error_handling.hpp>
#include <sdlxx/geometry.hpp>
#include <string>

namespace raf { namespace visual {

    class Config
    {
    public:
        sdlxx::Size board_size;
        sdlxx::Size border_size;
        sdlxx::Size cell_size;
        std::map<std::string, Board> boards;
    };

    sdlxx::result<Config>
    load_config(const stdnext::filesystem::path& config_file_path);

    namespace detail {
        template <typename GeometryT>
        struct LogicalToScreen;

        template <>
        struct LogicalToScreen<sdlxx::Point>
        {
            template <typename PieceT>
            auto operator()(const PieceT& piece, const Config& config) const
                noexcept
            {
                const auto location = piece.location();
                return sdlxx::Point{
                    location.x * config.cell_size.w() + config.border_size.w(),
                    location.y * config.cell_size.h() + config.border_size.h()};
            }
        };

        template <>
        struct LogicalToScreen<sdlxx::Size>
        {
            template <typename PieceT>
            auto operator()(const PieceT& piece, const Config& config) const
                noexcept
            {
                const auto size = piece.size();
                return sdlxx::Size{size.w * config.cell_size.w(),
                                   size.h * config.cell_size.h()};
            }
        };

        template <>
        struct LogicalToScreen<sdlxx::Rectangle>
        {
            template <typename PieceT>
            auto operator()(const PieceT& piece, const Config& config) const
                noexcept
            {
                return sdlxx::Rectangle{
                    LogicalToScreen<sdlxx::Point>{}(piece, config),
                    LogicalToScreen<sdlxx::Size>{}(piece, config)};
            }
        };

    } // namespace detail

    template <typename GeometryT, typename PieceT>
    inline auto logical_to_screen(const PieceT& piece,
                                  const Config& config) noexcept
    {
        return detail::LogicalToScreen<GeometryT>{}(piece, config);
    }

    inline auto screen_to_logical(const sdlxx::Point& location,
                                  const Config& config) noexcept
    {
        return Point{
            (location.x() - config.border_size.w()) / config.cell_size.w(),
            (location.y() - config.border_size.h()) / config.cell_size.h()};
    }

}} // namespace raf::visual
