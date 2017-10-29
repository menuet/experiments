
#pragma once


#include <sfml/graphics.hpp>
#include <chrono>


using namespace std::chrono_literals;


class Config {
public:

    static constexpr SizeU WindowSize{ 600, 400 };
    static constexpr SizeI CellSize{ 10, 10 };
    static constexpr SizeI GridSize{ static_cast<int>(WindowSize.w / CellSize.w), static_cast<int>(WindowSize.h / CellSize.h) };
    static constexpr PointI SnakeStartingPoint{ GridSize.w / 2, GridSize.h / 2 };
    static constexpr SizeI SnakeStartingDir{ 0, 1 };
    static constexpr unsigned int SnakeStartingLength = 40;
    static constexpr std::chrono::milliseconds TikTakInterval = 200ms;
};

inline sf::Vector2f toScreen(PointI point) {
    return { static_cast<float>(point.x * Config::CellSize.w), static_cast<float>(point.y * Config::CellSize.h) };
}

inline sf::Vector2f toScreen(SizeI size) {
    return { static_cast<float>(size.w * Config::CellSize.w), static_cast<float>(size.h * Config::CellSize.h) };
}
