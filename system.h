#ifndef __SYSTEM_H__
#define __SYSTEM_H__

struct System {
    SDL_GLContext context = nullptr;
    SDL_Window *window = nullptr;
};

extern System sys;

extern int window_width;
extern int window_height;
extern float now;
extern float delta_time;
extern int frame_num;
extern Vec2 mouse_position;

void sys_init();
void sys_shutdown();
void sys_update();
void sys_error(const char *text, ...);

#endif