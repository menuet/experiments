
#pragma once


#include <filesystem>
#include <chrono>
#include <fstream>
#include <string>
#include <ctime>


namespace delaysubtitles {

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
        sscanf_s(timeString.c_str(), "%d:%d:%d,%d", &time.m_hours, &time.m_minutes, &time.m_seconds, &time.m_milliseconds);
        return time;
    }

    static std::string formatTime(const Time& time)
    {
        std::string timeString;
        timeString.resize(std::strlen("HH:MM:SS:III") + 1);
        sprintf_s(&*timeString.begin(), timeString.length(), "%02d:%02d:%02d,%03d", time.m_hours, time.m_minutes, time.m_seconds, time.m_milliseconds);
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
        const std::experimental::filesystem::path& sourceFilePath,
        const std::experimental::filesystem::path& targetFilePath,
        const std::chrono::milliseconds& delay
        )
    {
        std::ofstream targetFile(targetFilePath);
        std::ifstream sourceFile(sourceFilePath);
        std::string sourceLine;
        while (sourceFile)
        {
            std::getline(sourceFile, sourceLine);
            delayLine(sourceLine, delay);
            targetFile.write(sourceLine.c_str(), sourceLine.length());
            targetFile << "\n";
        }
    }

} // namespace delaysubtitles
