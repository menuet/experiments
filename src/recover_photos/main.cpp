
#include "options.hpp"
#include "photos.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        const auto options = parse_command_line(argc, argv);

        if (options.mode == Mode::Slave)
        {
            copy_file_in_this_thread(options.source_file, options.target_dir);
            return 0;
        }

        auto files_descriptions = load_or_collect_files_descriptions(options.sorting, options.files_descriptions_file, options.source_dir);
        copy_files(options.mode, options.max_errors_count, argv[0], options.source_dir, options.target_dir, files_descriptions);
        save_files_descriptions(options.files_descriptions_file, files_descriptions);
    }
    catch (ParsingError& err)
    {
        std::cout << err.what();
        return -1;
    }

    return 0;
}
