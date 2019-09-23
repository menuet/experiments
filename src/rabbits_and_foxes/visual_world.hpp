
#pragma once

#include "board.hpp"
#include "pieces.hpp"
#include "visual_views.hpp"
#include <map>
#include <memory>

namespace raf { namespace raf_v1 { namespace visual {

    class Config;

    class World
    {
    public:
        World(sdlxx::Repository<sdlxx::Texture>&& textures);

        void play_board(const Config& config, const std::string& board_name);

        void on_click(const Config& config, const sdlxx::Renderer& renderer,
                      Uint8 button, sdlxx::Point mouse_location);

        void on_key(const Config& config, const sdlxx::Renderer& renderer,
                    const SDL_Keysym& keysym)
        {
        }

        void update(const Config& config, sdlxx::Renderer& renderer);

        void render(const Config& config, sdlxx::Renderer& renderer) const;

    private:
        class Piece
        {
        public:
            const sdlxx::Texture* texture;
            stdnext::variant<Mushroom, Rabbit, Fox> var_piece;

            auto location() const noexcept
            {
                return stdnext::visit(
                    [](const auto& piece) { return piece.location(); },
                    var_piece);
            }

            auto size() const noexcept
            {
                return stdnext::visit(
                    [](const auto& piece) { return piece.size(); }, var_piece);
            }

            auto can_move_to(Point location, const BoardLandscape& landscape,
                             const BoardState& state) const noexcept
            {
                return stdnext::visit(
                    [&](const auto& piece) {
                        return piece.can_move_to(location, landscape, state);
                    },
                    var_piece);
            }

            auto is_selectable() const noexcept
            {
                return var_piece.index() != 0;
            }
        };

        struct PlayingBoard
        {
            const Board* board{};
            BoardState state{Rabbits{}, Foxes{}};
            std::vector<Piece> pieces{};
            Piece* selected_piece{};
            bool win{};
        };

        sdlxx::Repository<sdlxx::Texture> m_textures;
        const sdlxx::Texture* m_board_texture;
        PlayingBoard m_playing_board;
    };

    sdlxx::result<World> load_world(const Config& config,
                                    const sdlxx::Renderer& renderer);

}}} // namespace raf::raf_v1::visual

namespace raf { namespace raf_v2 { namespace visual {

    class Config;

    class World
    {
    public:
        World(const Config& config, const sdlxx::Renderer& renderer,
              std::unique_ptr<Assets>&& assets);

        void on_click(const Config& config, const sdlxx::Renderer& renderer,
                      Uint8 button, sdlxx::Point mouse_location);

        void on_key(const Config& config, const sdlxx::Renderer& renderer,
                    const SDL_Keysym& keysym);

        void update(const Config& config, const sdlxx::Renderer& renderer);

        void render(const Config& config,
                    const sdlxx::Renderer& renderer) const;

    private:
        std::unique_ptr<Assets> m_assets;
        views::View m_view;
    };

    sdlxx::result<World> load_world(const Config& config,
                                    const sdlxx::Renderer& renderer);

}}} // namespace raf::raf_v2::visual
