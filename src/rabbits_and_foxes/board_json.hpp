
#pragma once

#include "board.hpp"
#include "pieces_json.hpp"

namespace nlohmann {

    template <>
    struct adl_serializer<raf::raf_v1::BoardLandscape>
    {
        static raf::raf_v1::BoardLandscape from_json(const json& j)
        {
            const auto size = j.at("size").get<raf::raf_v1::Size>();
            auto holes = j.at("holes").get<raf::raf_v1::Holes>();
            auto mushrooms = j.at("mushrooms").get<raf::raf_v1::Mushrooms>();
            return raf::raf_v1::BoardLandscape{size, std::move(holes),
                                               std::move(mushrooms)};
        }

        static void to_json(json& j,
                            const raf::raf_v1::BoardLandscape& landscape)
        {
            j["size"] = landscape.size();
            j["holes"] = landscape.holes();
            j["mushrooms"] = landscape.mushrooms();
        }
    };

    template <>
    struct adl_serializer<raf::raf_v1::BoardState>
    {
        static raf::raf_v1::BoardState from_json(const json& j)
        {
            auto rabbits = j.at("rabbits").get<raf::raf_v1::Rabbits>();
            auto foxes = j.at("foxes").get<raf::raf_v1::Foxes>();
            return raf::raf_v1::BoardState{std::move(rabbits),
                                           std::move(foxes)};
        }

        static void to_json(json& j, const raf::raf_v1::BoardState& state)
        {
            j["rabbits"] = state.rabbits();
            j["foxes"] = state.foxes();
        }
    };

    template <>
    struct adl_serializer<raf::raf_v1::Board>
    {
        static raf::raf_v1::Board from_json(const json& j)
        {
            auto landscape =
                j.at("landscape").get<raf::raf_v1::BoardLandscape>();
            auto state = j.at("state").get<raf::raf_v1::BoardState>();
            return raf::raf_v1::Board{std::move(landscape), std::move(state)};
        }

        static void to_json(json& j, const raf::raf_v1::Board& board)
        {
            j["landscape"] = board.landscape();
            j["state"] = board.state();
        }
    };

    template <>
    struct adl_serializer<raf::raf_v2::Board>
    {
        static raf::raf_v2::Board from_json(const json& j)
        {
            auto size = j.at("size").get<raf::raf_v2::Size>();
            auto pieces_and_locations =
                j.at("pieces").get<raf::raf_v2::PiecesAndLocations>();
            return raf::raf_v2::Board{size, pieces_and_locations};
        }

        static void to_json(json& j, const raf::raf_v2::Board& board)
        {
            j["size"] = board.size();
            j["pieces"] = board.pieces_and_initial_locations();
        }
    };

} // namespace nlohmann
