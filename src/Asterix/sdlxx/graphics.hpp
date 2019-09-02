
#pragma once

#include "raii.hpp"
#include "geometry.hpp"
#include "sdl_disabled_warnings.h"
#include <platform/filesystem.hpp>

namespace sdlxx {

    using Window = Raii<SDL_Window, &SDL_DestroyWindow>;

    using Renderer =Raii<SDL_Renderer, &SDL_DestroyRenderer>;

    using Surface = Raii<SDL_Surface, &SDL_FreeSurface>;

    using Texture =Raii<SDL_Texture, &SDL_DestroyTexture>;

    inline Window create_window(const char* title, const Rectangle& coordinates,
                                std::uint32_t flags) noexcept
    {
        return Window(SDL_CreateWindow(
            title, coordinates.origin().x(), coordinates.origin().y(),
            coordinates.size().w(), coordinates.size().h(), flags));
    }

    inline Renderer create_renderer(const Window& window,
                                    std::uint32_t flags) noexcept
    {
        return Renderer(SDL_CreateRenderer(window.get(), -1, flags));
    }

    inline Surface
    load_surface(const stdnext::filesystem::path& image_path) noexcept
    {
        return Surface(SDL_LoadBMP(image_path.string().c_str()));
    }

    inline Surface load_surface(const stdnext::filesystem::path& image_path,
                                Color color_key) noexcept
    {
        Surface surface(SDL_LoadBMP(image_path.string().c_str()));
        if (!surface)
            return surface;
        const auto key = SDL_MapRGB(surface->format, color_key.r(),
                                    color_key.g(), color_key.b());
        SDL_SetColorKey(surface.get(), 1, key);
        return surface;
    }

    inline Texture create_texture(const Renderer& renderer,
                                  const Surface& surface) noexcept
    {
        return Texture(
            SDL_CreateTextureFromSurface(renderer.get(), surface.get()));
    }

    inline Texture
    load_texture(const Renderer& renderer,
                 const stdnext::filesystem::path& image_path) noexcept
    {
        const auto surface = load_surface(image_path);
        if (!surface)
            return Texture{};
        return create_texture(renderer, surface);
    }

    inline Texture load_texture(const Renderer& renderer,
                                const stdnext::filesystem::path& image_path,
                                Color color_key) noexcept
    {
        const auto surface = load_surface(image_path, color_key);
        if (!surface)
            return Texture{};
        return create_texture(renderer, surface);
    }

    inline Size get_size(const Window& window) noexcept
    {
        int w = 0;
        int h = 0;
        SDL_GetWindowSize(window.get(), &w, &h);
        return Size(w, h);
    }

    inline Size get_size(const Texture& texture) noexcept
    {
        int w = 0;
        int h = 0;
        SDL_QueryTexture(texture.get(), nullptr, nullptr, &w, &h);
        return Size(w, h);
    }

    inline void render_texture(const Renderer& renderer,
                               const Texture& texture) noexcept
    {
        SDL_RenderCopy(renderer.get(), texture.get(), nullptr, nullptr);
    }

    inline void render_texture(const Renderer& renderer, const Texture& texture,
                               Point dst_position) noexcept
    {
        const auto sdl_dst_zone =
            Rectangle{dst_position, get_size(texture)}.to_sdl();
        SDL_RenderCopy(renderer.get(), texture.get(), nullptr, &sdl_dst_zone);
    }

    inline void render_texture(const Renderer& renderer, const Texture& texture,
                               const Rectangle& src_zone,
                               Point dst_position) noexcept
    {
        const auto sdl_src_zone = src_zone.to_sdl();
        const auto sdl_dst_zone =
            Rectangle{dst_position, get_size(texture)}.to_sdl();
        SDL_RenderCopy(renderer.get(), texture.get(), &sdl_src_zone,
                       &sdl_dst_zone);
    }

    inline void render_texture(const Renderer& renderer, const Texture& texture,
                               const Rectangle& src_zone,
                               const Rectangle& dst_zone) noexcept
    {
        const auto sdl_src_zone = src_zone.to_sdl();
        const auto sdl_dst_zone = dst_zone.to_sdl();
        SDL_RenderCopy(renderer.get(), texture.get(), &sdl_src_zone,
                       &sdl_dst_zone);
    }

    inline void clear(const Renderer& renderer) noexcept
    {
        SDL_RenderClear(renderer.get());
    }

    inline void present(const Renderer& renderer) noexcept
    {
        SDL_RenderPresent(renderer.get());
    }

} // namespace sdlxx
