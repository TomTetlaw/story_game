#include "include.h"

internal Array<Texture> textures;

void load_into_texture(Texture *texture) {
    SDL_Surface *surface = IMG_Load(texture->file_name);
    if(!surface) return;

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

	texture->size = Vec2(surface->w, surface->h);
    texture->api_object = api_object;

	SDL_FreeSurface(surface);
}

void texture_hotload_callback(const char *filename, void *data) {
	Texture *texture = (Texture *)data;
	glDeleteTextures(1, &texture->api_object);
	load_into_texture(texture);
}

Texture *load_texture(const char *file_name) {
    for(int i = 0; i < textures.num; i++) {
        if(!strcmp(file_name, textures[i].file_name)) {
            return &textures[i];
        }
    }

    Texture *texture = textures.alloc();
    texture->file_name = file_name;
    load_into_texture(texture);
    hotload_add_file(file_name, (void *)texture, texture_hotload_callback);
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