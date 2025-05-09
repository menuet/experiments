
#pragma once

namespace delaysubtitles {

    namespace fs = std::filesystem;

    struct Time
    {
        int m_hours = 0;
        int m_minutes = 0;
        int m_seconds = 0;
        int m_milliseconds = 0;

        auto operator<=>(const Time&) const = default;
    };

    static constexpr Time parseTime(const std::string_view& timeString)
    {
        const auto toInt = [](const auto& sv) {
            int value{};
            auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
            if (ec != std::errc())
                return 0;
            return value;
        };

        Time time{};

        auto splitSemi = timeString | std::views::split(':');

        auto iterSemi = splitSemi.begin();

        if (iterSemi == splitSemi.end())
            return time;

        time.m_hours = toInt(*iterSemi++);

        if (iterSemi == splitSemi.end())
            return time;

        time.m_minutes = toInt(*iterSemi++);

        if (iterSemi == splitSemi.end())
            return time;

        auto spliComa = (*iterSemi) | std::views::split(',');

        auto iterComa = spliComa.begin();

        if (iterComa == spliComa.end())
            return time;

        time.m_seconds = toInt(*iterComa++);

        if (iterComa == spliComa.end())
            return time;

        time.m_milliseconds = toInt(*iterComa++);

        return time;
    }

    static_assert(parseTime("10:19:28,123") == Time{10, 19, 28, 123});

    static std::string formatTime(const Time& time)
    {
        std::string timeString =
            std::format("{:02}:{:02}:{:02},{:03}", time.m_hours, time.m_minutes, time.m_seconds, time.m_milliseconds);
        return timeString;
    }

    static void delayTime(Time& time, const std::chrono::milliseconds& delay)
    {
        auto chrono = std::chrono::hours(time.m_hours) + std::chrono::minutes(time.m_minutes) +
                      std::chrono::seconds(time.m_seconds) + std::chrono::milliseconds(time.m_milliseconds);
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

    static void delaySubTitles(const std::chrono::milliseconds& delay, const fs::path& sourcePath, fs::path targetPath)
    {
        if (fs::is_directory(sourcePath))
        {
            std::vector<fs::path> sourceFilesPaths;
            const auto iterEnd = fs::directory_iterator();
            for (auto iter = fs::directory_iterator(sourcePath); iter != iterEnd; ++iter)
            {
                const auto& entry = *iter;
                const auto path = entry.path();
                const auto extension = path.extension();
                if (fs::is_regular_file(path) && extension == ".srt")
                    sourceFilesPaths.push_back(path);
            }
            std::cout << "Found " << sourceFilesPaths.size() << " srt file in directory '" << sourcePath << "'\n";
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
            std::cout << "sourcePath = '" << sourcePath << "'\n";
            if (targetPath.empty())
            {
                targetPath = sourcePath;
                targetPath.replace_extension(".delayed" + sourcePath.extension().string());
                std::cout << "targetPath = '" << targetPath << "'\n";
            }
            std::ofstream targetFile(targetPath.string());
            std::string sourceLine;
            while (sourceFile)
            {
                std::getline(sourceFile, sourceLine);
                delayLine(sourceLine, delay);
                targetFile.write(sourceLine.c_str(), static_cast<std::streamsize>(sourceLine.length()));
                targetFile << "\n";
            }
        }
    }

} // namespace delaysubtitles
