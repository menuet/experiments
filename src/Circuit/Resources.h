
#pragma once


#include <SFML/Audio.hpp>
#include <platform/filesystem.hpp>
#include <memory>
#include <utility>
#include <vector>
#include <regex>
#include <cstdio>
#if EXP_PLATFORM_OS_IS_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace stdnextfs = stdnext::filesystem;

static stdnextfs::path getRunningModuleDirectory()
{
    stdnextfs::path l_runningModuleDirectory;
#if EXP_PLATFORM_OS_IS_WINDOWS
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
#else
    static constexpr std::size_t MODULE_PATH_SIZE = 2048;
    char runningModulePath[MODULE_PATH_SIZE];
    const auto realSize = ::readlink("/proc/self/exe", runningModulePath, MODULE_PATH_SIZE - 1);
    if (realSize >= 0) {
        l_runningModuleDirectory.assign(runningModulePath, runningModulePath+realSize);
        l_runningModuleDirectory.remove_filename();
    }
#endif
    return l_runningModuleDirectory;
}

static stdnextfs::path getResourcesDirectory()
{
    auto l_resourcesDir = getRunningModuleDirectory();
    l_resourcesDir /= "Resources";
    return l_resourcesDir;
}

static stdnextfs::path getResourceFileAbsolutePathName(const stdnextfs::path& a_resourceFileName)
{
//    if (a_resourceFileName.has_root_path())
//        return a_resourceFileName;
    auto l_resourceFileAbsolutePathName = getResourcesDirectory();
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

    Sound(const stdnextfs::path& a_soundFileName)
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

    bool load(const stdnextfs::path& a_soundFileName)
    {
        const auto l_soundFilePathName = getResourceFileAbsolutePathName(a_soundFileName);
        m_spSound->second.stop();
        m_spSound->second.resetBuffer();
        if (!m_spSound->first.loadFromFile(l_soundFilePathName.string()))
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

static std::vector<Sound> loadSoundsSeries(std::string l_soundFilePrefix)
{
    std::vector<Sound> l_sounds;
    l_soundFilePrefix += R"((\d).wav)";
    const std::regex l_soundsPattern{ l_soundFilePrefix };
    const auto l_resourcesDirectory = getResourcesDirectory();
    std::for_each(stdnextfs::directory_iterator(l_resourcesDirectory), stdnextfs::directory_iterator(), [&](const stdnextfs::path& a_resourceFilePathName)
    {
        const auto& l_resourceFileName = a_resourceFilePathName.filename().string();
        std::smatch l_matchResults;
        if (std::regex_match(l_resourceFileName, l_matchResults, l_soundsPattern))
        {
            if (l_matchResults.size() == 2 && l_matchResults[1].length() == 1)
            {
                const auto l_digit = *l_matchResults[1].first;
                if (l_digit >= '0' && l_digit <= '9')
                {
                    const auto l_index = static_cast<size_t>(l_digit - '0');
                    l_sounds.resize(l_index + 1);
                    auto& l_sound = l_sounds[l_index];
                    l_sound.load(a_resourceFilePathName);
                }
            }
        }
    });
    return l_sounds;
}
