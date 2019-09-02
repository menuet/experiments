
#pragma once

#include <platform/platform.h>

#if EXP_PLATFORM_CPL_IS_CLANG
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragma-pack"
#endif

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#if EXP_PLATFORM_CPL_IS_CLANG
#pragma GCC diagnostic pop
#endif
