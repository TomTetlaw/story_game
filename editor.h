#ifndef __EDITOR_H__
#define __EDITOR_H__

void editor_update();
void editor_render();
void editor_init();
void editor_shutdown();
bool editor_mouse_press(int x, int y, uint buttons, bool down);
bool editor_key_press(int key, bool down, int mods);
bool editor_gui_handled(SDL_Event *event);

#endif