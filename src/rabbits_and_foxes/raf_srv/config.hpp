
#pragma once

#include "board.hpp"
#include <memory>
#include <optional>
#include <platform/filesystem.hpp>
#include <string>
#include <vector>

namespace raf { namespace srv {

    struct BoardDef
    {
        std::string name;
        stdnext::filesystem::path def_file_path;
        std::optional<raf_v2::Board> board;
    };

    using BoardDefs = std::vector<BoardDef>;

    struct Level
    {
        std::string name;
        BoardDefs board_defs;
    };

    using Levels = std::vector<Level>;

    struct Config
    {
        Levels levels;
    };

    std::optional<Config>
    load_config(const stdnext::filesystem::path& config_file_path);

    std::string get_levels_json_string(const Config& config);

    std::optional<std::string>
    get_level_json_string(const Config& config, const std::string& level_name);

    std::optional<std::string>
    get_board_json_string(const Config& config, const std::string& level_name,
                          const std::string& board_name);

}} // namespace raf::srv
