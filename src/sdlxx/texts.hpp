
#pragma once

#include "graphics.hpp"
#include "raii.hpp"
#include "sdl_disabled_warnings.h"
#include <cstdint>
#include <platform/filesystem.hpp>

namespace sdlxx {

    using Font = Raii<TTF_Font, &TTF_CloseFont>;

    inline result<Font> load_font(const stdnext::filesystem::path& font_path,
                          int font_size) noexcept
    {
        const auto sdl_font =
            TTF_OpenFont(font_path.string().c_str(), font_size);
        if (!sdl_font)
            return stdnext::make_error_code(stdnext::errc::invalid_argument);
        return Font{*sdl_font};
    }

    inline result<Surface> create_text_surface(const Font& font,
                                               const char* text,
                                       ColorAlpha color) noexcept
    {
        const auto sdl_surface =
            TTF_RenderText_Solid(to_sdl(font), text, to_sdl(color));
        if (!sdl_surface)
            return stdnext::make_error_code(stdnext::errc::invalid_argument);
        return Surface{*sdl_surface};
    }

    inline result<Texture> create_text_texture(const Renderer& renderer,
                                       const Font& font, const char* text,
                                       ColorAlpha color) noexcept
    {
        BOOST_OUTCOME_TRY(surface, create_text_surface(font, text, color));
        return create_texture(renderer, surface);
    }

} // namespace sdlxx
