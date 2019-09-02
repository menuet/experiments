
#include "world.hpp"
#include <sdlxx/assets.hpp>
#include <sdlxx/sounds.hpp>
#include <sdlxx/sprites.hpp>
#include <sdlxx/texts.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <chrono>
#include <random>

constexpr std::chrono::milliseconds ANIMATION_DELAY{100};
constexpr sdlxx::Size ANIMATION_STEP{10, 10};
constexpr int FONT_SIZE = 40;

static std::random_device random_device;
static std::mt19937 random_generator(random_device());

class Forest
{
public:
    Forest(const sdlxx::Renderer& renderer, sdlxx::Texture&& texture,
           sdlxx::Font&& font) noexcept
        : m_texture{std::move(texture)}, m_font(std::move(font))
    {
        miam_obelix(renderer);
        miam_asterix(renderer);
    }

    void render(const sdlxx::Renderer& renderer) const noexcept
    {
        sdlxx::render_texture(renderer, m_texture);
        sdlxx::render_texture(
            renderer, m_miams_obelix_texture,
            sdlxx::Point{SCREEN_COORDINATES.size().w() -
                             sdlxx::get_size(m_miams_obelix_texture).w() -
                             FONT_SIZE,
                         FONT_SIZE});
        sdlxx::render_texture(renderer, m_miams_asterix_texture,
                              sdlxx::Point{FONT_SIZE, FONT_SIZE});
    }

    void miam_obelix(const sdlxx::Renderer& renderer) noexcept
    {
        ++m_miams_obelix;
        const auto miams_string = fmt::format("Obelix: {}", m_miams_obelix);
        m_miams_obelix_texture = sdlxx::create_text_texture(
            renderer, m_font, miams_string.c_str(),
            sdlxx::ColorAlpha{255, 255, 255, 255});
    }

    void miam_asterix(const sdlxx::Renderer& renderer) noexcept
    {
        ++m_miams_asterix;
        const auto miams_string = fmt::format("Asterix: {}", m_miams_asterix);
        m_miams_asterix_texture = sdlxx::create_text_texture(
            renderer, m_font, miams_string.c_str(),
            sdlxx::ColorAlpha{255, 255, 255, 255});
    }

private:
    sdlxx::Texture m_texture;
    sdlxx::Font m_font;
    int m_miams_obelix{-1};
    sdlxx::Texture m_miams_obelix_texture;
    int m_miams_asterix{-1};
    sdlxx::Texture m_miams_asterix_texture;
};

class Boars
{
public:
    Boars(sdlxx::Texture&& texture, std::size_t boars_count) noexcept
        : m_texture{std::move(texture)}, m_size{sdlxx::get_size(m_texture)}
    {
        std::uniform_int_distribution<> x_distrib(
            0, SCREEN_COORDINATES.size().w() - m_size.w());
        std::uniform_int_distribution<> y_distrib(
            0, SCREEN_COORDINATES.size().h() - m_size.h());
        for (std::size_t boar_index = 0; boar_index < boars_count; ++boar_index)
            m_boars_locations.push_back(sdlxx::Point{
                x_distrib(random_generator), y_distrib(random_generator)});
    }

    void render(const sdlxx::Renderer& renderer) const noexcept
    {
        for (const auto& boar_location : m_boars_locations)
            sdlxx::render_texture(renderer, m_texture, boar_location);
    }

    bool are_colliding(const sdlxx::Rectangle& other) noexcept
    {
        const auto collided_boar_iter =
            std::find_if(begin(m_boars_locations), end(m_boars_locations),
                         [&](const auto& location) {
                             return sdlxx::are_colliding(
                                 sdlxx::Rectangle{location, m_size}, other);
                         });
        if (collided_boar_iter == end(m_boars_locations))
            return false;
        m_boars_locations.erase(collided_boar_iter);
        return true;
    }

    std::size_t boars_count() const noexcept
    {
        return m_boars_locations.size();
    }

private:
    sdlxx::Texture m_forest_texture;
    sdlxx::Texture m_texture;
    sdlxx::Size m_size;
    std::vector<sdlxx::Point> m_boars_locations;
};

class Obelix
{
public:
    Obelix(sdlxx::Texture&& texture, sdlxx::Chunk&& miam_chunk) noexcept
        : m_texture{std::move(texture)}, m_miam_chunk{std::move(miam_chunk)},
          m_size{sdlxx::get_size(this->m_texture)}
    {
    }

    void move() noexcept
    {
        const auto next_location =
            sdlxx::Point(m_location.x() + m_direction.w() * ANIMATION_STEP.w(),
                         m_location.y() + m_direction.h() * ANIMATION_STEP.h());

        bool ok_to_move = true;
        if (next_location.x() < 0 ||
            next_location.x() + m_size.w() > SCREEN_COORDINATES.size().w())
        {
            m_direction = sdlxx::Size(-m_direction.w(), m_direction.h());
            ok_to_move = false;
        }
        if (next_location.y() < 0 ||
            next_location.y() + m_size.h() > SCREEN_COORDINATES.size().h())
        {
            m_direction = sdlxx::Size(m_direction.w(), -m_direction.h());
            ok_to_move = false;
        }

        if (ok_to_move)
            m_location = next_location;
    }

    void render(const sdlxx::Renderer& renderer) const noexcept
    {
        sdlxx::render_texture(renderer, m_texture, m_location);
    }

    sdlxx::Rectangle rectangle() const noexcept
    {
        return sdlxx::Rectangle{m_location, m_size};
    }

    void miam() noexcept { sdlxx::play(m_miam_chunk); }

private:
    sdlxx::Texture m_texture;
    sdlxx::Chunk m_miam_chunk;
    sdlxx::Size m_size;
    sdlxx::Point m_location{3 * SCREEN_COORDINATES.size().w() / 4,
                            3 * SCREEN_COORDINATES.size().h() / 4};
    sdlxx::Size m_direction{-1, -1};
};

class Asterix
{
public:
    Asterix(sdlxx::SpriteSheet&& sprite_sheet, sdlxx::Chunk&& miam_chunk,
            sdlxx::Chunk&& aie_chunk) noexcept
        : m_sprite_sheet{std::move(sprite_sheet)}, m_miam_chunk{std::move(
                                                       miam_chunk)},
          m_aie_chunk{std::move(aie_chunk)}, m_sprite{m_sprite_sheet,
                                                      sdlxx::SpriteIndex(0, 0)}
    {
    }

    sdlxx::Size direction() const noexcept
    {
        int w = 0;
        if (static_cast<int>(m_keys) & static_cast<int>(Keys::Left))
            w -= 1;
        if (static_cast<int>(m_keys) & static_cast<int>(Keys::Right))
            w += 1;
        int h = 0;
        if (static_cast<int>(m_keys) & static_cast<int>(Keys::Up))
            h -= 1;
        if (static_cast<int>(m_keys) & static_cast<int>(Keys::Down))
            h += 1;
        return sdlxx::Size(w, h);
    }

    void move() noexcept
    {
        const auto sprite_size = m_sprite_sheet.sprite_size();
        const auto next_location =
            sdlxx::Point(m_location.x() + direction().w() * ANIMATION_STEP.w(),
                         m_location.y() + direction().h() * ANIMATION_STEP.h());
        if (next_location.x() >= 0 &&
            next_location.x() + sprite_size.w() <=
                SCREEN_COORDINATES.size().w() &&
            next_location.y() >= 0 &&
            next_location.y() + sprite_size.h() <=
                SCREEN_COORDINATES.size().h())
            m_location = next_location;

        m_sprite = m_sprite.next();
    }

    void render(const sdlxx::Renderer& renderer) const noexcept
    {
        m_sprite.render(renderer, m_location);
    }

    void set_key(Keys key) noexcept
    {
        m_keys =
            static_cast<Keys>(static_cast<int>(m_keys) | static_cast<int>(key));
    }

    void unset_key(Keys key) noexcept
    {
        m_keys = static_cast<Keys>(static_cast<int>(m_keys) &
                                   ~static_cast<int>(key));
    }

    sdlxx::Rectangle rectangle() const noexcept
    {
        return sdlxx::Rectangle{m_location, m_sprite_sheet.sprite_size()};
    }

    void miam() noexcept { sdlxx::play(m_miam_chunk); }

    void aie() noexcept { sdlxx::play(m_aie_chunk); }

private:
    sdlxx::SpriteSheet m_sprite_sheet;
    sdlxx::Chunk m_miam_chunk;
    sdlxx::Chunk m_aie_chunk;
    sdlxx::Sprite m_sprite;
    sdlxx::Point m_location{SCREEN_COORDINATES.size().w() / 4,
                            SCREEN_COORDINATES.size().h() / 4};
    Keys m_keys{Keys::None};
};

World::World(std::unique_ptr<Forest>&& forest, std::unique_ptr<Boars>&& boars,
             std::unique_ptr<Obelix>&& obelix,
             std::unique_ptr<Asterix>&& asterix)
    : m_forest(std::move(forest)), m_boars(std::move(boars)),
      m_obelix(std::move(obelix)), m_asterix(std::move(asterix))
{
}

World::World(World&&) = default;

World::~World() = default;

void World::set_key(Keys key) noexcept { m_asterix->set_key(key); }

void World::unset_key(Keys key) noexcept { m_asterix->unset_key(key); }

void World::move(const sdlxx::Renderer& renderer) noexcept
{
    if (m_end_of_game)
        return;

    const auto current_frame_time = std::chrono::system_clock::now();
    if (current_frame_time < m_previous_frame_time + ANIMATION_DELAY)
        return;

    m_obelix->move();
    m_asterix->move();

    if (m_boars->are_colliding(m_obelix->rectangle()))
    {
        m_obelix->miam();
        m_forest->miam_obelix(renderer);
    }
    if (m_boars->are_colliding(m_asterix->rectangle()))
    {
        m_asterix->miam();
        m_forest->miam_asterix(renderer);
    }

    const auto obelix_touched_asterix =
        sdlxx::are_colliding(m_obelix->rectangle(), m_asterix->rectangle());
    if (obelix_touched_asterix)
        m_asterix->aie();
    m_end_of_game = obelix_touched_asterix || m_boars->boars_count() == 0;

    m_previous_frame_time = current_frame_time;
}

void World::render(const sdlxx::Renderer& renderer) const noexcept
{
    m_forest->render(renderer);
    m_boars->render(renderer);
    m_obelix->render(renderer);
    m_asterix->render(renderer);
}

bout::result<World, stdnext::error_code>
load_world(const sdlxx::Renderer& renderer) noexcept
{
    auto forest_texture =
        sdlxx::load_texture(renderer, sdlxx::get_asset_path("Forest.bmp"));
    if (!forest_texture)
        return stdnext::make_error_code(stdnext::errc::invalid_argument);

    auto forest_font =
        sdlxx::load_font(sdlxx::get_asset_path("leadcoat.ttf"), FONT_SIZE);
    if (!forest_font)
        return stdnext::make_error_code(stdnext::errc::invalid_argument);

    auto forest = std::make_unique<Forest>(renderer, std::move(forest_texture),
                                           std::move(forest_font));

    auto boars_texture =
        sdlxx::load_texture(renderer, sdlxx::get_asset_path("Boar.Sprite.bmp"),
                            sdlxx::Color{52, 80, 225});
    if (!boars_texture)
        return stdnext::make_error_code(stdnext::errc::invalid_argument);

    auto boars = std::make_unique<Boars>(std::move(boars_texture), 10);

    auto obelix_texture = sdlxx::load_texture(
        renderer, sdlxx::get_asset_path("Obelix.Sprite.bmp"),
        sdlxx::Color{252, 254, 252});
    if (!obelix_texture)
        return stdnext::make_error_code(stdnext::errc::invalid_argument);

    auto obelix_miam_chunk =
        sdlxx::load_wav(sdlxx::get_asset_path("Obelix.Miam.wav"));
    if (!obelix_miam_chunk)
        return stdnext::make_error_code(stdnext::errc::invalid_argument);

    auto obelix = std::make_unique<Obelix>(std::move(obelix_texture),
                                           std::move(obelix_miam_chunk));

    auto asterix_sprite_sheet = sdlxx::load_spritesheet(
        renderer, sdlxx::get_asset_path("Asterix.Sprites.bmp"),
        sdlxx::Color{255, 200, 0}, sdlxx::Size{48, 64});
    if (!asterix_sprite_sheet)
        return stdnext::make_error_code(stdnext::errc::invalid_argument);

    auto asterix_miam_chunk =
        sdlxx::load_wav(sdlxx::get_asset_path("Asterix.Miam.wav"));
    if (!asterix_miam_chunk)
        return stdnext::make_error_code(stdnext::errc::invalid_argument);

    auto asterix_aie_chunk =
        sdlxx::load_wav(sdlxx::get_asset_path("Asterix.Aie.wav"));
    if (!asterix_aie_chunk)
        return stdnext::make_error_code(stdnext::errc::invalid_argument);

    auto asterix = std::make_unique<Asterix>(
        std::move(asterix_sprite_sheet.value()), std::move(asterix_miam_chunk),
        std::move(asterix_aie_chunk));

    return World(std::move(forest), std::move(boars), std::move(obelix),
                 std::move(asterix));
}
