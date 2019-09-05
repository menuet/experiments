
#pragma once

#include "pieces.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace raf {

    inline void to_json(json& j, RabbitColor p)
    {
        switch (p)
        {
        case RabbitColor::Grey:
            j = "Grey";
            break;
        case RabbitColor::White:
            j = "White";
            break;
        case RabbitColor::Brown:
            j = "Brown";
            break;
        }
    }

    inline void from_json(const json& j, RabbitColor& p)
    {
        const auto text = j.get<std::string>();
        if (text == "Grey")
            p = RabbitColor::Grey;
        else if (text == "White")
            p = RabbitColor::White;
        else if (text == "Brown")
            p = RabbitColor::Brown;
        else
            throw std::runtime_error("Unknown rabbit color");
    }

    inline void to_json(json& j, FoxOrientation p)
    {
        switch (p)
        {
        case FoxOrientation::Horizontal:
            j = "Horizontal";
            break;
        case FoxOrientation::Vertical:
            j = "Vertical";
            break;
        }
    }

    inline void from_json(const json& j, FoxOrientation& p)
    {
        const auto text = j.get<std::string>();
        if (text == "Horizontal")
            p = FoxOrientation::Horizontal;
        else if (text == "Vertical")
            p = FoxOrientation::Vertical;
        else
            throw std::runtime_error("Unknown fox orientation");
    }

    inline void to_json(json& j, FoxDirection p)
    {
        switch (p)
        {
        case FoxDirection::Forward:
            j = "Forward";
            break;
        case FoxDirection::Backward:
            j = "Backward";
            break;
        }
    }

    inline void from_json(const json& j, FoxDirection& p)
    {
        const auto text = j.get<std::string>();
        if (text == "Forward")
            p = FoxDirection::Forward;
        else if (text == "Backward")
            p = FoxDirection::Backward;
        else
            throw std::runtime_error("Unknown fox direction");
    }

    inline void to_json(json& j, const Point& p)
    {
        j = json{{"x", p.x}, {"y", p.y}};
    }

    inline void from_json(const json& j, Point& p)
    {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
    }

    inline void to_json(json& j, const Size& s)
    {
        j = json{{"w", s.w}, {"h", s.h}};
    }

    inline void from_json(const json& j, Size& s)
    {
        j.at("w").get_to(s.w);
        j.at("h").get_to(s.h);
    }

    template <typename PieceT>
    inline PieceT json_to_piece(const json& j)
    {
        const auto location = j.at("location").get<raf::Point>();
        return PieceT{location};
    }

    template <typename PieceT>
    inline void piece_to_json(json& j, const PieceT& piece)
    {
        j["location"] = piece.location();
    }

    template <>
    inline Rabbit json_to_piece<Rabbit>(const json& j)
    {
        const auto location = j.at("location").get<Point>();
        const auto color = j.at("color").get<RabbitColor>();
        return Rabbit{location, color};
    }

    template <>
    inline void piece_to_json(json& j, const Rabbit& piece)
    {
        j["location"] = piece.location();
        j["color"] = piece.color();
    }

    template <>
    inline Fox json_to_piece<Fox>(const json& j)
    {
        const auto location = j.at("location").get<Point>();
        const auto orientation = j.at("orientation").get<FoxOrientation>();
        const auto direction = j.at("direction").get<FoxDirection>();
        return Fox{location, orientation, direction};
    }

    template <>
    inline void piece_to_json(json& j, const Fox& piece)
    {
        j["location"] = piece.location();
        j["orientation"] = piece.orientation();
        j["direction"] = piece.direction();
    }

} // namespace raf

namespace nlohmann {

    template <>
    struct adl_serializer<raf::Hole>
    {
        static raf::Hole from_json(const json& j)
        {
            return raf::json_to_piece<raf::Hole>(j);
        }

        static void to_json(json& j, const raf::Hole& piece)
        {
            return raf::piece_to_json(j, piece);
        }
    };

    template <>
    struct adl_serializer<raf::Mushroom>
    {
        static raf::Mushroom from_json(const json& j)
        {
            return raf::json_to_piece<raf::Mushroom>(j);
        }

        static void to_json(json& j, const raf::Mushroom& piece)
        {
            return raf::piece_to_json(j, piece);
        }
    };

    template <>
    struct adl_serializer<raf::Rabbit>
    {
        static raf::Rabbit from_json(const json& j)
        {
            return raf::json_to_piece<raf::Rabbit>(j);
        }

        static void to_json(json& j, const raf::Rabbit& piece)
        {
            return raf::piece_to_json(j, piece);
        }
    };

    template <>
    struct adl_serializer<raf::Fox>
    {
        static raf::Fox from_json(const json& j)
        {
            return raf::json_to_piece<raf::Fox>(j);
        }

        static void to_json(json& j, const raf::Fox& piece)
        {
            return raf::piece_to_json(j, piece);
        }
    };

} // namespace nlohmann
