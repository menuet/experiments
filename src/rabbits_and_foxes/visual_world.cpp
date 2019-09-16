
#include "visual_world.hpp"
#include "board.hpp"
#include "pieces_ranges.hpp"
#include "visual_app.hpp"
#include "visual_config.hpp"
#include <sdlxx/assets.hpp>

namespace raf { namespace raf_v1 { namespace visual {

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

}}} // namespace raf::raf_v1::visual

namespace raf { namespace raf_v2 { namespace visual {

    Piece piece(const World::VisualPiece& visual_piece,
                const World::PlayingBoard& playing_board)
    {
        return playing_board.board->pieces()[visual_piece.piece_index];
    }

    Point location(const World::VisualPiece& visual_piece,
                   const World::PlayingBoard& playing_board)
    {
        return playing_board.locations[visual_piece.piece_index];
    }

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
        auto current_board_size = board.size();
        if (current_board_size.w == 0 || current_board_size.h == 0)
            return;

        const auto& pieces_locations = board.initial_locations();

        using namespace raf_v2;

        auto pieces = merge_pieces_infos(board.pieces(), pieces_locations);

        const auto mushroom_texture = m_textures.find_asset("mushroom");
        auto visual_mushrooms =
            pieces | keep_mushrooms_only() |
            stdnext::ranges::views::transform([=](const auto& piece_info) {
                return VisualPiece{mushroom_texture, index_of(piece_info)};
            });

        auto visual_rabbits =
            pieces | keep_rabbits_only() |
            stdnext::ranges::views::transform([this](const auto& piece_info) {
                const auto texture = [&] {
                    std::string texture_key;
                    switch (piece_of(piece_info).color())
                    {
                    case RabbitFacet::Color::Grey:
                        texture_key = "grey-rabbit";
                        break;
                    case RabbitFacet::Color::Brown:
                        texture_key = "brown-rabbit";
                        break;
                    case RabbitFacet::Color::White:
                    default:
                        texture_key = "white-rabbit";
                        break;
                    }
                    return m_textures.find_asset(texture_key);
                }();
                return VisualPiece{texture, index_of(piece_info)};
            });

        auto visual_foxes =
            pieces | keep_foxes_only() |
            stdnext::ranges::views::transform([this](const auto& piece_info) {
                const auto texture = [&] {
                    std::string texture_key;
                    switch (piece_of(piece_info).orientation())
                    {
                    case FoxFacet::Orientation::Horizontal:
                        texture_key = piece_of(piece_info).direction() ==
                                              FoxFacet::Direction::Forward
                                          ? "hor-right-fox"
                                          : "hor-left-fox";
                        break;
                    case FoxFacet::Orientation::Vertical:
                    default:
                        texture_key = piece_of(piece_info).direction() ==
                                              FoxFacet::Direction::Forward
                                          ? "ver-down-fox"
                                          : "ver-up-fox";
                        break;
                    }
                    return m_textures.find_asset(texture_key);
                }();
                return VisualPiece{texture, index_of(piece_info)};
            });

        auto visual_pieces =
            stdnext::ranges::views::concat(visual_mushrooms, visual_rabbits,
                                           visual_foxes) |
            stdnext::ranges::to<std::vector>();

        m_playing_board = PlayingBoard{
            &board, pieces_locations, std::move(visual_pieces), nullptr, false};
    }

    void World::update(const Config& config, sdlxx::Renderer& renderer) {}

    void World::render(const Config& config, sdlxx::Renderer& renderer) const
    {
        sdlxx::render_texture(renderer, *m_board_texture);

        if (!m_playing_board.board)
            return;

        for (const auto& visual_piece : m_playing_board.visual_pieces)
        {
            const auto screen_location = logical_to_screen(
                location(visual_piece, m_playing_board), config);
            sdlxx::render_texture(renderer, *visual_piece.texture,
                                  screen_location);
        }

        if (m_playing_board.win)
        {
            auto visual_rabbits =
                m_playing_board.visual_pieces |
                stdnext::ranges::views::filter(
                    [this](const auto& visual_piece) {
                        return piece(visual_piece, m_playing_board).type() ==
                               Piece::Type::Rabbit;
                    });
            for (const auto& visual_rabbit : visual_rabbits)
            {
                const auto screen_rectangle = logical_to_screen(
                    Rectangle{location(visual_rabbit, m_playing_board),
                              piece(visual_rabbit, m_playing_board).size()},
                    config);
                sdlxx::draw_rectangle(renderer, screen_rectangle,
                                      sdlxx::ColorAlpha{0, 255, 0, 0});
            }
        }
        else if (m_playing_board.selected_visual_piece)
        {
            const auto screen_rectangle = logical_to_screen(
                Rectangle{location(*m_playing_board.selected_visual_piece,
                                   m_playing_board),
                          piece(*m_playing_board.selected_visual_piece,
                                m_playing_board)
                              .size()},
                config);
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

        const auto iter_visual_piece = std::find_if(
            m_playing_board.visual_pieces.begin(),
            m_playing_board.visual_pieces.end(),
            [&, this](const auto& visual_piece) {
                return location(visual_piece, m_playing_board) == mouse_cell;
            });

        if (iter_visual_piece != m_playing_board.visual_pieces.end())
        {
            auto& visual_piece = *iter_visual_piece;
            const auto is_selectable = [&, this] {
                switch (piece(visual_piece, m_playing_board).type())
                {
                case Piece::Type::Rabbit:
                case Piece::Type::Fox:
                    return true;
                }
                return false;
            }();
            if (is_selectable &&
                &visual_piece != m_playing_board.selected_visual_piece)
                m_playing_board.selected_visual_piece = &visual_piece;
            else
                m_playing_board.selected_visual_piece = nullptr;
            return;
        }

        if (!m_playing_board.selected_visual_piece)
            return;

        const auto can_move = m_playing_board.board->can_move_piece(
            m_playing_board.locations,
            m_playing_board.selected_visual_piece->piece_index, mouse_cell);
        if (!can_move)
            return;

        m_playing_board
            .locations[m_playing_board.selected_visual_piece->piece_index] =
            mouse_cell;

        m_playing_board.win = m_playing_board.board->all_rabbits_in_hole(
            m_playing_board.locations);
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

}}} // namespace raf::raf_v2::visual
