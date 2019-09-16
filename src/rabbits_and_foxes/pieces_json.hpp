
#pragma once

#include "pieces.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace raf { namespace raf_v1 {

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
        const auto location = j.at("location").get<raf::raf_v1::Point>();
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

}} // namespace raf::raf_v1

namespace raf { namespace raf_v2 {

    inline void to_json(json& j, Piece::Type p)
    {
        switch (p)
        {
        case Piece::Type::Hole:
            j = "Hole";
            break;
        case Piece::Type::Mushroom:
            j = "Mushroom";
            break;
        case Piece::Type::Rabbit:
            j = "Rabbit";
            break;
        case Piece::Type::Fox:
            j = "Fox";
            break;
        }
    }

    inline void from_json(const json& j, Piece::Type& p)
    {
        const auto text = j.get<std::string>();
        if (text == "Hole")
            p = Piece::Type::Hole;
        else if (text == "Mushroom")
            p = Piece::Type::Mushroom;
        else if (text == "Rabbit")
            p = Piece::Type::Rabbit;
        else if (text == "Fox")
            p = Piece::Type::Fox;
        else
            throw std::runtime_error("Unknown piece type");
    }

    inline void to_json(json& j, RabbitFacet::Color p)
    {
        switch (p)
        {
        case RabbitFacet::Color::Grey:
            j = "Grey";
            break;
        case RabbitFacet::Color::White:
            j = "White";
            break;
        case RabbitFacet::Color::Brown:
            j = "Brown";
            break;
        }
    }

    inline void from_json(const json& j, RabbitFacet::Color& p)
    {
        const auto text = j.get<std::string>();
        if (text == "Grey")
            p = RabbitFacet::Color::Grey;
        else if (text == "White")
            p = RabbitFacet::Color::White;
        else if (text == "Brown")
            p = RabbitFacet::Color::Brown;
        else
            throw std::runtime_error("Unknown rabbit color");
    }

    inline void to_json(json& j, FoxFacet::Orientation p)
    {
        switch (p)
        {
        case FoxFacet::Orientation::Horizontal:
            j = "Horizontal";
            break;
        case FoxFacet::Orientation::Vertical:
            j = "Vertical";
            break;
        }
    }

    inline void from_json(const json& j, FoxFacet::Orientation& p)
    {
        const auto text = j.get<std::string>();
        if (text == "Horizontal")
            p = FoxFacet::Orientation::Horizontal;
        else if (text == "Vertical")
            p = FoxFacet::Orientation::Vertical;
        else
            throw std::runtime_error("Unknown fox orientation");
    }

    inline void to_json(json& j, FoxFacet::Direction p)
    {
        switch (p)
        {
        case FoxFacet::Direction::Forward:
            j = "Forward";
            break;
        case FoxFacet::Direction::Backward:
            j = "Backward";
            break;
        }
    }

    inline void from_json(const json& j, FoxFacet::Direction& p)
    {
        const auto text = j.get<std::string>();
        if (text == "Forward")
            p = FoxFacet::Direction::Forward;
        else if (text == "Backward")
            p = FoxFacet::Direction::Backward;
        else
            throw std::runtime_error("Unknown fox direction");
    }

    template <typename T>
    T json_to(const json& j);

    template <>
    inline HoleFacet json_to<HoleFacet>(const json& j)
    {
        return HoleFacet{};
    }

    template <>
    inline MushroomFacet json_to<MushroomFacet>(const json& j)
    {
        return MushroomFacet{};
    }

    template <>
    inline RabbitFacet json_to<RabbitFacet>(const json& j)
    {
        const auto color = j.at("color").get<RabbitFacet::Color>();
        return RabbitFacet{color};
    }

    template <>
    inline FoxFacet json_to<FoxFacet>(const json& j)
    {
        const auto orientation =
            j.at("orientation").get<FoxFacet::Orientation>();
        const auto direction = j.at("direction").get<FoxFacet::Direction>();
        return FoxFacet{orientation, direction};
    }

    template <>
    inline PieceAndLocation json_to<PieceAndLocation>(const json& j)
    {
        const auto type = j.at("type").get<Piece::Type>();
        const auto location = j.get<Point>();
        switch (type)
        {
        case Piece::Type::Hole:
            return {Piece{json_to<HoleFacet>(j)}, location};
        case Piece::Type::Mushroom:
            return {Piece{json_to<MushroomFacet>(j)}, location};
        case Piece::Type::Rabbit:
            return {Piece{json_to<RabbitFacet>(j)}, location};
        case Piece::Type::Fox:
        default:
            return {Piece{json_to<FoxFacet>(j)}, location};
        }
    }

    inline void json_from(json& j, const HoleFacet&) {}

    inline void json_from(json& j, const MushroomFacet&) {}

    inline void json_from(json& j, const RabbitFacet& facet)
    {
        j["color"] = facet.color();
    }

    inline void json_from(json& j, const FoxFacet& facet)
    {
        j["orientation"] = facet.orientation();
        j["direction"] = facet.direction();
    }

    inline void json_from(json& j, const PieceAndLocation& piece_location)
    {
        j["type"] = piece_location.first.type();
        piece_location.first.visit(
            [&](const auto& facet) { return json_from(j, facet); });
        j["x"] = piece_location.second.x;
        j["y"] = piece_location.second.y;
    }

}} // namespace raf::raf_v2

namespace nlohmann {

    template <>
    struct adl_serializer<raf::raf_v1::Hole>
    {
        static raf::raf_v1::Hole from_json(const json& j)
        {
            return raf::raf_v1::json_to_piece<raf::raf_v1::Hole>(j);
        }

        static void to_json(json& j, const raf::raf_v1::Hole& piece)
        {
            return raf::raf_v1::piece_to_json(j, piece);
        }
    };

    template <>
    struct adl_serializer<raf::raf_v1::Mushroom>
    {
        static raf::raf_v1::Mushroom from_json(const json& j)
        {
            return raf::raf_v1::json_to_piece<raf::raf_v1::Mushroom>(j);
        }

        static void to_json(json& j, const raf::raf_v1::Mushroom& piece)
        {
            return raf::raf_v1::piece_to_json(j, piece);
        }
    };

    template <>
    struct adl_serializer<raf::raf_v1::Rabbit>
    {
        static raf::raf_v1::Rabbit from_json(const json& j)
        {
            return raf::raf_v1::json_to_piece<raf::raf_v1::Rabbit>(j);
        }

        static void to_json(json& j, const raf::raf_v1::Rabbit& piece)
        {
            return raf::raf_v1::piece_to_json(j, piece);
        }
    };

    template <>
    struct adl_serializer<raf::raf_v1::Fox>
    {
        static raf::raf_v1::Fox from_json(const json& j)
        {
            return raf::raf_v1::json_to_piece<raf::raf_v1::Fox>(j);
        }

        static void to_json(json& j, const raf::raf_v1::Fox& piece)
        {
            return raf::raf_v1::piece_to_json(j, piece);
        }
    };

    template <>
    struct adl_serializer<raf::raf_v2::PieceAndLocation>
    {
        static raf::raf_v2::PieceAndLocation from_json(const json& j)
        {
            return raf::raf_v2::json_to<raf::raf_v2::PieceAndLocation>(j);
        }

        static void to_json(json& j, const raf::raf_v2::PieceAndLocation& piece)
        {
            return raf::raf_v2::json_from(j, piece);
        }
    };

} // namespace nlohmann
