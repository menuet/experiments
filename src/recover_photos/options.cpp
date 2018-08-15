
#include "options.hpp"
#include <boost/program_options.hpp>
#include <algorithm>
#include <iostream>
#include <cctype>

namespace bpop = boost::program_options;
namespace fs = stdnext::filesystem;

std::istream& operator>>(std::istream& in, Mode& mode)
{
    std::string token;
    in >> token;
    std::transform(begin(token), end(token), begin(token), [](const auto& c) { return (char)std::tolower(c); });
    if (token == "direct")
        mode = Mode::Direct;
    else if (token == "thread")
        mode = Mode::Thread;
    else if (token == "process")
        mode = Mode::Process;
    else if (token == "slave")
        mode = Mode::Slave;
    else
        in.setstate(std::ios_base::failbit);
    return in;
}

std::istream& operator>>(std::istream& in, Sorting& sorting)
{
    std::string token;
    in >> token;
    std::transform(begin(token), end(token), begin(token), [](const auto& c) { return (char)std::tolower(c); });
    if (token == "name_desc")
        sorting = Sorting::NameDesc;
    else if (token == "name_asc")
        sorting = Sorting::NameAsc;
    else if (token == "random")
        sorting = Sorting::Random;
    else if (token == "default")
        sorting = Sorting::Default;
    else
        in.setstate(std::ios_base::failbit);
    return in;
}

std::ostream& operator<<(std::ostream& out, Sorting sorting)
{
    switch (sorting)
    {
    case Sorting::NameDesc:
        out << "name_desc";
        break;
    case Sorting::NameAsc:
        out << "name_asc";
        break;
    case Sorting::Random:
        out << "random";
        break;
    default:
        out << "default";
        break;
    }
    return out;
}

Options parse_command_line(int argc, char* argv[])
{
    Options options;
    bpop::options_description desc{ "Options" };
    bpop::variables_map vm;

    const auto throw_parsing_error = [&](const char* error_text)
    {
        std::stringstream strs;
        if (error_text)
        {
            strs << "Error:\n";
            strs << error_text << "\n";
        }
        strs << "Usage:\n";
        strs << desc << "\n";
        throw ParsingError(strs.str());
    };

    try
    {
        desc.add_options()
            ("help", "Help screen")
            ("mode", bpop::value<Mode>(&options.mode)->required(), "Can be direct, thread or process")
            ("sorting", bpop::value<Sorting>(&options.sorting)->default_value(Sorting::Default), "Can be default, name_desc, name_asc or random")
            ("source_dir", bpop::value<fs::path>(&options.source_dir), "Directory where to copy the photos from")
            ("source_file", bpop::value<fs::path>(&options.source_file), "Full path name of file to copy")
            ("target_dir", bpop::value<fs::path>(&options.target_dir), "Directory where to copy the photos to")
            ("files_descriptions_file", bpop::value<fs::path>(&options.files_descriptions_file)->default_value(fs::path(argv[0]).parent_path() / "files_descriptions.txt"), "Full path name of file containing the source files descriptions")
            ("max_errors_count", bpop::value<unsigned int>(&options.max_errors_count)->default_value(50U), "Maximum errors count before stopping")
            ;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);
        switch (options.mode)
        {
        case Mode::Direct:
        case Mode::Thread:
        case Mode::Process:
            if (vm.count("source_dir") == 0)
                throw_parsing_error("missing source_dir");
            if (vm.count("target_dir") == 0)
                throw_parsing_error("missing target_dir");
            break;
        case Mode::Slave:
            if (vm.count("source_file") == 0)
                throw_parsing_error("missing source_file");
            if (vm.count("target_dir") == 0)
                throw_parsing_error("missing target_dir");
            break;
        }
    }
    catch (std::exception& err)
    {
        throw_parsing_error(err.what());
    }

    return options;
}
