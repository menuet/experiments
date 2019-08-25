
#pragma once

#include "raii.hpp"
#include <SDL_mixer.h>
#include <cstdint>
#include <platform/filesystem.hpp>

namespace sdlxx {

    class AudioInitializer
    {
    public:
        static constexpr int DEFAULT_FREQUENCY = 44100;
        static constexpr std::uint16_t DEFAULT_FORMAT = MIX_DEFAULT_FORMAT;
        static constexpr int DEFAULT_CHANNELS = 2;
        static constexpr int DEFAULT_CHUNKSIZE = 4096;

        explicit AudioInitializer(int frequency = DEFAULT_FREQUENCY,
                                  std::uint16_t format = DEFAULT_FORMAT,
                                  int channels = DEFAULT_CHANNELS,
                                  int chunksize = DEFAULT_CHUNKSIZE) noexcept
            : m_init_result(
                  Mix_OpenAudio(frequency, format, channels, chunksize))
        {
        }

        ~AudioInitializer() noexcept
        {
            if (m_init_result != 0)
                return;
            Mix_CloseAudio();
        }

        AudioInitializer(const AudioInitializer&) = delete;
        AudioInitializer& operator=(const AudioInitializer&) = delete;

        explicit operator bool() const noexcept { return m_init_result == 0; }

    private:
        int m_init_result;
    };

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
