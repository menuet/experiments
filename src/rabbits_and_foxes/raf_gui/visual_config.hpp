
#pragma once

#include "board.hpp"
#include <map>
#include <memory>
#include <platform/filesystem.hpp>
#include <sdlxx/assets.hpp>
#include <sdlxx/error_handling.hpp>
#include <sdlxx/geometry.hpp>
#include <sdlxx/texts.hpp>
#include <string>

namespace raf { namespace raf_v1 { namespace visual {

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

}}} // namespace raf::raf_v1::visual

namespace raf { namespace raf_v2 { namespace visual {

    using Offsets = std::map<std::string, sdlxx::Size>;
    using Boards = std::map<std::string, Board>;

    class Config
    {
    public:
        struct Server
        {
            std::string address;
            std::string port;
        };

        Server server;
        sdlxx::Size board_size;
        sdlxx::Size border_size;
        sdlxx::Size cell_size;
        Offsets offsets;
        Boards boards;
    };

    sdlxx::result<Config>
    load_config(const stdnext::filesystem::path& config_file_path);

    inline auto logical_to_screen(const Point& location,
                                  const Config& config) noexcept
    {
        return sdlxx::Point{
            location.x * config.cell_size.w() + config.border_size.w(),
            location.y * config.cell_size.h() + config.border_size.h()};
    }

    inline auto logical_to_screen(const Size& size,
                                  const Config& config) noexcept
    {
        return sdlxx::Size{size.w * config.cell_size.w(),
                           size.h * config.cell_size.h()};
    }

    inline auto logical_to_screen(const Rectangle& rectangle,
                                  const Config& config) noexcept
    {
        return sdlxx::Rectangle{logical_to_screen(rectangle.location(), config),
                                logical_to_screen(rectangle.size(), config)};
    }

    inline auto screen_to_logical(const sdlxx::Point& location,
                                  const Config& config) noexcept
    {
        return raf_v2::Point{
            (location.x() - config.border_size.w()) / config.cell_size.w(),
            (location.y() - config.border_size.h()) / config.cell_size.h()};
    }

    class Assets
    {
    public:
        Assets(sdlxx::Repository<sdlxx::Texture>&& textures,
               const sdlxx::Texture& board_texture, sdlxx::Font&& font)
            : textures(std::move(textures)), board_texture(&board_texture),
              font(std::move(font))
        {
        }

        sdlxx::Repository<sdlxx::Texture> textures;
        const sdlxx::Texture* board_texture;
        sdlxx::Font font;
    };

    sdlxx::result<std::unique_ptr<Assets>>
    load_assets(const sdlxx::Renderer& renderer,
                const stdnext::filesystem::path& assets_file_path);

}}} // namespace raf::raf_v2::visual

namespace raf { namespace visual {
    //#define USE_OLD_V1

#ifdef USE_OLD_V1
    using raf_v1::visual::Config;
    using raf_v1::visual::load_config;
    constexpr auto config_file_name = "config_v1.json";
#else
    using raf_v2::visual::Config;
    using raf_v2::visual::load_config;
    constexpr auto config_file_name = "config_v2.json";
#endif

}} // namespace raf::visual
