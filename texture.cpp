#include "include.h"

internal Array<Texture *> textures;

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

	texture->size = Vec2(surface->w, surface->h);
	unsigned int t;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    glObjectLabel(GL_TEXTURE, t, -1, texture->file_name);
    texture->api_object = t;

	SDL_FreeSurface(surface);
}

void texture_hotload_callback(const char *filename, void *data) {
	Texture *texture = (Texture *)data;
	glDeleteTextures(1, &texture->api_object);
	load_into_texture(texture);
}

Texture *load_texture(const char *file_name) {
    for(int i = 0; i < textures.num; i++) {
        if(!strcmp(file_name, textures[i]->file_name)) {
            return textures[i];
        }
    }

    Texture *texture = new Texture;
    texture->file_name = file_name;
    load_into_texture(texture);
    hotload_add_file(file_name, (void *)texture, texture_hotload_callback);
    textures.append(texture);
    return texture;
}

void unload_textures() {
    for(int i = 0; i < textures.num; i++) {
        if(textures[i]->api_object) {
            glDeleteTextures(1, &textures[i]->api_object);
            textures[i]->api_object = 0;
        }
    }
}

void textures_shutdown() {
    For(textures) {
        delete it;
    }}}
}


internal void create_texture_data_from_surface(Texture *texture, SDL_Surface *surf) {
	if (!surf) {
		return;
	}

	if (surf->format->format != SDL_PIXELFORMAT_RGBA8888) {
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

		SDL_Surface *newSurf = SDL_ConvertSurface(surf, &format, 0);
		SDL_FreeSurface(surf);
		surf = newSurf;
	}

	unsigned int t;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
    glObjectLabel(GL_TEXTURE, t, -1, texture->file_name);

	SDL_FreeSurface(surf);

	texture->size.x = surf->w;
	texture->size.y = surf->h;
	if (texture->size.x <= 0 || texture->size.y <= 0) {
		//@todo: figure out why this happens. It doesn't seem to mess anything up.
		//console_printf("Texture with zero width but valid SDL surface! [%s: (%d, %d) (%d, %d)]\n", texture->filename, texture->width, texture->height, surf->clip_rect.w, surf->clip_rect.h);
		texture->size.x = surf->clip_rect.w;
		texture->size.y = surf->clip_rect.h;
	}
	texture->api_object = t;
}

Texture *create_texture_from_surface(const char *name, SDL_Surface *surface) {
	Texture *texture = new Texture;
	texture->file_name = name;
	textures.append(texture);
	create_texture_data_from_surface(texture, surface);
	return texture;
}
