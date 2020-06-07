/* date = June 6th 2020 9:20 am */

#ifndef UI_H
#define UI_H

extern bool ui_wants_focus;

void ui_init();
void ui_shutdown();

void ui_process_event(SDL_Event *event);

void ui_new_frame();
void ui_render();

void ui_new_panel(Vec2 position, const char *title);
void ui_end_panel();

void ui_label(const char *text);
bool ui_button(const char *text);


#endif //UI_H
