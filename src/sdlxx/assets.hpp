
#pragma once

#include "graphics.hpp"
#include "sdl_disabled_warnings.h"
#include <map>
#include <platform/filesystem.hpp>
#include <string>

namespace sdlxx {

    namespace fs = stdnext::filesystem;

    inline const fs::path& get_base_path()
    {
        static const fs::path base_path = [] {
            const auto base_path_string = SDL_GetBasePath();
            if (!base_path_string)
                return fs::path();
            fs::path base_path(base_path_string);
            SDL_free(base_path_string);
            return base_path;
        }();
        return base_path;
    }

    inline const fs::path& get_assets_path()
    {
        static const fs::path assets_path = get_base_path() / "assets";
        return assets_path;
    }

    inline fs::path get_asset_path(const fs::path& asset_name)
    {
        return get_assets_path() / asset_name;
    }

    template <typename AssetT, typename KeyT = std::string>
    class Repository
    {
    public:
        const AssetT* insert_asset(const KeyT& asset_key, AssetT&& asset)
        {
            const auto result = m_assets.emplace(asset_key, std::move(asset));
            const auto already_inserted = !result.second;
            if (already_inserted)
                return nullptr;
            const auto& iter_asset = result.first;
            return &iter_asset->second;
        }

        const AssetT* find_asset(const KeyT& asset_key) const
        {
            const auto iter_asset = m_assets.find(asset_key);
            if (iter_asset == m_assets.end())
                return nullptr;
            return &iter_asset->second;
        }

    private:
        std::map<KeyT, AssetT> m_assets;
    };

    namespace detail {

        template <typename AssetT, typename... Ts>
        struct AssetLoader;

        template <typename... Ts>
        struct AssetLoader<Texture, Ts...>
        {
            static auto load(const Renderer& renderer,
                             const stdnext::filesystem::path& asset_path,
                             Ts&&... ts) noexcept
            {
                return load_texture(renderer, asset_path,
                                    std::forward<Ts>(ts)...);
            }
        };

    } // namespace detail

    template <typename AssetT, typename... Ts>
    inline result<AssetT>
    load_asset(const Renderer& renderer,
               const stdnext::filesystem::path& asset_path, Ts&&... ts) noexcept
    {
        return detail::AssetLoader<AssetT, std::remove_cv_t<Ts>...>::load(
            renderer, asset_path, std::forward<Ts>(ts)...);
    }

    template <typename AssetT, typename... Ts>
    inline result<Repository<AssetT>> load_assets(
        const stdnext::filesystem::path& assets_path, const char* assets_extension,
                                                  const Renderer& renderer,
                                                  Ts&&... ts) noexcept
    {
        try
        {
            Repository<AssetT> assets;
            for (const auto& entry :
                 stdnext::filesystem::directory_iterator(assets_path))
            {
                const auto& entry_path = entry.path();
                if (stdnext::filesystem::is_regular_file(entry_path) &&
                    entry_path.extension() == assets_extension)
                {
                    BOOST_OUTCOME_TRY(
                        asset, load_asset<AssetT>(renderer, entry_path,
                                                  std::forward<Ts>(ts)...));
                    assets.insert_asset(entry_path.stem().string(),
                                        std::move(asset));
                }
            }
            return assets;
        }
        catch (...)
        {
            return stdnext::make_error_code(stdnext::errc::bad_file_descriptor);
        }
    }

} // namespace sdlxx
