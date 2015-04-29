
#pragma once


#include <filesystem>
#include <memory>
#include <utility>
#include <sfml/audio.hpp>
#include <windows.h>


static std::tr2::sys::path getRunningModuleDirectory()
{
    std::tr2::sys::path l_runningModuleDirectory;

    static const char l_dummyChar = 0;
    HMODULE l_hModule{};
    if (::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, &l_dummyChar, &l_hModule) && l_hModule)
    {
        char l_runningModuleDirectoryString[_MAX_PATH]{};
        if (::GetModuleFileNameA(l_hModule, l_runningModuleDirectoryString, _countof(l_runningModuleDirectoryString)))
        {
            l_runningModuleDirectory = l_runningModuleDirectoryString;
            l_runningModuleDirectory.remove_filename();
        }
    }

    return l_runningModuleDirectory;
}

static std::tr2::sys::path getResourceFileAbsolutePathName(const std::tr2::sys::path& a_resourceFileName)
{
    if (a_resourceFileName.has_root_path())
        return a_resourceFileName;
    auto l_resourceFileAbsolutePathName = getRunningModuleDirectory();
    l_resourceFileAbsolutePathName /= a_resourceFileName;
    return l_resourceFileAbsolutePathName;
}

class Sound
{
public:

    Sound()
        : m_spSound{ std::make_unique<std::pair<sf::SoundBuffer, sf::Sound>>() }
    {
    }

    Sound(const std::tr2::sys::path& a_soundFileName)
        : m_spSound{ std::make_unique<std::pair<sf::SoundBuffer, sf::Sound>>() }
    {
        load(a_soundFileName);
    }

    Sound(Sound&& a_sound)
        : m_spSound{ std::move(a_sound.m_spSound) }
    {
    }

    Sound& operator=(Sound&& a_sound)
    {
        m_spSound = std::move(a_sound.m_spSound);
        return *this;
    }

    bool load(const std::tr2::sys::path& a_soundFileName)
    {
        const auto l_soundFilePathName = getResourceFileAbsolutePathName(a_soundFileName);
        m_spSound->second.stop();
        m_spSound->second.resetBuffer();
        if (!m_spSound->first.loadFromFile(l_soundFilePathName))
            return false;
        m_spSound->second.setBuffer(m_spSound->first);
        return true;
    }

    sf::Sound& get()
    {
        return m_spSound->second;
    }

    const sf::Sound& get() const
    {
        return m_spSound->second;
    }

private:

    std::unique_ptr<std::pair<sf::SoundBuffer, sf::Sound>> m_spSound;
};
