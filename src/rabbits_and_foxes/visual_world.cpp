
#include "visual_world.hpp"
#include "board.hpp"
#include "visual_app.hpp"
#include <sdlxx/assets.hpp>

namespace raf { namespace visual {

    constexpr sdlxx::SpriteIndex GreyRabbitIndex{0, 0};
    constexpr sdlxx::SpriteIndex BrownRabbitIndex{1, 0};
    constexpr sdlxx::SpriteIndex WhiteRabbitIndex{2, 0};
    constexpr sdlxx::SpriteIndex HorFoxIndex{0, 1};
    constexpr sdlxx::SpriteIndex VerFoxIndex{1, 1};
    constexpr sdlxx::SpriteIndex MushroomIndex{2, 1};

    template <typename PieceT>
    static auto sdl_rectangle(const PieceT& piece, sdlxx::Size cell_size) noexcept
    {
        const auto lo = piece.location();
        const auto si = piece.size();
        return sdlxx::Rectangle{{lo.x * cell_size.w(), lo.y * cell_size.h()},
                                {si.w * cell_size.w(), si.h * cell_size.h()}};
    }

    World::World(sdlxx::Texture&& board_texture,
                 sdlxx::SpriteSheet&& pieces_sheet)
        : m_board_texture{std::move(board_texture)}, m_pieces_sheet{std::move(
                                                         pieces_sheet)}
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

        const auto cell_size =
            sdlxx::Size{SCREEN_SIZE.w() / current_board_size.w,
                        SCREEN_SIZE.h() / current_board_size.h};
        if (cell_size.w() == 0 || cell_size.h() == 0)
            return;

        const auto& state = board.state();

        std::vector<Piece> pieces;

        for (const auto& mushroom : board.landscape().mushrooms())
            pieces.push_back({MushroomIndex, mushroom});

        for (const auto& rabbit : state.rabbits())
        {
            const auto sprite_index = [&] {
                switch (rabbit.color())
                {
                case RabbitColor::Grey:
                    return GreyRabbitIndex;
                case RabbitColor::Brown:
                    return BrownRabbitIndex;
                case RabbitColor::White:
                    return WhiteRabbitIndex;
                }
                return WhiteRabbitIndex;
            }();
            pieces.push_back({sprite_index, rabbit});
        }

        for (const auto& fox : state.foxes())
        {
            const auto sprite_index = [&] {
                switch (fox.orientation())
                {
                case FoxOrientation::Horizontal:
                    return HorFoxIndex;
                case FoxOrientation::Vertical:
                    return VerFoxIndex;
                }
                return VerFoxIndex;
            }();
            pieces.push_back({sprite_index, fox});
        }

        m_playing_board =
            PlayingBoard{&board, cell_size, state, std::move(pieces), nullptr};
    }

    void World::update(const Config& config, sdlxx::Renderer& renderer) {}

    void World::render(const Config& config, sdlxx::Renderer& renderer) const
    {
        sdlxx::render_texture(renderer, m_board_texture);

        if (!m_playing_board.board)
            return;

        for (const auto& piece : m_playing_board.pieces)
        {
            sdlxx::Sprite{m_pieces_sheet, piece.sprite_index}.render(
                renderer, sdl_rectangle(piece, m_playing_board.cell_size));
        }

        if (m_playing_board.win)
        {
            for (const auto& rabbit : m_playing_board.state.rabbits())
                sdlxx::draw_rectangle(renderer, sdl_rectangle(rabbit, m_playing_board.cell_size),
                                      sdlxx::ColorAlpha{0, 255, 0, 0});
        }
        else if (m_playing_board.selected_piece)
            sdlxx::draw_rectangle(renderer,
                sdl_rectangle(
                    *m_playing_board.selected_piece,
                                      m_playing_board.cell_size),
                                  sdlxx::ColorAlpha{255, 0, 0, 0});
    }

    void World::on_click(const Config& config, sdlxx::Point mouse_location)
    {
        if (!m_playing_board.board)
            return;
        if (m_playing_board.win)
            return;

        Point mouse_cell{mouse_location.x() / m_playing_board.cell_size.w(),
                         mouse_location.y() / m_playing_board.cell_size.h()};

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

    std::unique_ptr<World> load_world(const sdlxx::Renderer& renderer)
    {
        auto board_texture =
            sdlxx::load_texture(renderer, sdlxx::get_asset_path("board.bmp"));
        if (!board_texture)
            return nullptr;

        auto pieces_sheet = sdlxx::load_spritesheet(
            renderer, sdlxx::get_asset_path("pieces.png"),
            sdlxx::Color{0, 0, 0}, sdlxx::Size{256, 256});
        if (!pieces_sheet)
            return nullptr;

        return std::make_unique<World>(
            std::move(board_texture),
            std::move(std::move(pieces_sheet.value())));
        return nullptr;
    }

}} // namespace raf::visual
