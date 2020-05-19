#include "include.h"

ImGuiIO *io = nullptr;
bool drag_select = false;
Vec2 drag_start_point;
Vec2 drag_size;

Array<Entity *> selected;
Entity *maybe_select = nullptr;

bool shift_down = false;

void editor_update() {
    if(drag_select) {
        drag_size = mouse_position - drag_start_point;
    }
}

void editor_render() {
   if(drag_select) {
       render_box(drag_start_point, drag_size, Vec4(0, 1, 0, 1));
       printf("Dragging %f.\n", now);
   }
}

void editor_init() {
    io = &ImGui::GetIO();
}

void editor_shutdown() {
}

Entity *find_entity_at(Vec2 position) {
    For(entity_manager.entities) {
        Vec2 pos_tl = it->physics->position - (it->physics->size * 0.5f);
        Vec2 size = it->physics->size;
        if(position.x < pos_tl.x) continue;
        if(position.x > pos_tl.x + size.x) continue;
        if(position.y < pos_tl.y) continue;
        if(position.y > pos_tl.y + size.y) continue;

        return it;
    }}}

    return nullptr;
}

void finished_dragging() {
    if(drag_size.x == 0) {
        drag_size.x = 1;
    }
    if(drag_size.y == 0) {
        drag_size.y = 1;
    }
    if(drag_size.x < 0) {
        drag_start_point.x = drag_start_point.x + drag_size.x;
        drag_size.x = drag_size.x * -1;
    }
    if(drag_size.y < 0) {
        drag_start_point.y = drag_start_point.y + drag_size.y;
        drag_size.y = drag_size.y * -1;
    }
}

bool editor_mouse_press(Vec2 position, int button, bool down) {
    if(button == SDL_BUTTON_LEFT) {
        Entity *entity = find_entity_at(position);
        if(entity) {
            if(down) {
                maybe_select = entity;
            }
            else {
                if(maybe_select == entity) {
                    if(!shift_down) selected.num = 0;
                    selected.append(entity);
                    maybe_select = nullptr;
                    return true;
                }

                return false;
            }
        }

        maybe_select = nullptr;
        drag_select = down;
        if(down) {
            drag_start_point = position;
            drag_size = Vec2(0, 0);
        } else {
            finished_dragging();
        }

        return true;
    }

    return false;
}

bool editor_key_press(int key, bool down, int mods) {
    if(mods & KEY_MOD_SHIFT) shift_down = down;

    return false;
}

bool editor_gui_handled(SDL_Event *event) {
    ImGui_ImplSDL2_ProcessEvent(event);
    return io->WantCaptureKeyboard || io->WantCaptureMouse;
}