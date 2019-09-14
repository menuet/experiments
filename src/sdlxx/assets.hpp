
#pragma once

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

} // namespace sdlxx
