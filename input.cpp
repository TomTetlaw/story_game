#include "include.h"

bool input_mouse(int x, int y, uint buttons, bool down) {
    if(editor_mouse_press(x, y, buttons, down)) return true;
    return false;
}

bool input_key(int key, bool down, int mods) {
    if(editor_key_press(key, down, mods)) return true;
    return false;
}