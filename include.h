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

#define internal static
typedef unsigned int uint;

#include "allocator.h"
#include "array.h"
#include "mathlib.h"
#include "common.h"
#include "system.h"
#include "texture.h"
#include "renderer.h"
#include "hotload.h"
#include "input.h"

#endif