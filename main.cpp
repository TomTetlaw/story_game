#define IMGUI_IMPLIMENTATION
#include "include.h"

int main() {
    sys_init();

    Entity *entity = create_entity();
    add_texture_component(entity);
    add_physics_component(entity);
    entity->physics->position = Vec2(100, 100);
    entity->physics->size = Vec2(32, 32);
    entity->texture->texture = load_texture("data/textures/test.png");

    bool running = true;
    SDL_Event event;
    while(running) {
        if(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)) {
                running = false;
                break;
            }

            if(!editor_gui_handled(&event)) {
                if(event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
                    bool down = (event.type == SDL_MOUSEBUTTONDOWN);
                    input_mouse(Vec2(event.button.x, event.button.y), event.button.button, down);
                } else if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                    int mods = 0;
                    if(event.key.keysym.mod | KMOD_CTRL) mods |= KEY_MOD_CTRL;
                    if(event.key.keysym.mod | KMOD_ALT) mods |= KEY_MOD_ALT;
                    if(event.key.keysym.mod | KMOD_SHIFT) mods |= KEY_MOD_SHIFT;
                    bool down = (event.type == SDL_KEYDOWN);
                    input_key(event.key.keysym.scancode, down, mods);
                }
            }
        } else {
            sys_update();
            entity_update();
            editor_update();

            renderer_begin_frame();
            entity_render();
            editor_render();
            renderer_end_frame();
            
            frame_num++;
        }
    }

    sys_shutdown();

    return 0;
}