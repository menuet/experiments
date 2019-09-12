
#pragma once

#include "sdl_disabled_warnings.h"
#include <platform/filesystem.hpp>

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

    static fs::path get_asset_path(const fs::path& asset_name)
    {
        return get_assets_path() / asset_name;
    }

} // namespace sdlxx
