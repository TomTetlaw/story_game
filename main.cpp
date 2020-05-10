#include "include/raylib/raylib.h"
#include "array.h"
#include <string.h>
#include <stdio.h>
#define internal static

struct My_Texture {
    Texture2D raylib_texture;
    const char *file_name = nullptr;
};

internal Array<My_Texture> textures;

internal int load_texture(const char *file_name) {
    for(int i = 0; i < textures.num; i++) 
        if(!strcmp(textures[i].file_name, file_name)) return i;

    Texture2D raylib_texture = LoadTexture(file_name);
    if(raylib_texture.id == 0) {
        return -1;
    }

    textures.append({raylib_texture, file_name});
    return textures.num - 1;
}

void unload_textures() {
    for(int i = 0; i < textures.num; i++) UnloadTexture(textures[i].raylib_texture);
    textures.num = 0;
}

Texture2D *get_texture(int texture) {
    if(texture < 0 || texture >= textures.num) return nullptr;
    return &textures[texture].raylib_texture;
}

int main() {
    InitWindow(1366, 768, "Game");
    SetTargetFPS(120);

    int t = load_texture("data/textures/test.png");

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        char text[1024] = {};
        sprintf_s(text, 1024, "%d", GetFPS());
        DrawText(text, 10, 10, 16, WHITE);
        DrawTexture(*get_texture(t), 100, 100, WHITE);

        EndDrawing();
    }

    unload_textures();
    CloseWindow();
    return 0;
}