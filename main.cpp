#define IMGUI_IMPLIMENTATION
#include "include.h"

struct Texture {
    int width = 0;
    int height = 0;
    uint api_object = 0;
    const char *file_name = nullptr;
};

internal Array<Texture> textures;

Texture *load_texture(const char *file_name) {
    for(int i = 0; i < textures.num; i++) {
        if(!strcmp(file_name, textures[i].file_name)) {
            return &textures[i];
        }
    }

    SDL_Surface *surface = IMG_Load(file_name);
    if(!surface) return nullptr;

	if (surface->format->format != SDL_PIXELFORMAT_RGBA8888) {
		SDL_PixelFormat format = { 0 };
		format.BitsPerPixel = 32;
		format.BytesPerPixel = 4;
		format.format = SDL_PIXELFORMAT_RGBA8888;
		format.Rshift = 0;
		format.Gshift = 8;
		format.Bshift = 16;
		format.Ashift = 24;
		format.Rmask = 0xff << format.Rshift;
		format.Gmask = 0xff << format.Gshift;
		format.Bmask = 0xff << format.Bshift;
		format.Amask = 0xff << format.Ashift;

		SDL_Surface *new_surface = SDL_ConvertSurface(surface, &format, 0);
		SDL_FreeSurface(surface);
		surface = new_surface;
	}

	unsigned int api_object = 0;
	glGenTextures(1, &api_object);
	glBindTexture(GL_TEXTURE_2D, api_object);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	SDL_FreeSurface(surface);

    Texture *texture = textures.alloc();
	texture->width = surface->w;
	texture->height = surface->h;
    texture->api_object = api_object;
    texture->file_name = file_name;
    return texture;
}

void unload_textures() {
    for(int i = 0; i < textures.num; i++) {
        if(textures[i].api_object) {
            glDeleteTextures(1, &textures[i].api_object);
            textures[i].api_object = 0;
        }
    }
}

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1366, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GLContext context = SDL_GL_CreateContext(window);

    IMG_Init(IMG_INIT_PNG);

    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init();

    Texture *texture = load_texture("data/textures/test.png");

    bool running = true;
    SDL_Event event;
    while(running) {
        if(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)) {
                running = false;
                break;
            }
        } else {
        }
    }

    unload_textures();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();

    IMG_Quit();
    SDL_Quit();

    return 0;
}