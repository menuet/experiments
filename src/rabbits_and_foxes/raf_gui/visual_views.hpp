
#pragma once

#include "geometry.hpp"
#include "solver.hpp"
#include "visual_config.hpp"
#include <functional>
#include <map>
#include <platform/variant.hpp>
#include <sdlxx/assets.hpp>
#include <sdlxx/graphics.hpp>
#include <tuple>
#include <vector>

namespace raf { namespace raf_v2 {
    class Board;
}} // namespace raf::raf_v2

namespace raf { namespace raf_v2 { namespace visual { namespace views {

    using Boards = std::map<std::string, Board>;

    class Empty;
    class Choose;
    class Play;
    class Solve;
    using View = stdnext::variant<Empty, Choose, Play, Solve>;

    using Transition = std::function<View()>;

    struct VisualPiece
    {
        const sdlxx::Texture* texture{};
        sdlxx::Size offset{0, 0};
        std::size_t piece_index{};
    };

    using VisualPieces = std::vector<VisualPiece>;

    struct VisualBoard
    {
        std::string board_name;
        const Board* board{};
        Points locations{};
        VisualPieces visual_pieces{};
    };

    class Empty
    {
    public:
        void update(const Config& config, const sdlxx::Renderer& renderer,
                    const Assets& assets)
        {
        }

        void render(const Config& config, const sdlxx::Renderer& renderer,
                    const Assets& assets) const
        {
        }

        Transition on_click(const Config& config,
                            const sdlxx::Renderer& renderer,
                            const Assets& assets, Uint8 button,
                            sdlxx::Point mouse_location)
        {
            return Transition{};
        }

        Transition on_key(const Config& config, const sdlxx::Renderer& renderer,
                          const Assets& assets, const SDL_Keysym& keysym)
        {
            return Transition{};
        }
    };

    class Choose
    {
    public:
        Choose(VisualBoard&& visual_board, sdlxx::Texture&& name_texture)
            : m_visual_board{std::move(visual_board)}, m_name_texture{std::move(
                                                           name_texture)}
        {
        }

        static std::optional<Choose> create(const Config& config,
                                            const sdlxx::Renderer& renderer,
                                            const Assets& assets,
                                            const std::string& board_name);

        void update(const Config& config, const sdlxx::Renderer& renderer,
                    const Assets& assets)
        {
        }

        void render(const Config& config, const sdlxx::Renderer& renderer,
                    const Assets& assets) const;

        Transition on_click(const Config& config,
                            const sdlxx::Renderer& renderer,
                            const Assets& assets, Uint8 button,
                            sdlxx::Point mouse_location);

        Transition on_key(const Config& config, const sdlxx::Renderer& renderer,
                          const Assets& assets, const SDL_Keysym& keysym);

    private:
        VisualBoard m_visual_board{};
        sdlxx::Texture m_name_texture;
    };

    class Play
    {
    public:
        Play(VisualBoard&& visual_board)
            : m_visual_board{std::move(visual_board)}
        {
        }

        static std::optional<Play> create(const Config& config,
                                          const sdlxx::Renderer& renderer,
                                          const Assets& assets,
                                          const std::string& board_name);

        void update(const Config& config, const sdlxx::Renderer& renderer,
                    const Assets& assets)
        {
        }

        void render(const Config& config, const sdlxx::Renderer& renderer,
                    const Assets& assets) const;

        Transition on_click(const Config& config,
                            const sdlxx::Renderer& renderer,
                            const Assets& assets, Uint8 button,
                            sdlxx::Point mouse_location);

        Transition on_key(const Config& config, const sdlxx::Renderer& renderer,
                          const Assets& assets, const SDL_Keysym& keysym);

    private:
        VisualBoard m_visual_board{};
        VisualPiece* m_selected_visual_piece{};
        bool m_win{};
    };

    class Solve
    {
    public:
        Solve(VisualBoard&& visual_board)
            : m_visual_board{std::move(visual_board)},
              m_solver_graph{solve(*m_visual_board.board)},
              m_solver_moves{m_solver_graph.fastest_solution()},
              m_next_move{m_solver_moves.begin()}
        {
        }

        static std::optional<Solve> create(const Config& config,
                                           const sdlxx::Renderer& renderer,
                                           const Assets& assets,
                                           const std::string& board_name);

        void update(const Config& config, const sdlxx::Renderer& renderer,
                    const Assets& assets)
        {
        }

        void render(const Config& config, const sdlxx::Renderer& renderer,
                    const Assets& assets) const;

        Transition on_click(const Config& config,
                            const sdlxx::Renderer& renderer,
                            const Assets& assets, Uint8 button,
                            sdlxx::Point mouse_location);

        Transition on_key(const Config& config, const sdlxx::Renderer& renderer,
                          const Assets& assets, const SDL_Keysym& keysym);

    private:
        VisualBoard m_visual_board{};
        SolverGraph m_solver_graph;
        SolverMoves m_solver_moves;
        SolverMoves::const_iterator m_next_move;
    };

    View create_initial_view(const Config& config,
                             const sdlxx::Renderer& renderer,
                             const Assets& assets);

}}}} // namespace raf::raf_v2::visual::views
