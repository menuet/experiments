
#pragma once

#include <platform/filesystem.hpp>
#include <stdexcept>

enum class Mode
{
    Direct,
    Thread,
    Process,
    Slave,
};

enum class Sorting
{
    Default,
    NameDesc,
    NameAsc,
    Random,
};

struct Options
{
    Mode mode{};
    Sorting sorting{};
    stdnext::filesystem::path source_dir{};
    stdnext::filesystem::path source_file{};
    stdnext::filesystem::path target_dir{};
    stdnext::filesystem::path files_descriptions_file{};
    unsigned int max_errors_count{};
};

struct ParsingError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

Options parse_command_line(int argc, char* argv[]);
