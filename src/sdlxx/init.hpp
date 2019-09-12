
#pragma once

#include "sdl_disabled_warnings.h"

namespace sdlxx {

    struct MainLib
    {
        Uint32 flags{SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS};
    };

    struct ImageLib
    {
        int flags{IMG_INIT_PNG};
    };

    struct MixerLib
    {
        int frequency{44100};
        Uint16 format{MIX_DEFAULT_FORMAT};
        int channels{2};
        int chunk_size{4096};
    };

    struct TextLib
    {
    };

    namespace detail {
        template <typename LibT>
        struct LibTraits;

        template <>
        struct LibTraits<MainLib>
        {
            using Arg = MainLib;

            static bool init(const Arg& arg) noexcept
            {
                const auto result = SDL_Init(arg.flags);
                return result == 0;
            }

            static void quit() noexcept { SDL_Quit(); }
        };

        template <>
        struct LibTraits<ImageLib>
        {
            using Arg = ImageLib;

            static bool init(const Arg& arg) noexcept
            {
                const auto result = IMG_Init(arg.flags);
                return result == arg.flags;
            }

            static void quit() noexcept { IMG_Quit(); }
        };

        template <>
        struct LibTraits<MixerLib>
        {
            using Arg = MixerLib;

            static bool init(const Arg& arg) noexcept
            {
                const auto result = Mix_OpenAudio(arg.frequency, arg.format,
                                                  arg.channels, arg.chunk_size);
                return result == 0;
            }

            static void quit() noexcept { Mix_CloseAudio(); }
        };

        template <>
        struct LibTraits<TextLib>
        {
            using Arg = TextLib;

            static bool init(const Arg&) noexcept
            {
                const auto result = TTF_Init();
                return result == 0;
            }

            static void quit() noexcept { TTF_Quit(); }
        };

        template <typename LibT>
        class LibInitializer
        {
        public:
            explicit LibInitializer(const LibT& lib) noexcept
                : m_init_result(LibTraits<LibT>::init(lib))
            {
            }

            ~LibInitializer() noexcept
            {
                if (!m_init_result)
                    return;
                LibTraits<LibT>::quit();
            }

            LibInitializer(const LibInitializer&) = delete;
            LibInitializer& operator=(const LibInitializer&) = delete;

            LibInitializer(LibInitializer&& other) = delete;
            LibInitializer& operator=(LibInitializer&& other) = delete;

            explicit operator bool() const noexcept { return m_init_result; }

        private:
            bool m_init_result;
        };
    } // namespace detail

    template <typename... LibTs>
    class Initializer : private detail::LibInitializer<MainLib>,
                        private detail::LibInitializer<LibTs>...
    {
    public:
        Initializer(const MainLib& main_lib,
                    const LibTs&... other_libs) noexcept
            : detail::LibInitializer<MainLib>(main_lib),
              detail::LibInitializer<LibTs>(other_libs)...
        {
        }

        explicit operator bool() const noexcept
        {
            return detail::LibInitializer<MainLib>::operator bool() &&
                   (detail::LibInitializer<LibTs>::operator bool() && ...);
        }
    };

} // namespace sdlxx
