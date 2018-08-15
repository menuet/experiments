
#pragma once

#include "options.hpp"
#include <platform/filesystem.hpp>
#include <vector>

struct FileDescription
{
    stdnext::filesystem::path name{};
    bool is_damaged{};
    std::uintmax_t size{};
    bool is_copied{};
};

using FilesDescriptions = std::vector<FileDescription>;

FilesDescriptions load_or_collect_files_descriptions(
    Sorting sorting,
    const stdnext::filesystem::path& files_descriptions_file,
    const stdnext::filesystem::path& source_dir);

void save_files_descriptions(
    const stdnext::filesystem::path& files_descriptions_file,
    const FilesDescriptions& files_descriptions);

void copy_file_in_this_thread(
    const stdnext::filesystem::path& source_file,
    const stdnext::filesystem::path& target_dir);

void copy_files(
    Mode mode,
    unsigned int max_errors_count,
    const stdnext::filesystem::path& process_file,
    const stdnext::filesystem::path& source_dir,
    const stdnext::filesystem::path& target_dir,
    FilesDescriptions& files_descriptions);
