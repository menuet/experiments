
#pragma once


#if defined(__GNUC__)
#include <boost/filesystem.hpp>
#else
#include <filesystem>
#endif
#include <chrono>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <iostream>


namespace delaysubtitles {

#if defined(__GNUC__)
    namespace fs = boost::filesystem;
    template< typename... Ts >
    inline void string_scanf(const std::string& s, const char* format, const Ts&... ts)
    {
        sscanf(s.c_str(), format, ts...); // not safe
    }
    template< typename... Ts >
    inline void string_printf(std::string& s, const char* format, const Ts&... ts)
    {
        char sz[500] = {0};
        sprintf(sz, format, ts...);
        s = sz;
    }
#else
    namespace fs = std::experimental::filesystem;
    template< typename... Ts >
    inline void string_scanf(const std::string& s, const char* format, const Ts&... ts)
    {
        sscanf_s(s.c_str(), format, ts...); // not safe
    }
    template< typename... Ts >
    inline void string_printf(std::string& s, const char* format, const Ts&... ts)
    {
        char sz[500] = {0};
        sprintf_s(sz, format, ts...);
        s = sz;
    }
#endif

    struct Time
    {
        int m_hours = 0;
        int m_minutes = 0;
        int m_seconds = 0;
        int m_milliseconds = 0;
    };

    static Time parseTime(const std::string& timeString)
    {
        Time time;
        string_scanf(timeString, "%d:%d:%d,%d", &time.m_hours, &time.m_minutes, &time.m_seconds, &time.m_milliseconds);
        return time;
    }

    static std::string formatTime(const Time& time)
    {
        std::string timeString;
        string_printf(timeString, "%02d:%02d:%02d,%03d", time.m_hours, time.m_minutes, time.m_seconds, time.m_milliseconds);
        timeString.resize(timeString.length() - 1);
        return timeString;
    }

    static void delayTime(Time& time, const std::chrono::milliseconds& delay)
    {
        auto chrono = std::chrono::hours(time.m_hours) + std::chrono::minutes(time.m_minutes) + std::chrono::seconds(time.m_seconds) + std::chrono::milliseconds(time.m_milliseconds);
        chrono += delay;
        if (chrono.count() < 0)
            return;
        time.m_hours = std::chrono::duration_cast<std::chrono::hours>(chrono).count();
        chrono -= std::chrono::hours(time.m_hours);
        time.m_minutes = std::chrono::duration_cast<std::chrono::minutes>(chrono).count();
        chrono -= std::chrono::minutes(time.m_minutes);
        time.m_seconds = (int)std::chrono::duration_cast<std::chrono::seconds>(chrono).count();
        chrono -= std::chrono::seconds(time.m_seconds);
        time.m_milliseconds = (int)std::chrono::duration_cast<std::chrono::milliseconds>(chrono).count();
    }

    static void delayTime(std::string& timeString, const std::chrono::milliseconds& delay)
    {
        auto time = parseTime(timeString);
        delayTime(time, delay);
        timeString = formatTime(time);
    }

    static void delayLine(std::string& line, const std::chrono::milliseconds& delay)
    {
        const auto arrowPos = line.find(" --> ");
        if (arrowPos == std::string::npos)
            return;
        auto startTimeString = line.substr(0, arrowPos);
        delayTime(startTimeString, delay);
        auto stopTimeString = line.substr(arrowPos + 5);
        delayTime(stopTimeString, delay);
        line = startTimeString;
        line += " --> ";
        line += stopTimeString;
    }

    static void delaySubTitles(
        const std::chrono::milliseconds& delay,
        const fs::path& sourcePath,
        fs::path targetPath
        )
    {
        if (fs::is_directory(sourcePath))
        {
            std::vector<fs::path> sourceFilesPaths;
            const auto iterEnd = fs::directory_iterator();
            for (auto iter = fs::directory_iterator(sourcePath) ; iter!=iterEnd; ++iter)
            {
                const auto& entry = *iter;
                const auto path = entry.path();
                const auto extension = path.extension();
                if (fs::is_regular_file(path) && extension == ".srt")
                    sourceFilesPaths.push_back(path);
            }
            std::cout << "Found " << sourceFilesPaths.size() << " in directory '" << sourcePath << "'\n";
            for (const auto& sourceFilePath : sourceFilesPaths)
            {
                delaySubTitles(delay, sourceFilePath, fs::path());
            }
        }
        else
        {
            std::ifstream sourceFile(sourcePath.string());
            if (!sourceFile)
            {
                std::cout << "Could not read the subtitle file '" << sourcePath << "'\n";
                return;
            }
            if (targetPath.empty())
            {
                targetPath = sourcePath;
                targetPath.remove_filename();
                targetPath /= targetPath.stem().string() + "-delayed.srt";
            }
            std::ofstream targetFile(targetPath.string());
            std::string sourceLine;
            while (sourceFile)
            {
                std::getline(sourceFile, sourceLine);
                delayLine(sourceLine, delay);
                targetFile.write(sourceLine.c_str(), sourceLine.length());
                targetFile << "\n";
            }
        }
    }

} // namespace delaysubtitles
