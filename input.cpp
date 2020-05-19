#include "include.h"

bool input_mouse(Vec2 position, int button, bool down) {
    if(editor_mouse_press(position, button, down)) return true;
    return false;
}

bool input_key(int key, bool down, int mods) {
    if(editor_key_press(key, down, mods)) return true;
    return false;
}