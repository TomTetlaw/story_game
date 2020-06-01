#define IMGUI_IMPLIMENTATION
#include "include.h"

enum Tile_Type {
    TT_FLOOR,
    TT_WALL,
};

struct Tile {
    Tile_Type type;
    Texture *texture = nullptr;
    Vec2 position;
};

bool tile_is_blocking(Tile *tile) {
    return tile->type == TT_WALL;
}

struct Level {
    Array<Tile> tiles;
    int width = 0;
    int height = 0;
};

void parse_level_row(Level *level, const char *string) {
    level->width = strlen(string);

    for(int i = 0; i < level->width; i++) {
        char c = string[i];

        switch(c) {
        case 'x': {
            Tile *tile = level->tiles.alloc();
            tile->type = TT_WALL;
            tile->texture = load_texture("data/textures/wall.png");
            tile->position = Vec2(i, level->height);            
        } break;
        case ' ': {
            Tile *tile = level->tiles.alloc();
            tile->type = TT_FLOOR;
            tile->texture = load_texture("data/textures/floor.png");
            tile->position = Vec2(i, level->height);
        } break;
        default:
            printf("unknown char: %d '%c'\n", c, c);
            break;
        }
    }

    level->height += 1;
}

int main() {
    sys_init();

    Level l0;
    parse_level_row(&l0, "xxxxxxxxxxxxxxxxxx");
    parse_level_row(&l0, "x                x");
    parse_level_row(&l0, "x        xxxxxxxxx");
    parse_level_row(&l0, "x        x        ");
    parse_level_row(&l0, "x        x        ");
    parse_level_row(&l0, "x        xxxxxxxxx");
    parse_level_row(&l0, "x                x");
    parse_level_row(&l0, "x                x");
    parse_level_row(&l0, "xxxxxxxxxxxxxxxxxx");

    bool running = true;
    SDL_Event event;
    while(running) {
        if(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)) {
                running = false;
                break;
            }

            if(event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
                bool down = (event.type == SDL_MOUSEBUTTONDOWN);
                input_mouse(Vec2(event.button.x, event.button.y), event.button.button, down);
            } else if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                int mods = 0;
                if(event.key.keysym.mod | KMOD_CTRL)    mods |= KEY_MOD_CTRL;
                if(event.key.keysym.mod | KMOD_ALT)     mods |= KEY_MOD_ALT;
                if(event.key.keysym.mod | KMOD_SHIFT)   mods |= KEY_MOD_SHIFT;
                bool down = (event.type == SDL_KEYDOWN);
                input_key(event.key.keysym.scancode, down, mods);
            }
        } else {
            sys_update();

            renderer_begin_frame();

            const float tile_size = 32;
            For(l0.tiles) {
                Render_Texture rt;
                rt.position = it.position * Vec2(tile_size, tile_size);
                rt.texture = it.texture;
                render_texture(&rt);
            }}}

            renderer_end_frame();
            
            frame_num++;
        }
    }

    sys_shutdown();

    return 0;
}