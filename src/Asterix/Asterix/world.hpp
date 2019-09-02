
#pragma once

#include <sdlxx/graphics.hpp>
#include <sdlxx/outcome_disabled_warnings.hpp>
#include <platform/system_error.hpp>

namespace bout = BOOST_OUTCOME_V2_NAMESPACE;

constexpr sdlxx::Rectangle SCREEN_COORDINATES(sdlxx::Point(100, 100),
                                              sdlxx::Size(960, 600));

enum class Keys
{
    None = 0,
    Up = 0x01,
    Down = 0x02,
    Left = 0x04,
    Right = 0x08,
};

class Forest;
class Boars;
class Obelix;
class Asterix;

class World
{
public:

    World(std::unique_ptr<Forest>&& forest, std::unique_ptr<Boars>&& boars,
          std::unique_ptr<Obelix>&& obelix,
          std::unique_ptr<Asterix>&& asterix);

    World(World&&);

    ~World();

    bool load(const sdlxx::Renderer& renderer) noexcept;

    void set_key(Keys key) noexcept;

    void unset_key(Keys key) noexcept;

    void move(const sdlxx::Renderer& renderer) noexcept;

    void render(const sdlxx::Renderer& renderer) const noexcept;

private:
    std::unique_ptr<Forest> m_forest;
    std::unique_ptr<Boars> m_boars;
    std::unique_ptr<Obelix> m_obelix;
    std::unique_ptr<Asterix> m_asterix;
    std::chrono::system_clock::time_point m_previous_frame_time =
        std::chrono::system_clock::now();
    bool m_end_of_game{false};
};

bout::result<World, stdnext::error_code>
load_world(const sdlxx::Renderer& renderer) noexcept;
