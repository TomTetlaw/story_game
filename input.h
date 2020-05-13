#ifndef __INPUT_H__
#define __INPUT_H__

enum Key_Mods {
    KEY_MOD_CTRL,
    KEY_MOD_ALT,
    KEY_MOD_SHIFT,
};

bool input_mouse(int x, int y, uint buttons, bool down);
bool input_key(int key, bool down, int mods);

#endif