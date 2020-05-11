#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include <string.h>
#include <stdio.h>

#define SDL_MAIN_HANDLED
#include "include/sdl2/sdl.h"
#include "include/sdl2/SDL_image.h"

#define GLEW_STATIC
#include "include/glew/glew.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#include "array.h"

#define internal static

#endif