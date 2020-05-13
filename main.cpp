#define IMGUI_IMPLIMENTATION
#include "include.h"

int main() {
    sys_init();

    Entity *test_entity = create_entity();
    set_entity_texture(test_entity, "data/textures/test.png");
    test_entity->position = Vec2(100, 100);

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
                    input_mouse(event.button.x, event.button.y, event.button.button, down);
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

            update_entities();

            renderer_begin_frame();
            render_entities();
            renderer_end_frame();
            
            frame_num++;
        }
    }

    sys_shutdown();

    return 0;
}