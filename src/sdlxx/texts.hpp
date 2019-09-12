
#pragma once

#include "graphics.hpp"
#include "raii.hpp"
#include "sdl_disabled_warnings.h"
#include <cstdint>
#include <platform/filesystem.hpp>

namespace sdlxx {

    using Font = Raii<TTF_Font, &TTF_CloseFont>;

    inline Font load_font(const stdnext::filesystem::path& font_path,
                          int font_size) noexcept
    {
        return Font{TTF_OpenFont(font_path.string().c_str(), font_size)};
    }

    inline Surface create_text_surface(const Font& font, const char* text,
                                       ColorAlpha color) noexcept
    {
        return Surface{TTF_RenderText_Solid(font.get(), text, color.to_sdl())};
    }

    inline Texture create_text_texture(const Renderer& renderer,
                                       const Font& font, const char* text,
                                       ColorAlpha color) noexcept
    {
        const auto surface = create_text_surface(font, text, color);
        if (!surface)
            return Texture{};
        return create_texture(renderer, surface);
    }

} // namespace sdlxx
