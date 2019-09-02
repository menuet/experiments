
#pragma once

#include "graphics.hpp"
#include <sdlxx/outcome_disabled_warnings.hpp>
#include <platform/filesystem.hpp>
#include <platform/system_error.hpp>

namespace sdlxx {

    namespace bout = BOOST_OUTCOME_V2_NAMESPACE;

    class SpriteSheet;

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

    class Sprite
    {
    public:
        Sprite(const SpriteSheet& sprite_sheet, SpriteIndex index)
            : m_sprite_sheet(&sprite_sheet), m_index(index)
        {
        }

        Sprite next() const noexcept;

        void render(const Renderer& render, Point dst_origin) const noexcept;

    private:
        const SpriteSheet* m_sprite_sheet;
        SpriteIndex m_index;
    };

    class SpriteSheet
    {
    public:
        friend bout::result<SpriteSheet, stdnext::error_code>
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

    inline Sprite Sprite::next() const noexcept
    {
        const auto grid_size = m_sprite_sheet->grid_size();
        auto x = m_index.x() + 1;
        auto y = m_index.y();
        if (x >= grid_size.w())
        {
            x = 0;
            ++y;
        }
        if (y >= grid_size.h())
            y = 0;
        return Sprite(*m_sprite_sheet, SpriteIndex(x, y));
    }

    inline void Sprite::render(const Renderer& render, Point dst_origin) const
        noexcept
    {
        const auto sprite_size = m_sprite_sheet->sprite_size();
        const Point src_origin(m_index.x() * sprite_size.w(),
                               m_index.y() * sprite_size.h());
        const Rectangle src_zone(src_origin, sprite_size);
        const Rectangle dst_zone(dst_origin, sprite_size);
        render_texture(render, m_sprite_sheet->texture(), src_zone, dst_zone);
    }

    inline bout::result<SpriteSheet, stdnext::error_code>
    load_spritesheet(const Renderer& renderer,
                     const stdnext::filesystem::path& image_path, Color color_key,
                     Size sprite_size) noexcept
    {
        if (sprite_size.w() <= 0 || sprite_size.h() <= 0)
            return stdnext::make_error_code(stdnext::errc::invalid_argument);
        auto sheet_texture = load_texture(renderer, image_path, color_key);
        if (!sheet_texture)
            return stdnext::make_error_code(stdnext::errc::invalid_argument);
        const auto sheet_size = get_size(sheet_texture);
        if (sheet_size.w() < sprite_size.w() ||
            sheet_size.h() < sprite_size.h())
            return stdnext::make_error_code(stdnext::errc::invalid_argument);
        const auto grid_size = Size(sheet_size.w() / sprite_size.w(),
                                    sheet_size.h() / sprite_size.h());
        return SpriteSheet(std::move(sheet_texture), sprite_size, grid_size);
    }

} // namespace sdlxx
