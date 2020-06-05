#ifndef __TEXTURE_H__
#define __TEXTURE_H__

struct Texture {
    Vec2 size;
    uint api_object = 0;
    const char *file_name = nullptr;
};

void textures_shutdown();

Texture *load_texture(const char *file_name);
void unload_textures();

// frees the surface after it creates the texture
Texture *create_texture_from_surface(const char *name, SDL_Surface *surface);

#endif