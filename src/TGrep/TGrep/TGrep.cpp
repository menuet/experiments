
#include "stdafx.h"


using FilePath = std::tr2::sys::path;
using Regex = std::regex;
using FileLineMatch = std::pair < std::string, std::string > ;
using FileLineMatches = std::vector < FileLineMatch > ;

enum class LogLevel
{
    Quiet = 0,
    Minimal = 1,
	Normal = 2,
	Verbose = 3,
};

class Logger
{
public:

    Logger() = default;

    Logger(const Logger&) = delete;

    Logger(Logger&&) = delete;

    Logger(LogLevel a_level)
        : m_level(a_level)
    {
    }

    Logger& operator=(const Logger&) = delete;

    Logger& operator=(Logger&&) = delete;

    template< typename... ArgTs >
    void log(LogLevel a_level, const ArgTs&... a_args) const
    {
        if (a_level > m_level)
            return;

        std::lock_guard<std::mutex> l_guard(m_mutex);

		std::cout << "[" << std::this_thread::get_id() << ":" << (int)m_level << "]:";

        logImpl(a_args...);

        std::cout << "\n";
    }

    void setLevel(LogLevel a_level)
    {
        m_level = a_level;
    }

private:

    template< typename ArgT >
    void logImpl(const ArgT& a_arg) const
    {
        std::cout << a_arg;
    }

    template< typename ArgT0, typename... ArgTs >
    void logImpl(const ArgT0& a_arg0, const ArgTs&... a_args) const
    {
        logImpl(a_arg0);
        logImpl(a_args...);
    }

    LogLevel m_level = LogLevel::Quiet;
    mutable std::mutex m_mutex;
};

struct Options
{
    Logger m_logger;
	bool m_reportFilesOnly = false;
    Regex m_lineRegex;
    Regex m_fileRegex = Regex{ R"(.*)" };
    Regex m_directoryRegex = Regex{ R"(.*)" };
};

static concurrency::task<FileLineMatches> searchInFile(
    const FilePath& a_filePath,
    const Options& a_options
    )
{
    auto l_fileName = a_filePath.filename();
	if (!regex_match(l_fileName, a_options.m_fileRegex))
		return concurrency::task < FileLineMatches > {[]() { return FileLineMatches{}; }};

	if (a_options.m_reportFilesOnly)
		return concurrency::task < FileLineMatches > {[&]() { return FileLineMatches{ FileLineMatch{ a_filePath, std::string{} } }; }};

    return concurrency::task<FileLineMatches>([a_filePath, &a_options]()
    {
        FileLineMatches l_fileLineMatches;
        std::ifstream l_fileStream(a_filePath);
        std::string l_fileLine;
        while (getline(l_fileStream, l_fileLine))
        {
            if (regex_search(l_fileLine, a_options.m_lineRegex))
                l_fileLineMatches.push_back(FileLineMatch(a_filePath, l_fileLine));
        }
        return l_fileLineMatches;
    });
}

static concurrency::task<FileLineMatches> searchInDirectory(
    const FilePath& a_directoryPath,
    const Options& a_options
    )
{
    auto l_fileName = a_directoryPath.filename();
    if (!regex_match(l_fileName, a_options.m_directoryRegex))
        return concurrency::task<FileLineMatches>([]() { return FileLineMatches(); });

//    return concurrency::task<FileLineMatches>([a_directoryPath, &a_options]()
//    {
        a_options.m_logger.log(LogLevel::Normal, "Begin searching in: ", a_directoryPath);

        std::vector<concurrency::task<FileLineMatches>> l_fileLineMatchesCollection;

		unsigned int l_dirCount = 0;
		unsigned int l_fileCount = 0;
        for (auto l_directoryIter = std::tr2::sys::directory_iterator(a_directoryPath); l_directoryIter != std::tr2::sys::directory_iterator(); ++l_directoryIter)
        {
            const auto& l_directoryEntry = *l_directoryIter;
			if (is_regular_file(l_directoryEntry.path()))
			{
				++l_fileCount;
				l_fileLineMatchesCollection.push_back(searchInFile(l_directoryEntry.path(), a_options));
			}
			else if (is_directory(l_directoryEntry.path()))
			{
				++l_dirCount;
				l_fileLineMatchesCollection.push_back(searchInDirectory(l_directoryEntry.path(), a_options));
			}
        }

        auto l_all = concurrency::when_all(begin(l_fileLineMatchesCollection), end(l_fileLineMatchesCollection));

		a_options.m_logger.log(LogLevel::Normal, "End searching in: ", a_directoryPath, " (dirs=", l_dirCount, ",files=", l_fileCount, ")");
		return l_all;
//    });
}

void searchPixlPublicHeadersIncludes()
{
	Options l_options;
	l_options.m_logger.setLevel(LogLevel::Minimal);
	l_options.m_lineRegex = Regex{ R"(^\s*#\s*include\s*[<,"][p,P][i,I][x,X][l,L][\\,/].*[>,"])" };
	l_options.m_fileRegex = Regex{ R"(.*\.(?:h|hpp|c|cpp))" };
	l_options.m_directoryRegex = Regex{ R"(.*)" };

	const auto l_startTime = std::chrono::steady_clock::now();

	// C:\DEV\DESKTOP\DEV\src
	// C:\DEV\PIXL\TRUNK\src\core\jumbos
	// C:\DEV\PIXL\TRUNK\src\core\jumbos\chainmgr
	searchInDirectory(R"(C:\DEV\DESKTOP\DEV\src)", l_options).then([&](FileLineMatches a_fileLineMatches)
	{
		const auto l_stopTime = std::chrono::steady_clock::now();

		const auto l_duration = std::chrono::duration<double>(l_stopTime - l_startTime);

		l_options.m_logger.log(LogLevel::Minimal, "Duration: ", l_duration.count(), " s, Matches: ", a_fileLineMatches.size());

		const auto l_regexExtract = Regex{ R"(^\s*#\s*include\s*[<,"][p,P][i,I][x,X][l,L][\\,/](.*)[>,"])" };

		FileLineMatches l_refinedMatches;
		for (auto&& l_fileLineMatch : a_fileLineMatches)
		{
			//			l_options.m_logger.log(LogLevel::Minimal, l_fileLineMatch.second);
			std::smatch l_results;
			if (std::regex_search(l_fileLineMatch.second, l_results, l_regexExtract) && l_results.size() == 2)
				l_refinedMatches.emplace_back(l_fileLineMatch.first, l_results[1]);
			//            l_options.m_logger.log(LogLevel::Minimal, R"({"File":")", l_fileLineMatch.first, R"(", "Line":")", l_fileLineMatch.second, R"("})");
		}
		l_options.m_logger.log(LogLevel::Minimal, "Refined Matches: ", l_refinedMatches.size());

		for (auto&& l_refinedMatch : l_refinedMatches)
		{
			std::transform(begin(l_refinedMatch.second), end(l_refinedMatch.second), begin(l_refinedMatch.second), ::tolower);
		}

		std::sort(begin(l_refinedMatches), end(l_refinedMatches), [](decltype(*begin(l_refinedMatches)) a_m1, decltype(*begin(l_refinedMatches)) a_m2)
		{
			return a_m1.second < a_m2.second;
		});
		const auto l_last = std::unique(begin(l_refinedMatches), end(l_refinedMatches), [](decltype(*begin(l_refinedMatches)) a_m1, decltype(*begin(l_refinedMatches)) a_m2)
		{
			return a_m1.second == a_m2.second;
		});
		l_refinedMatches.erase(l_last, end(l_refinedMatches));
		l_options.m_logger.log(LogLevel::Minimal, "Sorted Unique Matches: ", l_refinedMatches.size());

		for (auto&& l_refinedMatch : l_refinedMatches)
		{
			l_options.m_logger.log(LogLevel::Minimal, l_refinedMatch.second);
		}

	}).wait();
}

void browsePixlPublicHeaders()
{
	Options l_options;
	l_options.m_logger.setLevel(LogLevel::Minimal);
	l_options.m_reportFilesOnly = true;
	l_options.m_lineRegex = Regex{ R"(^\s*#\s*include\s*[<,"][p,P][i,I][x,X][l,L][\\,/].*[>,"])" };
	l_options.m_fileRegex = Regex{ R"(.*\.(?:h|hpp|c|cpp))" };
	l_options.m_directoryRegex = Regex{ R"(.*)" };

	const auto l_startTime = std::chrono::steady_clock::now();

	searchInDirectory(R"(C:\DEV\PIXL\TRUNK\src\core\pixl)", l_options).then([&](FileLineMatches a_fileLineMatches)
	{
		const auto l_stopTime = std::chrono::steady_clock::now();

		const auto l_duration = std::chrono::duration<double>(l_stopTime - l_startTime);

		l_options.m_logger.log(LogLevel::Minimal, "Duration: ", l_duration.count(), " s, Matches: ", a_fileLineMatches.size());

		for (auto&& l_refinedMatch : a_fileLineMatches)
		{
			std::transform(begin(l_refinedMatch.first), end(l_refinedMatch.first), begin(l_refinedMatch.first), ::tolower);
		}

		std::sort(begin(a_fileLineMatches), end(a_fileLineMatches), [](decltype(*begin(a_fileLineMatches)) a_m1, decltype(*begin(a_fileLineMatches)) a_m2)
		{
			return a_m1.second < a_m2.second;
		});

		l_options.m_logger.log(LogLevel::Minimal, "Sorted Matches: ", a_fileLineMatches.size());

		for (auto&& l_refinedMatch : a_fileLineMatches)
		{
			l_options.m_logger.log(LogLevel::Minimal, l_refinedMatch.first);
		}

	}).wait();
}

int main(int argc, char* argv [])
{
	browsePixlPublicHeaders();
//	searchPixlPublicHeadersIncludes();
	return 0;
}
