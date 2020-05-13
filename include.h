#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include <string.h>
#include <stdio.h>
#include <math.h>

#define SDL_MAIN_HANDLED 1
#include "include/sdl2/sdl.h"
#include "include/sdl2/sdl_image.h"

#define GLEW_STATIC
#include "include/glew/glew.h"

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

#define internal static
typedef unsigned int uint;

#include "array.h"
#include "mathlib.h"
#include "common.h"
#include "system.h"
#include "texture.h"
#include "renderer.h"
#include "hotload.h"
#include "entity.h"

#endif