#include "raylib.h"

#define SCREEN_FACTOR 100
#define WIDTH 16 * SCREEN_FACTOR
#define HEIGHT 9 * SCREEN_FACTOR

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Lithos");
    SetWindowMonitor(0); // force primary monitor
    SetTargetFPS(240);

    while (!WindowShouldClose()) {
        BeginDrawing(); {
            ClearBackground(BLACK);
            DrawText("Lithos Alpha", 0, 0, 20, LIGHTGRAY);
        } EndDrawing();
    }

    return 0;
}
