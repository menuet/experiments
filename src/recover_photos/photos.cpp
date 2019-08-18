
#include "photos.hpp"
#include <boost/process.hpp>
#include <system_error>
#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <algorithm>
#include <numeric>
#if EXP_PLATFORM_OS_IS_WINDOWS
#include <Windows.h>
#endif

namespace fs = stdnext::filesystem;
namespace bproc = boost::process;

static FileDescription load_file_description(const std::string& file_description_line)
{
    FileDescription file_description;
    const auto line_end_iter = end(file_description_line);

    auto prop_begin_iter = begin(file_description_line);
    auto prop_end_iter = std::find(prop_begin_iter, line_end_iter, ',');
    file_description.name = std::string(prop_begin_iter, prop_end_iter);
    if (prop_end_iter == line_end_iter)
        return file_description;

    prop_begin_iter = prop_end_iter + 1;
    prop_end_iter = std::find(prop_begin_iter, line_end_iter, ',');
    const auto is_damaged_string = std::string(prop_begin_iter, prop_end_iter);
    file_description.is_damaged = is_damaged_string == "1";
    if (prop_end_iter == line_end_iter)
        return file_description;

    prop_begin_iter = prop_end_iter + 1;
    prop_end_iter = std::find(prop_begin_iter, line_end_iter, ',');
    const auto file_size_string = std::string(prop_begin_iter, prop_end_iter);
    file_description.size = atoll(file_size_string.c_str());
    if (prop_end_iter == line_end_iter)
        return file_description;

    prop_begin_iter = prop_end_iter + 1;
    prop_end_iter = std::find(prop_begin_iter, line_end_iter, ',');
    const auto is_copied_string = std::string(prop_begin_iter, prop_end_iter);
    file_description.is_copied = is_copied_string == "1";
    if (prop_end_iter == line_end_iter)
        return file_description;

    return file_description;
}

static std::ofstream& operator<<(std::ofstream& ofs, const FileDescription& file_description)
{
    ofs << file_description.name.string() << ',' << file_description.is_damaged << ',' << file_description.size << ',' << file_description.is_copied << "\n";
    return ofs;
}

static FilesDescriptions load_files_descriptions(const fs::path& files_descriptions_file)
{
    std::cout << "Loading files descriptions\n";

    FilesDescriptions files_descriptions;

    std::ifstream ifs(files_descriptions_file.string());
    if (ifs)
    {
        std::string file_description_line;
        while (std::getline(ifs, file_description_line))
        {
            files_descriptions.push_back(load_file_description(file_description_line));
            file_description_line.clear();
        }
    }

    return files_descriptions;
}

static FilesDescriptions collect_files_descriptions(const fs::path& source_dir)
{
    std::cout << "Collecting files descriptions\n";

    FilesDescriptions files_descriptions;
    for (const auto& entry : fs::directory_iterator(source_dir))
    {
        const auto& path = entry.path();
        files_descriptions.push_back(FileDescription{ path.filename().string(), false, fs::file_size(path) });
    }
    return files_descriptions;
}

static std::chrono::milliseconds infer_timeout_from_file_size(const FileDescription& file_description)
{
    return std::min(std::chrono::milliseconds(file_description.size / 1000), std::chrono::milliseconds(60000));
}

static bool copy_file_in_this_thread_adapted(const fs::path& process_file, std::chrono::milliseconds timeout, const fs::path& source_file, const fs::path& target_dir)
{
    copy_file_in_this_thread(source_file, target_dir);
    return true;
}

static bool copy_file_in_separate_thread(const fs::path& process_file, std::chrono::milliseconds timeout, const fs::path& source_file, const fs::path& target_dir)
{
    std::mutex mut;
    std::condition_variable cv;

    std::thread copier_thread([&]
    {
        copy_file_in_this_thread(source_file, target_dir);
        cv.notify_one();
    });

    std::unique_lock<std::mutex> lock(mut);
    const auto cv_wait_result = cv.wait_for(lock, timeout);
    if (cv_wait_result == std::cv_status::timeout)
    {
        std::cout << "Could not copy: " << source_file << " - Elapsed timeout: " << timeout.count() << " ms\n";
        const auto native_thread_handle = copier_thread.native_handle();
        copier_thread.detach();
#if EXP_PLATFORM_OS_IS_WINDOWS
        ::TerminateThread(native_thread_handle, 0);
#endif
        return false;
    }

    copier_thread.join();
    return true;
}

static bool copy_file_in_separate_process(const fs::path& process_file, std::chrono::milliseconds timeout, const fs::path& source_file, const fs::path& target_dir)
{
    std::error_code ec;
    bproc::child c(ec, process_file.string(), "--mode", "slave", "--source_file", source_file.string(), "--target_dir", target_dir.string());
    if (ec)
        std::cout << "Error " << ec.message() << " when spawning: " << process_file << "\n";

    const auto cv_wait_result = c.wait_for(timeout);
    if (!cv_wait_result)
    {
        std::cout << "Could not copy: " << source_file << " - Elapsed timeout: " << timeout.count() << " ms\n";
        c.terminate(ec);
        std::cout << "Child was terminated with error message: " << ec << "\n";
        return false;
    }

    return true;
}

static void setup_sorting(Sorting sorting, FilesDescriptions& files_descriptions)
{
    switch (sorting)
    {
    case Sorting::NameDesc:
        std::sort(begin(files_descriptions), end(files_descriptions), [&](const auto& desc1, const auto& desc2)
        {
            if (desc1.is_copied < desc2.is_copied)
                return true;
            if (desc1.is_copied > desc2.is_copied)
                return false;
            if (desc1.is_damaged < desc2.is_damaged)
                return true;
            if (desc1.is_damaged > desc2.is_damaged)
                return false;
            return desc1.name.string() > desc2.name.string();
        });
        break;
    case Sorting::NameAsc:
        std::sort(begin(files_descriptions), end(files_descriptions), [&](const auto& desc1, const auto& desc2)
        {
            if (desc1.is_copied < desc2.is_copied)
                return true;
            if (desc1.is_copied > desc2.is_copied)
                return false;
            if (desc1.is_damaged < desc2.is_damaged)
                return true;
            if (desc1.is_damaged > desc2.is_damaged)
                return false;
            return desc1.name.string() < desc2.name.string();
        });
        break;
    case Sorting::Random:
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::shuffle(begin(files_descriptions), end(files_descriptions), gen);
    }
    break;
    }
}

FilesDescriptions load_or_collect_files_descriptions(
    Sorting sorting,
    const fs::path& files_descriptions_file,
    const fs::path& source_dir)
{
    auto files_descriptions = load_files_descriptions(files_descriptions_file);
    if (files_descriptions.empty())
    {
        files_descriptions = collect_files_descriptions(source_dir);
        save_files_descriptions(files_descriptions_file, files_descriptions);
    }
    setup_sorting(sorting, files_descriptions);
    return files_descriptions;
}

void save_files_descriptions(
    const fs::path& files_descriptions_file,
    const FilesDescriptions& files_descriptions)
{
    std::cout << "Saving files descriptions\n";

    std::ofstream ofs(files_descriptions_file.string());
    if (ofs)
    {
        for (const auto& file_description : files_descriptions)
        {
            ofs << file_description;
        }
    }
}

void copy_file_in_this_thread(
    const fs::path& source_file,
    const fs::path& target_dir)
{
    stdnext::error_code ec;
    if (!fs::exists(target_dir))
    {
        std::cout << "Creating target directory: " << target_dir << "\n";
        fs::create_directories(target_dir, ec);
    }
    const auto target_file = target_dir / source_file.filename();
    std::cout << "Copying: " << source_file << " to: " << target_file << "\n";
    fs::copy_file(source_file, target_file, fs::copy_options_skip_existing, ec);
}

void copy_files(
    Mode mode,
    unsigned int max_errors_count,
    const fs::path& process_file,
    const fs::path& source_dir,
    const fs::path& target_dir,
    FilesDescriptions& files_descriptions)
{
    std::cout << "Copying files based on collected files descriptions\n";

    if (files_descriptions.empty())
        return;

    const auto copy_file_mode = [&]
    {
        switch (mode)
        {
        case Mode::Direct:
            return copy_file_in_this_thread_adapted;
        case Mode::Thread:
            return copy_file_in_separate_thread;
        case Mode::Process:
            return copy_file_in_separate_process;
        }
        return copy_file_in_this_thread_adapted;
    }();

    unsigned int errors_count = 0;
    for (auto& file_description : files_descriptions)
    {
        const auto source_file = source_dir / file_description.name;
        if (!file_description.is_damaged && !file_description.is_copied)
        {
            const auto timeout = infer_timeout_from_file_size(file_description);
            file_description.is_copied = copy_file_mode(process_file, timeout, source_file, target_dir);
            file_description.is_damaged = !file_description.is_copied;
            if (file_description.is_damaged)
                ++errors_count;
            if (errors_count >= max_errors_count)
            {
                std::cout << "More than " << max_errors_count << " errors - exiting\n";
                return;
            }
        }
    }
}
