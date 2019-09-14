
#pragma once

#include "geometry.hpp"
#include "raii.hpp"
#include "error_handling.hpp"
#include <platform/filesystem.hpp>

namespace sdlxx {

    using Window = Raii<SDL_Window, &SDL_DestroyWindow>;

    using Renderer = Raii<SDL_Renderer, &SDL_DestroyRenderer>;

    using Surface = Raii<SDL_Surface, &SDL_FreeSurface>;

    using Texture = Raii<SDL_Texture, &SDL_DestroyTexture>;

    constexpr Point CenteredWindow{SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED};

    inline result<Window> create_window(const char* title, Size size, Point origin,
                                std::uint32_t flags) noexcept
    {
        const auto sdl_window = SDL_CreateWindow(title, origin.x(), origin.y(),
                                                 size.w(), size.h(), flags);
        if (!sdl_window)
            return stdnext::make_error_code(stdnext::errc::invalid_argument);
        return Window{*sdl_window};
    }

    inline result<Window> create_window(const char* title, Size size) noexcept
    {
        return create_window(title, size, CenteredWindow, SDL_WINDOW_SHOWN);
    }

    inline result<Renderer> create_renderer(const Window& window,
                                    std::uint32_t flags) noexcept
    {
        const auto sdl_renderer = SDL_CreateRenderer(to_sdl(window), -1, flags);
        if (!sdl_renderer)
            return stdnext::make_error_code(stdnext::errc::invalid_argument);
        return Renderer{*sdl_renderer};
    }

    inline result<Renderer> create_renderer(const Window& window) noexcept
    {
        return create_renderer(window, SDL_RENDERER_ACCELERATED |
                                           SDL_RENDERER_PRESENTVSYNC);
    }

    inline result<Surface>
    load_surface(const stdnext::filesystem::path& image_path) noexcept
    {
        const auto sdl_surface = IMG_Load(image_path.string().c_str());
        if (!sdl_surface)
            return stdnext::make_error_code(stdnext::errc::invalid_argument);
        return Surface{*sdl_surface};
    }

    inline result<Surface>
    load_surface(const stdnext::filesystem::path& image_path,
                                Color color_key) noexcept
    {
        const auto sdl_surface = IMG_Load(image_path.string().c_str());
        if (!sdl_surface)
            return stdnext::make_error_code(stdnext::errc::invalid_argument);
        const auto key = SDL_MapRGB(sdl_surface->format, color_key.r(),
                                    color_key.g(), color_key.b());
        SDL_SetColorKey(sdl_surface, 1, key);
        return Surface{*sdl_surface};
    }

    inline result<Texture> create_texture(const Renderer& renderer,
                                  const Surface& surface) noexcept
    {
        const auto sdl_surface =
            SDL_CreateTextureFromSurface(to_sdl(renderer), to_sdl(surface));
        if (!sdl_surface)
            return stdnext::make_error_code(stdnext::errc::invalid_argument);
        return Texture{*sdl_surface};
    }

    inline result<Texture>
    load_texture(const Renderer& renderer,
                 const stdnext::filesystem::path& image_path) noexcept
    {
        BOOST_OUTCOME_TRY(surface, load_surface(image_path));
        return create_texture(renderer, surface);
    }

    inline result<Texture> load_texture(const Renderer& renderer,
                                const stdnext::filesystem::path& image_path,
                                Color color_key) noexcept
    {
        BOOST_OUTCOME_TRY(surface, load_surface(image_path, color_key));
        return create_texture(renderer, surface);
    }

    inline Size get_size(const Window& window) noexcept
    {
        int w = 0;
        int h = 0;
        SDL_GetWindowSize(to_sdl(window), &w, &h);
        return Size(w, h);
    }

    inline Size get_size(const Texture& texture) noexcept
    {
        int w = 0;
        int h = 0;
        SDL_QueryTexture(to_sdl(texture), nullptr, nullptr, &w, &h);
        return Size(w, h);
    }

    inline void render_texture(const Renderer& renderer,
                               const Texture& texture) noexcept
    {
        SDL_RenderCopy(to_sdl(renderer), to_sdl(texture), nullptr, nullptr);
    }

    inline void render_texture(const Renderer& renderer, const Texture& texture,
                               Point dst_position) noexcept
    {
        const auto sdl_dst_zone =
            to_sdl(Rectangle{dst_position, get_size(texture)});
        SDL_RenderCopy(to_sdl(renderer), to_sdl(texture), nullptr, &sdl_dst_zone);
    }

    inline void render_texture(const Renderer& renderer, const Texture& texture,
                               const Rectangle& src_zone,
                               Point dst_position) noexcept
    {
        const auto sdl_src_zone = to_sdl(src_zone);
        const auto sdl_dst_zone =
            to_sdl(Rectangle{dst_position, get_size(texture)});
        SDL_RenderCopy(to_sdl(renderer), to_sdl(texture), &sdl_src_zone,
                       &sdl_dst_zone);
    }

    inline void render_texture(const Renderer& renderer, const Texture& texture,
                               const Rectangle& src_zone,
                               const Rectangle& dst_zone) noexcept
    {
        const auto sdl_src_zone = to_sdl(src_zone);
        const auto sdl_dst_zone = to_sdl(dst_zone);
        SDL_RenderCopy(to_sdl(renderer), to_sdl(texture), &sdl_src_zone,
                       &sdl_dst_zone);
    }

    inline void clear(const Renderer& renderer,
                      Color color = {0, 0, 0}) noexcept
    {
        SDL_SetRenderDrawColor(to_sdl(renderer), color.r(), color.g(),
                               color.b(), 0);
        SDL_RenderClear(to_sdl(renderer));
    }

    inline void present(const Renderer& renderer) noexcept
    {
        SDL_RenderPresent(to_sdl(renderer));
    }

    inline void draw_rectangle(const Renderer& renderer, const Rectangle& rectangle, ColorAlpha color) noexcept
    {
        SDL_SetRenderDrawColor(to_sdl(renderer), color.r(), color.g(), color.b(),
                               color.a());
        const auto sdl_rect = to_sdl(rectangle);
        SDL_RenderDrawRect(to_sdl(renderer), &sdl_rect);
    }

} // namespace sdlxx
