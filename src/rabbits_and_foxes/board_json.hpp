
#pragma once

#include "board.hpp"
#include "pieces_json.hpp"

namespace nlohmann {

    template <>
    struct adl_serializer<raf::BoardLandscape>
    {
        static raf::BoardLandscape from_json(const json& j)
        {
            const auto size = j.at("size").get<raf::Size>();
            auto holes = j.at("holes").get<raf::Holes>();
            auto mushrooms = j.at("mushrooms").get<raf::Mushrooms>();
            return raf::BoardLandscape{size, std::move(holes),
                                       std::move(mushrooms)};
        }

        static void to_json(json& j, const raf::BoardLandscape& landscape)
        {
            j["size"] = landscape.size();
            j["holes"] = landscape.holes();
            j["mushrooms"] = landscape.mushrooms();
        }
    };

    template <>
    struct adl_serializer<raf::BoardState>
    {
        static raf::BoardState from_json(const json& j)
        {
            auto rabbits = j.at("rabbits").get<raf::Rabbits>();
            auto foxes = j.at("foxes").get<raf::Foxes>();
            return raf::BoardState{std::move(rabbits), std::move(foxes)};
        }

        static void to_json(json& j, const raf::BoardState& state)
        {
            j["rabbits"] = state.rabbits();
            j["foxes"] = state.foxes();
        }
    };

    template <>
    struct adl_serializer<raf::Board>
    {
        static raf::Board from_json(const json& j)
        {
            auto landscape = j.at("landscape").get<raf::BoardLandscape>();
            auto state = j.at("state").get<raf::BoardState>();
            return raf::Board{std::move(landscape), std::move(state)};
        }

        static void to_json(json& j, const raf::Board& board)
        {
            j["landscape"] = board.landscape();
            j["state"] = board.state();
        }
    };

} // namespace nlohmann
