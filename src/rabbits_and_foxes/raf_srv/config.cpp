
#include "config.hpp"
#include "board_json.hpp"
#include "pieces_json.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

namespace raf { namespace srv {

    using nlohmann::json;

    inline void to_json(json& j, const BoardDef& b)
    {
        j["name"] = b.name;
        j["file"] = b.def_file_path;
    }

    inline void from_json(const json& j, BoardDef& b)
    {
        b.name = j.at("name").get<std::string>();
        b.def_file_path = j.at("file").get<std::string>();
    }

    inline void to_json(json& j, const Level& l)
    {
        j["name"] = l.name;
        j["boards"] = l.board_defs;
    }

    inline void from_json(const json& j, Level& l)
    {
        l.name = j.at("name").get<std::string>();
        l.board_defs = j.at("boards").get<BoardDefs>();
    }

    inline void to_json(json& j, const Config& c) { j["levels"] = c.levels; }

    inline void from_json(const json& j, Config& c)
    {
        c.levels = j.at("levels").get<Levels>();
    }

    namespace {

        Config load_config_without_boards(
            const stdnext::filesystem::path& config_file_path)
        {
            std::ifstream config_ifs(config_file_path.string().c_str());
            if (!config_ifs)
                throw std::invalid_argument("bad config file path");
            const auto config_as_json = json::parse(config_ifs);
            return config_as_json.get<Config>();
        }

        void load_boards(const stdnext::filesystem::path& config_file_path,
                         Config& config)
        {
            const auto config_dir = config_file_path.parent_path();
            for (auto& level : config.levels)
            {
                for (auto& board_def : level.board_defs)
                {
                    const auto board_file_path =
                        config_dir / board_def.def_file_path;
                    std::ifstream board_ifs(board_file_path.string().c_str());
                    if (board_ifs)
                    {
                        try
                        {
                            const auto board_as_json = json::parse(board_ifs);
                            board_def.board =
                                board_as_json.get<raf_v2::Board>();
                        }
                        catch (std::exception&)
                        {
                        }
                    }
                }
            }
        }

        const Level* find_level(const Config& config,
                                const std::string& level_name)
        {
            const auto level_iter = std::find_if(
                config.levels.begin(), config.levels.end(),
                [&](const auto& level) { return level.name == level_name; });
            if (level_iter == config.levels.end())
                return {};

            const auto& level = *level_iter;
            return &level;
        }

        const BoardDef* find_board(const Config& config,
                                   const std::string& level_name,
                                   const std::string& board_name)
        {
            const auto level_opt = find_level(config, level_name);
            if (!level_opt)
                return {};
            const auto& level = *level_opt;

            const auto& board_defs = level.board_defs;
            const auto board_def_iter =
                std::find_if(board_defs.begin(), board_defs.end(),
                             [&](const auto& board_def) {
                                 return board_def.name == board_name;
                             });
            if (board_def_iter == board_defs.end())
                return {};

            const auto& board_def = *board_def_iter;
            return &board_def;
        }

    } // namespace

    std::optional<Config>
    load_config(const stdnext::filesystem::path& config_file_path)
    {
        try
        {
            auto config = load_config_without_boards(config_file_path);
            load_boards(config_file_path, config);
            return {std::move(config)};
        }
        catch (std::exception&)
        {
            return {};
        }
    }

    std::string get_levels_json_string(const Config& config)
    {
        const auto& levels = config.levels;

        json j;
        for (const auto& level : levels)
            j.push_back(level.name);
        return j.dump();
    }

    std::optional<std::string>
    get_level_json_string(const Config& config, const std::string& level_name)
    {
        const auto level_ptr = find_level(config, level_name);
        if (!level_ptr)
            return {};
        const auto& level = *level_ptr;

        const auto& boards = level.board_defs;

        json j;
        j["name"] = level.name;
        auto& boards_j = j["boards"];
        for (const auto& board : boards)
            boards_j.push_back(board.name);
        return j.dump();
    }

    std::optional<std::string>
    get_board_json_string(const Config& config, const std::string& level_name,
                          const std::string& board_name)
    {
        const auto board_def_ptr = find_board(config, level_name, board_name);
        if (!board_def_ptr)
            return {};
        const auto& board_def = *board_def_ptr;

        if (!board_def.board)
            return {};

        json j;
        j = *board_def.board;
        j["name"] = board_def.name;
        return j.dump();
    }

}} // namespace raf::srv
