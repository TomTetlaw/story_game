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

#endif