
#pragma once

#include "error_handling.hpp"
#include "raii.hpp"
#include <cstdint>
#include <platform/filesystem.hpp>

namespace sdlxx {

    using Music = Raii<Mix_Music, &Mix_FreeMusic>;

    using Chunk = Raii<Mix_Chunk, &Mix_FreeChunk>;

    inline result<Chunk>
    load_wav(const stdnext::filesystem::path& sound_path) noexcept
    {
        const auto sdl_chunk = Mix_LoadWAV(sound_path.string().c_str());
        if (!sdl_chunk)
            return stdnext::make_error_code(stdnext::errc::invalid_argument);
        return Chunk{*sdl_chunk};
    }

    inline int play(const Chunk& chunk, int channel_id = -1) noexcept
    {
        return Mix_PlayChannel(channel_id, to_sdl(chunk), 0);
    }

    inline void pause(int channel_id) noexcept { Mix_Pause(channel_id); }

} // namespace sdlxx
