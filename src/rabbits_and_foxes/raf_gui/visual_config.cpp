
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
    struct adl_serializer<raf::raf_v2::visual::Config::Server>
    {
        static raf::raf_v2::visual::Config::Server from_json(const json& j)
        {
            return raf::raf_v2::visual::Config::Server{
                j.at("address").get<std::string>(),
                j.at("port").get<std::string>()};
        }

        static void to_json(json& j,
                            const raf::raf_v2::visual::Config::Server& server)
        {
            j["address"] = server.address;
            j["port"] = server.port;
        }
    };

    template <>
    struct adl_serializer<raf::raf_v2::visual::Config>
    {
        static raf::raf_v2::visual::Config from_json(const json& j)
        {
            return raf::raf_v2::visual::Config{
                j.at("server").get<raf::raf_v2::visual::Config::Server>(),
                j.at("board_size").get<sdlxx::Size>(),
                j.at("border_size").get<sdlxx::Size>(),
                j.at("cell_size").get<sdlxx::Size>(),
                j.at("offsets").get<raf::raf_v2::visual::Offsets>(),
                j.at("boards").get<raf::raf_v2::visual::Boards>()};
        }

        static void to_json(json& j, const raf::raf_v2::visual::Config& config)
        {
            j["server"] = config.server;
            j["board_size"] = config.board_size;
            j["border_size"] = config.border_size;
            j["cell_size"] = config.cell_size;
            j["offsets"] = config.offsets;
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
            return config_as_json.get<Config>();
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
            return config_as_json.get<Config>();
        }
        catch (std::exception&)
        {
            return stdnext::make_error_code(stdnext::errc::invalid_argument);
        }
    }

    sdlxx::result<std::unique_ptr<Assets>>
    load_assets(const sdlxx::Renderer& renderer,
                const stdnext::filesystem::path& assets_file_path)
    {
        BOOST_OUTCOME_TRY(textures, sdlxx::load_assets<sdlxx::Texture>(
                                        assets_file_path, ".png", renderer));

        const auto board_texture = textures.find_asset("board");
        if (!board_texture)
            return stdnext::make_error_code(stdnext::errc::invalid_argument);

        BOOST_OUTCOME_TRY(
            font, sdlxx::load_font(assets_file_path / "leadcoat.ttf", 50));

        return std::make_unique<Assets>(std::move(textures), *board_texture,
                                        std::move(font));
    }

}}} // namespace raf::raf_v2::visual
