#include "include/raylib/raylib.h"

int main() {
    InitWindow(1366, 768, "Game");
    SetTargetFPS(120);

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}