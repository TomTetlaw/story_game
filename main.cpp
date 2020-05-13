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
        } else {
            sys_update();

            update_entities();
            if(frame_num == 1000) remove_entity(test_entity);

            renderer_begin_frame();
            render_entities();
            renderer_end_frame();
            
            frame_num++;
        }
    }

    sys_shutdown();

    return 0;
}