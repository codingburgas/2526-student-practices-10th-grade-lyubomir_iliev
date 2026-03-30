#include "App.h"

App::App(int width, int height, const char* title) {
    InitWindow(width, height, title);
    SetTargetFPS(60);
}

App::~App() {
    CloseWindow();
}

void App::Run() {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("RAYLIB IS LINKED!", 250, 200, 20, MAROON);
        EndDrawing();
    }
}