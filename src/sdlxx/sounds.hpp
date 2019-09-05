
#pragma once

#include "raii.hpp"
#include "sdl_disabled_warnings.h"
#include <cstdint>
#include <platform/filesystem.hpp>

namespace sdlxx {

    using Music = Raii<Mix_Music, &Mix_FreeMusic>;

    using Chunk = Raii<Mix_Chunk, &Mix_FreeChunk>;

    inline Chunk load_wav(const stdnext::filesystem::path& sound_path) noexcept
    {
        return Chunk{Mix_LoadWAV(sound_path.string().c_str())};
    }

    inline int play(const Chunk& chunk, int channel_id = -1) noexcept
    {
        return Mix_PlayChannel(channel_id, chunk.get(), 0);
    }

    inline void pause(int channel_id) noexcept
    {
        Mix_Pause(channel_id);
    }

} // namespace sdlxx
