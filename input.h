#ifndef __INPUT_H__
#define __INPUT_H__

enum Key_Mods {
    KEY_MOD_CTRL,
    KEY_MOD_ALT,
    KEY_MOD_SHIFT,
};

bool input_mouse(Vec2 position, int button, bool down);
bool input_key(int key, bool down, int mods);

#endif