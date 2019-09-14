
#pragma once

#include "board.hpp"
#include "geometry.hpp"
#include "pieces.hpp"
#include <map>
#include <memory>
#include <optional>
#include <platform/filesystem.hpp>
#include <platform/variant.hpp>
#include <sdlxx/assets.hpp>
#include <sdlxx/graphics.hpp>
#include <vector>

namespace raf {
    class Board;
}

namespace raf { namespace visual {

    class Config;

    class World
    {
    public:
        World(sdlxx::Repository<sdlxx::Texture>&& textures);

        void play_board(const Config& config, const std::string& board_name);

        void on_click(const Config& config, sdlxx::Point mouse_location);

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

    sdlxx::result<World> load_world(const sdlxx::Renderer& renderer);

}} // namespace raf::visual
