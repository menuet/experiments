
#include "visual_world.hpp"
#include "board.hpp"
#include "visual_app.hpp"
#include "visual_config.hpp"
#include <sdlxx/assets.hpp>

namespace raf { namespace visual {

    World::World(sdlxx::Repository<sdlxx::Texture>&& textures)
        : m_textures{std::move(textures)}, m_board_texture{
                                               m_textures.find_asset("board")}
    {
    }

    void World::play_board(const Config& config, const std::string& board_name)
    {
        m_playing_board = PlayingBoard{};

        const auto iter_board = config.boards.find("27");
        if (iter_board == config.boards.end())
            return;

        const auto& board = iter_board->second;
        auto current_board_size = board.landscape().size();
        if (current_board_size.w == 0 || current_board_size.h == 0)
            return;

        const auto& state = board.state();

        std::vector<Piece> pieces;

        {
            const auto texture = m_textures.find_asset("mushroom");
            for (const auto& mushroom : board.landscape().mushrooms())
                pieces.push_back({texture, mushroom});
        }

        for (const auto& rabbit : state.rabbits())
        {
            const auto texture = [&] {
                std::string texture_key;
                switch (rabbit.color())
                {
                case RabbitColor::Grey:
                    texture_key = "grey-rabbit";
                    break;
                case RabbitColor::Brown:
                    texture_key = "brown-rabbit";
                    break;
                case RabbitColor::White:
                default:
                    texture_key = "white-rabbit";
                    break;
                }
                return m_textures.find_asset(texture_key);
            }();
            pieces.push_back({texture, rabbit});
        }

        for (const auto& fox : state.foxes())
        {
            const auto texture = [&] {
                std::string texture_key;
                switch (fox.orientation())
                {
                case FoxOrientation::Horizontal:
                    texture_key = fox.direction() == FoxDirection::Forward
                                      ? "hor-right-fox"
                                      : "hor-left-fox";
                    break;
                case FoxOrientation::Vertical:
                default:
                    texture_key = fox.direction() == FoxDirection::Forward
                                      ? "ver-down-fox"
                                      : "ver-up-fox";
                    break;
                }
                return m_textures.find_asset(texture_key);
            }();
            pieces.push_back({texture, fox});
        }

        m_playing_board =
            PlayingBoard{&board, state, std::move(pieces), nullptr, false};
    }

    void World::update(const Config& config, sdlxx::Renderer& renderer) {}

    void World::render(const Config& config, sdlxx::Renderer& renderer) const
    {
        sdlxx::render_texture(renderer, *m_board_texture);

        if (!m_playing_board.board)
            return;

        for (const auto& piece : m_playing_board.pieces)
        {
            const auto screen_location =
                logical_to_screen<sdlxx::Point>(piece, config);
            sdlxx::render_texture(renderer, *piece.texture, screen_location);
        }

        if (m_playing_board.win)
        {
            for (const auto& rabbit : m_playing_board.state.rabbits())
            {
                const auto screen_rectangle =
                    logical_to_screen<sdlxx::Rectangle>(rabbit, config);
                sdlxx::draw_rectangle(renderer, screen_rectangle,
                                      sdlxx::ColorAlpha{0, 255, 0, 0});
            }
        }
        else if (m_playing_board.selected_piece)
        {
            const auto screen_rectangle = logical_to_screen<sdlxx::Rectangle>(
                *m_playing_board.selected_piece, config);
            sdlxx::draw_rectangle(renderer, screen_rectangle,
                                  sdlxx::ColorAlpha{255, 0, 0, 0});
        }
    }

    void World::on_click(const Config& config, sdlxx::Point mouse_location)
    {
        if (!m_playing_board.board)
            return;
        if (m_playing_board.win)
            return;

        const auto mouse_cell = screen_to_logical(mouse_location, config);

        const auto iter_piece = std::find_if(
            m_playing_board.pieces.begin(), m_playing_board.pieces.end(),
            [&](const auto& piece) { return piece.location() == mouse_cell; });

        if (iter_piece != m_playing_board.pieces.end())
        {
            auto& piece = *iter_piece;
            if (piece.is_selectable() &&
                &piece != m_playing_board.selected_piece)
                m_playing_board.selected_piece = &piece;
            else
                m_playing_board.selected_piece = nullptr;
            return;
        }

        if (!m_playing_board.selected_piece)
            return;

        const auto can_move = m_playing_board.selected_piece->can_move_to(
            mouse_cell, m_playing_board.board->landscape(),
            m_playing_board.state);
        if (!can_move)
            return;

        const auto iter_rabbit = std::find_if(
            m_playing_board.state.rabbits().begin(),
            m_playing_board.state.rabbits().end(), [&](const auto& rabbit) {
                return rabbit.location() ==
                       m_playing_board.selected_piece->location();
            });
        if (iter_rabbit != m_playing_board.state.rabbits().end())
        {
            const auto index =
                iter_rabbit - m_playing_board.state.rabbits().begin();
            m_playing_board.state =
                m_playing_board.state.move_to<Rabbit>(index, mouse_cell);
            m_playing_board.selected_piece->var_piece =
                m_playing_board.state.rabbits()[index];
        }
        else
        {
            const auto iter_fox = std::find_if(
                m_playing_board.state.foxes().begin(),
                m_playing_board.state.foxes().end(), [&](const auto& fox) {
                    return fox.location() ==
                           m_playing_board.selected_piece->location();
                });
            if (iter_fox != m_playing_board.state.foxes().end())
            {
                const auto index =
                    iter_fox - m_playing_board.state.foxes().begin();
                m_playing_board.state =
                    m_playing_board.state.move_to<Fox>(index, mouse_cell);
                m_playing_board.selected_piece->var_piece =
                    m_playing_board.state.foxes()[index];
            }
        }

        m_playing_board.win = m_playing_board.state.all_rabbits_in_hole(
            m_playing_board.board->landscape());
    }

    sdlxx::result<World> load_world(const sdlxx::Renderer& renderer)
    {
        try
        {
            sdlxx::Repository<sdlxx::Texture> textures;
            const auto& assets_path = sdlxx::get_assets_path();
            for (const auto& entry :
                 stdnext::filesystem::directory_iterator(assets_path))
            {
                const auto& entry_path = entry.path();
                if (stdnext::filesystem::is_regular_file(entry_path) &&
                    entry_path.extension() == ".png")
                {
                    BOOST_OUTCOME_TRY(
                        texture, sdlxx::load_texture(renderer, entry_path));
                    textures.insert_asset(entry_path.stem().string(),
                                          std::move(texture));
                }
            }
            return World(std::move(textures));
        }
        catch (...)
        {
            return stdnext::make_error_code(stdnext::errc::bad_file_descriptor);
        }
    }

}} // namespace raf::visual
