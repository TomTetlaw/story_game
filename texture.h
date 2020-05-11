#ifndef __TEXTURE_H__
#define __TEXTURE_H__

struct Texture {
    int width = 0;
    int height = 0;
    uint api_object = 0;
    const char *file_name = nullptr;
};

Texture *load_texture(const char *file_name);
void unload_textures();

#endif