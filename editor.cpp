#include "include.h"

ImGuiIO *io = nullptr;

void editor_update() {

}

void editor_render() {

}

void editor_init() {
    io = &ImGui::GetIO();
}

void editor_shutdown() {

}

bool editor_mouse_press(int x, int y, uint buttons, bool down) {
    return false;
}

bool editor_key_press(int key, bool down, int mods) {
    return false;
}

bool editor_gui_handled(SDL_Event *event) {
    ImGui_ImplSDL2_ProcessEvent(event);
    return io->WantCaptureKeyboard || io->WantCaptureMouse;
}