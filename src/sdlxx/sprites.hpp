
#pragma once

#include "graphics.hpp"

namespace sdlxx {

    class SpriteIndex
    {
    public:
        constexpr SpriteIndex(int x, int y) noexcept : m_x(x), m_y(y) {}

        constexpr int x() const noexcept { return m_x; }

        constexpr int y() const noexcept { return m_y; }

    private:
        int m_x;
        int m_y;
    };

    class SpriteSheet
    {
    public:
        friend result<SpriteSheet>
        load_spritesheet(const Renderer& renderer,
                         const stdnext::filesystem::path& image_path,
                         Color yor_key, Size sprite_size) noexcept;

        const Texture& texture() const noexcept { return m_sheet_texture; }

        const Size& sprite_size() const noexcept { return m_sprite_size; }

        const Size& grid_size() const noexcept { return m_grid_size; }

    private:
        SpriteSheet(Texture&& sheet_texture, Size sprite_size, Size grid_size)
            : m_sheet_texture(std::move(sheet_texture)),
              m_sprite_size(sprite_size), m_grid_size(grid_size)
        {
        }

        Texture m_sheet_texture;
        Size m_sprite_size;
        Size m_grid_size;
    };

    inline SpriteIndex next(const SpriteSheet& sprite_sheet, SpriteIndex sprite_index) noexcept
    {
        const auto grid_size = sprite_sheet.grid_size();
        auto x = sprite_index.x() + 1;
        auto y = sprite_index.y();
        if (x >= grid_size.w())
        {
            x = 0;
            ++y;
        }
        if (y >= grid_size.h())
            y = 0;
        return SpriteIndex{x, y};
    }

    inline void render_sprite(const Renderer& renderer,
                              const SpriteSheet& sprite_sheet,
                              SpriteIndex sprite_index,
                              Point dst_origin) noexcept
    {
        const auto sprite_size = sprite_sheet.sprite_size();
        const Point src_origin(sprite_index.x() * sprite_size.w(),
                               sprite_index.y() * sprite_size.h());
        const Rectangle src_zone(src_origin, sprite_size);
        const Rectangle dst_zone(dst_origin, sprite_size);
        render_texture(renderer, sprite_sheet.texture(), src_zone, dst_zone);
    }

    inline void render_sprite(const Renderer& renderer,
                              const SpriteSheet& sprite_sheet,
                              SpriteIndex sprite_index,
                              Rectangle dst_zone) noexcept
    {
        const auto sprite_size = sprite_sheet.sprite_size();
        const Point src_origin(sprite_index.x() * sprite_size.w(),
                               sprite_index.y() * sprite_size.h());
        const Rectangle src_zone(src_origin, sprite_size);
        render_texture(renderer, sprite_sheet.texture(), src_zone, dst_zone);
    }

    inline result<SpriteSheet>
    load_spritesheet(const Renderer& renderer,
                     const stdnext::filesystem::path& image_path, Color color_key,
                     Size sprite_size) noexcept
    {
        if (sprite_size.w() <= 0 || sprite_size.h() <= 0)
            return stdnext::make_error_code(stdnext::errc::invalid_argument);
        BOOST_OUTCOME_TRY(sheet_texture, load_texture(renderer, image_path, color_key));
        const auto sheet_size = get_size(sheet_texture);
        if (sheet_size.w() < sprite_size.w() ||
            sheet_size.h() < sprite_size.h())
            return stdnext::make_error_code(stdnext::errc::invalid_argument);
        const auto grid_size = Size(sheet_size.w() / sprite_size.w(),
                                    sheet_size.h() / sprite_size.h());
        return SpriteSheet{std::move(sheet_texture), sprite_size, grid_size};
    }

} // namespace sdlxx
