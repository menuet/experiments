
#include "visual_config.hpp"
#include "board_json.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

namespace nlohmann {

    template <>
    struct adl_serializer<sdlxx::Size>
    {
        static sdlxx::Size from_json(const json& j)
        {
            return sdlxx::Size{j.at("w").get<int>(), j.at("h").get<int>()};
        }

        static void to_json(json& j, const sdlxx::Size& size)
        {
            j["w"] = size.w();
            j["h"] = size.h();
        }
    };

    template <>
    struct adl_serializer<raf::raf_v1::visual::Config>
    {
        static raf::raf_v1::visual::Config from_json(const json& j)
        {
            return raf::raf_v1::visual::Config{
                j.at("board_size").get<sdlxx::Size>(),
                j.at("border_size").get<sdlxx::Size>(),
                j.at("cell_size").get<sdlxx::Size>(),
                j.at("boards")
                    .get<std::map<std::string, raf::raf_v1::Board>>()};
        }

        static void to_json(json& j, const raf::raf_v1::visual::Config& config)
        {
            j["board_size"] = config.board_size;
            j["border_size"] = config.border_size;
            j["cell_size"] = config.cell_size;
            j["boards"] = config.boards;
        }
    };

    template <>
    struct adl_serializer<raf::raf_v2::visual::Config>
    {
        static raf::raf_v2::visual::Config from_json(const json& j)
        {
            return raf::raf_v2::visual::Config{
                j.at("board_size").get<sdlxx::Size>(),
                j.at("border_size").get<sdlxx::Size>(),
                j.at("cell_size").get<sdlxx::Size>(),
                j.at("boards")
                    .get<std::map<std::string, raf::raf_v2::Board>>()};
        }

        static void to_json(json& j, const raf::raf_v2::visual::Config& config)
        {
            j["board_size"] = config.board_size;
            j["border_size"] = config.border_size;
            j["cell_size"] = config.cell_size;
            j["boards"] = config.boards;
        }
    };

} // namespace nlohmann

namespace raf { namespace raf_v1 { namespace visual {

    sdlxx::result<Config>
    load_config(const stdnext::filesystem::path& config_file_path)
    {
        std::ifstream config_is(config_file_path.string().c_str());
        try
        {
            if (!config_is)
                return stdnext::make_error_code(
                    stdnext::errc::invalid_argument);
            const auto config_as_json = json::parse(config_is);
            auto config = config_as_json.get<Config>();
            return config;
        }
        catch (std::exception&)
        {
            return stdnext::make_error_code(stdnext::errc::invalid_argument);
        }
    }

}}} // namespace raf::raf_v1::visual

namespace raf { namespace raf_v2 { namespace visual {

    sdlxx::result<Config>
    load_config(const stdnext::filesystem::path& config_file_path)
    {
        std::ifstream config_is(config_file_path.string().c_str());
        try
        {
            if (!config_is)
                return stdnext::make_error_code(
                    stdnext::errc::invalid_argument);
            const auto config_as_json = json::parse(config_is);
            auto config = config_as_json.get<Config>();
            return config;
        }
        catch (std::exception&)
        {
            return stdnext::make_error_code(stdnext::errc::invalid_argument);
        }
    }

}}} // namespace raf::raf_v2::visual
