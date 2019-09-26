
#include "visual_views.hpp"
#include "board.hpp"
#include "pieces_ranges.hpp"
#include "visual_config.hpp"
#include <optional>
#include <sdlxx/texts.hpp>

namespace raf { namespace raf_v2 { namespace visual { namespace views {

    static Piece::Type type_of(const VisualPiece& visual_piece,
                               const Board& board)
    {
        return board.pieces()[visual_piece.piece_index].type();
    }

    static Size size_of(const VisualPiece& visual_piece, const Board& board)
    {
        return board.pieces()[visual_piece.piece_index].size();
    }

    static Point location_of(const VisualPiece& visual_piece,
                             const Points& locations)
    {
        return locations[visual_piece.piece_index];
    }

    static sdlxx::Size offset_of(const Config& config,
                                 const std::string& texture_key)
    {
        const auto iter = config.offsets.find(texture_key);
        if (iter == config.offsets.end())
            return sdlxx::Size{0, 0};
        return iter->second;
    }

    static std::optional<VisualBoard>
    create_board(const Config& config, const std::string& board_name,
                 const Board& board, const Assets& assets)
    {
        auto board_size = board.size();
        if (board_size.w == 0 || board_size.h == 0)
            return std::nullopt;

        const auto& pieces_locations = board.initial_locations();

        using namespace raf_v2;

        auto pieces = merge_pieces_infos(board.pieces(), pieces_locations);

        const std::string mushroom_texture_key = "mushroom";
        const auto mushroom_texture =
            assets.textures.find_asset(mushroom_texture_key);
        const auto mushroom_offset = offset_of(config, mushroom_texture_key);
        auto visual_mushrooms =
            pieces | keep_mushrooms_only() |
            stdnext::ranges::views::transform([=](const auto& piece_info) {
                return VisualPiece{mushroom_texture, mushroom_offset,
                                   index_of(piece_info)};
            });

        auto visual_rabbits =
            pieces | keep_rabbits_only() |
            stdnext::ranges::views::transform([&](const auto& piece_info) {
                const auto [texture, offset] = [&] {
                    std::string texture_key;
                    switch (piece_of(piece_info).color())
                    {
                    case RabbitFacet::Color::Grey:
                        texture_key = "rabbit-grey";
                        break;
                    case RabbitFacet::Color::Brown:
                        texture_key = "rabbit-brown";
                        break;
                    case RabbitFacet::Color::White:
                    default:
                        texture_key = "rabbit-white";
                        break;
                    }
                    return std::pair{assets.textures.find_asset(texture_key),
                                     offset_of(config, texture_key)};
                }();
                return VisualPiece{texture, offset, index_of(piece_info)};
            });

        auto visual_foxes =
            pieces | keep_foxes_only() |
            stdnext::ranges::views::transform([&](const auto& piece_info) {
                const auto [texture, offset] = [&] {
                    std::string texture_key;
                    switch (piece_of(piece_info).orientation())
                    {
                    case FoxFacet::Orientation::Horizontal:
                        if (piece_of(piece_info).direction() ==
                            FoxFacet::Direction::Forward)
                            texture_key = "fox-hor-forward";
                        else
                            texture_key = "fox-hor-backward";
                        break;
                    case FoxFacet::Orientation::Vertical:
                    default:
                        if (piece_of(piece_info).direction() ==
                            FoxFacet::Direction::Forward)
                            texture_key = "fox-ver-forward";
                        else
                            texture_key = "fox-ver-backward";
                        break;
                    }
                    return std::pair{assets.textures.find_asset(texture_key),
                                     offset_of(config, texture_key)};
                }();
                return VisualPiece{texture, offset, index_of(piece_info)};
            });

        auto visual_pieces =
            stdnext::ranges::views::concat(visual_mushrooms, visual_rabbits,
                                           visual_foxes) |
            stdnext::ranges::to<std::vector>();

        return VisualBoard{board_name, &board, pieces_locations,
                           std::move(visual_pieces)};
    }

    static void render_board(const Config& config,
                             const sdlxx::Renderer& renderer,
                             const Assets& assets,
                             const VisualBoard& visual_board)
    {
        sdlxx::render_texture(renderer, *assets.board_texture);

        for (const auto& visual_piece : visual_board.visual_pieces)
        {
            const auto screen_location = logical_to_screen(
                location_of(visual_piece, visual_board.locations), config);
            const sdlxx::Point offsetted_screen_location{
                screen_location.x() + visual_piece.offset.w(),
                screen_location.y() + visual_piece.offset.h()};
            sdlxx::render_texture(renderer, *visual_piece.texture,
                                  offsetted_screen_location);
        }
    }

    template <typename ViewT, typename... Ts>
    static View create_view(const Config& config,
                            const sdlxx::Renderer& renderer,
                            const Assets& assets, Ts&&... ts)
    {
        auto view_opt =
            ViewT::create(config, renderer, assets, std::forward<Ts>(ts)...);
        if (!view_opt)
            return View{Empty{}};
        return View{std::move(view_opt.value())};
    }

    static std::string prev_board_name(const Config& config,
                                       const std::string& board_name)
    {
        auto iter_board = config.boards.find(board_name);
        if (iter_board == config.boards.end())
        {
            iter_board = config.boards.begin();
            if (iter_board == config.boards.end())
                return std::string();
            return iter_board->first;
        }
        if (iter_board == config.boards.begin())
            iter_board = config.boards.end();
        --iter_board;
        return iter_board->first;
    }

    static std::string next_board_name(const Config& config,
                                       const std::string& board_name)
    {
        auto iter_board = config.boards.find(board_name);
        if (iter_board == config.boards.end())
        {
            iter_board = config.boards.begin();
            if (iter_board == config.boards.end())
                return std::string();
            return iter_board->first;
        }
        if (iter_board != config.boards.end())
        {
            ++iter_board;
            if (iter_board == config.boards.end())
                iter_board = config.boards.begin();
        }
        return iter_board->first;
    }

    View create_initial_view(const Config& config,
                             const sdlxx::Renderer& renderer,
                             const Assets& assets)
    {
        return create_view<Choose>(config, renderer, assets,
                                   next_board_name(config, std::string()));
    }

    std::optional<Choose> Choose::create(const Config& config,
                                         const sdlxx::Renderer& renderer,
                                         const Assets& assets,
                                         const std::string& board_name)
    {
        const auto iter_board = config.boards.find(board_name);
        if (iter_board == config.boards.end())
            return std::nullopt;

        auto name_texture_or_error = sdlxx::create_text_texture(
            renderer, assets.font, board_name.c_str(), {255, 255, 255, 255});
        if (!name_texture_or_error)
            return std::nullopt;

        auto visual_board_opt =
            create_board(config, board_name, iter_board->second, assets);
        if (!visual_board_opt)
            return std::nullopt;

        return Choose{std::move(visual_board_opt.value()),
                      std::move(name_texture_or_error.value())};
    }

    void Choose::render(const Config& config, const sdlxx::Renderer& renderer,
                        const Assets& assets) const
    {
        assert(m_visual_board.board);

        render_board(config, renderer, assets, m_visual_board);

        const auto name_size = sdlxx::get_size(m_name_texture);
        sdlxx::render_texture(renderer, m_name_texture,
                              sdlxx::Point{
                                  config.board_size.w() / 2 - name_size.w() / 2,
                                  config.board_size.h() / 2 - name_size.h() / 2,
                              });
    }

    Transition Choose::on_click(const Config& config,
                                const sdlxx::Renderer& renderer,
                                const Assets& assets, Uint8 button,
                                sdlxx::Point mouse_location)
    {
        switch (button)
        {
        case SDL_BUTTON_LEFT:
            return [&, board_name = m_visual_board.board_name] {
                return create_view<Play>(config, renderer, assets, board_name);
            };
        case SDL_BUTTON_RIGHT:
            return [&, board_name =
                           next_board_name(config, m_visual_board.board_name)] {
                return create_view<Choose>(config, renderer, assets,
                                           board_name);
            };
        default:
            return Transition{};
        }
    }

    Transition Choose::on_key(const Config& config,
                              const sdlxx::Renderer& renderer,
                              const Assets& assets, const SDL_Keysym& keysym)
    {
        switch (keysym.sym)
        {
        case SDLK_LEFT:
            return [&, board_name =
                           prev_board_name(config, m_visual_board.board_name)] {
                return create_view<Choose>(config, renderer, assets,
                                           board_name);
            };
        case SDLK_RIGHT:
            return [&, board_name =
                           next_board_name(config, m_visual_board.board_name)] {
                return create_view<Choose>(config, renderer, assets,
                                           board_name);
            };
        case SDLK_RETURN:
            return [&, board_name = m_visual_board.board_name] {
                return create_view<Play>(config, renderer, assets, board_name);
            };
        case SDLK_s:
            return [&, board_name = m_visual_board.board_name] {
                return create_view<Solve>(config, renderer, assets, board_name);
            };
        default:
            return Transition{};
        }
    }

    std::optional<Play> Play::create(const Config& config,
                                     const sdlxx::Renderer& renderer,
                                     const Assets& assets,
                                     const std::string& board_name)
    {
        const auto iter_board = config.boards.find(board_name);
        if (iter_board == config.boards.end())
            return std::nullopt;

        auto visual_board_opt =
            create_board(config, board_name, iter_board->second, assets);
        if (!visual_board_opt)
            return std::nullopt;

        return Play{std::move(visual_board_opt.value())};
    }

    void Play::render(const Config& config, const sdlxx::Renderer& renderer,
                      const Assets& assets) const
    {
        assert(m_visual_board.board);

        render_board(config, renderer, assets, m_visual_board);

        if (m_win)
        {
            auto visual_rabbits =
                m_visual_board.visual_pieces |
                stdnext::ranges::views::filter(
                    [this](const auto& visual_piece) {
                        return type_of(visual_piece, *m_visual_board.board) ==
                               Piece::Type::Rabbit;
                    });
            for (const auto& visual_rabbit : visual_rabbits)
            {
                const auto screen_rectangle = logical_to_screen(
                    Rectangle{
                        location_of(visual_rabbit, m_visual_board.locations),
                        size_of(visual_rabbit, *m_visual_board.board)},
                    config);
                sdlxx::draw_rectangle(renderer, screen_rectangle,
                                      sdlxx::ColorAlpha{0, 255, 0, 0});
            }
        }
        else if (m_selected_visual_piece)
        {
            const auto screen_rectangle = logical_to_screen(
                Rectangle{
                    location_of(*m_selected_visual_piece,
                                m_visual_board.locations),
                    size_of(*m_selected_visual_piece, *m_visual_board.board)},
                config);
            sdlxx::draw_rectangle(renderer, screen_rectangle,
                                  sdlxx::ColorAlpha{255, 0, 0, 0});
        }
    }

    Transition Play::on_click(const Config& config,
                              const sdlxx::Renderer& renderer,
                              const Assets& assets, Uint8 button,
                              sdlxx::Point mouse_location)
    {
        assert(m_visual_board.board);

        if (m_win)
            return [&, board_name =
                           next_board_name(config, m_visual_board.board_name)] {
                return create_view<Choose>(config, renderer, assets,
                                           board_name);
            };

        switch (button)
        {
        case SDL_BUTTON_LEFT:
            break;
        case SDL_BUTTON_RIGHT:
            m_selected_visual_piece = nullptr;
            return Transition{};
        default:
            return Transition{};
        }

        const auto mouse_cell = screen_to_logical(mouse_location, config);

        const auto iter_visual_piece = std::find_if(
            m_visual_board.visual_pieces.begin(),
            m_visual_board.visual_pieces.end(),
            [&, this](const auto& visual_piece) {
                return location_of(visual_piece, m_visual_board.locations) ==
                       mouse_cell;
            });

        if (iter_visual_piece != m_visual_board.visual_pieces.end())
        {
            auto& visual_piece = *iter_visual_piece;
            const auto is_selectable = [&, this] {
                switch (type_of(visual_piece, *m_visual_board.board))
                {
                case Piece::Type::Rabbit:
                case Piece::Type::Fox:
                    return true;
                default:
                    break;
                }
                return false;
            }();
            if (is_selectable && &visual_piece != m_selected_visual_piece)
                m_selected_visual_piece = &visual_piece;
            else
                m_selected_visual_piece = nullptr;
            return Transition{};
        }

        if (!m_selected_visual_piece)
            return Transition{};

        const auto can_move = m_visual_board.board->can_move_piece(
            m_visual_board.locations, m_selected_visual_piece->piece_index,
            mouse_cell);
        if (!can_move)
            return Transition{};

        m_visual_board.locations[m_selected_visual_piece->piece_index] =
            mouse_cell;

        m_win =
            m_visual_board.board->all_rabbits_in_hole(m_visual_board.locations);

        return Transition{};
    }

    Transition Play::on_key(const Config& config,
                            const sdlxx::Renderer& renderer,
                            const Assets& assets, const SDL_Keysym& keysym)
    {
        switch (keysym.sym)
        {
        case SDLK_RETURN:
            return [&, board_name =
                           next_board_name(config, m_visual_board.board_name)] {
                return create_view<Choose>(config, renderer, assets,
                                           board_name);
            };
        case SDLK_ESCAPE:
            return [&, board_name = m_visual_board.board_name] {
                return create_view<Choose>(config, renderer, assets,
                                           board_name);
            };
        default:
            return Transition{};
        }
    }

    std::optional<Solve> Solve::create(const Config& config,
                                       const sdlxx::Renderer& renderer,
                                       const Assets& assets,
                                       const std::string& board_name)
    {
        const auto iter_board = config.boards.find(board_name);
        if (iter_board == config.boards.end())
            return std::nullopt;

        auto visual_board_opt =
            create_board(config, board_name, iter_board->second, assets);
        if (!visual_board_opt)
            return std::nullopt;

        return Solve{std::move(visual_board_opt.value())};
    }

    void Solve::render(const Config& config, const sdlxx::Renderer& renderer,
                       const Assets& assets) const
    {
        assert(m_visual_board.board);

        render_board(config, renderer, assets, m_visual_board);

        if (m_next_move == m_solver_moves.end())
        {
            auto visual_rabbits =
                m_visual_board.visual_pieces |
                stdnext::ranges::views::filter(
                    [this](const auto& visual_piece) {
                        return type_of(visual_piece, *m_visual_board.board) ==
                               Piece::Type::Rabbit;
                    });
            for (const auto& visual_rabbit : visual_rabbits)
            {
                const auto screen_rectangle = logical_to_screen(
                    Rectangle{
                        location_of(visual_rabbit, m_visual_board.locations),
                        size_of(visual_rabbit, *m_visual_board.board)},
                    config);
                sdlxx::draw_rectangle(renderer, screen_rectangle,
                                      sdlxx::ColorAlpha{0, 255, 0, 0});
            }
        }
        else
        {
            const auto screen_rectangle = logical_to_screen(
                Rectangle{
                    m_visual_board.locations[m_next_move->piece_index],
                    m_visual_board.board->pieces()[m_next_move->piece_index]
                        .size()},
                config);
            sdlxx::draw_rectangle(renderer, screen_rectangle,
                                  sdlxx::ColorAlpha{255, 0, 0, 0});
        }
    }

    Transition Solve::on_click(const Config& config,
                               const sdlxx::Renderer& renderer,
                               const Assets& assets, Uint8 button,
                               sdlxx::Point mouse_location)
    {
        if (m_next_move == m_solver_moves.end())
            return Transition{};
        m_visual_board.locations[m_next_move->piece_index] =
            m_next_move->location;
        ++m_next_move;
        return Transition{};
    }

    Transition Solve::on_key(const Config& config,
                             const sdlxx::Renderer& renderer,
                             const Assets& assets, const SDL_Keysym& keysym)
    {
        switch (keysym.sym)
        {
        case SDLK_RETURN:
            if (m_next_move == m_solver_moves.end())
                return Transition{};
            m_visual_board.locations[m_next_move->piece_index] =
                m_next_move->location;
            ++m_next_move;
            break;
        case SDLK_ESCAPE:
            return [&, board_name = m_visual_board.board_name] {
                return create_view<Choose>(config, renderer, assets,
                                           board_name);
            };
        default:
            break;
        }
        return Transition{};
    }

}}}} // namespace raf::raf_v2::visual::views
