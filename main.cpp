#define IMGUI_IMPLIMENTATION
#include "include.h"

int main() {
    sys_init();

    Texture *texture = load_texture("data/textures/test.png");
    Render_Texture rt = { texture, Vec2(300, 300), Vec2(300, 300) };

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

            renderer_begin_frame();
            render_texture(&rt);
            renderer_end_frame();
        }
    }

    sys_shutdown();

    return 0;
}