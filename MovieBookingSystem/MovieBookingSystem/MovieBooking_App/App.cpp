#include "App.h"

App::App(int width, int height, const char* title) {
    InitWindow(width, height, title);
    SetTargetFPS(60);
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            seats[r][c].rect = { (float)(150 + c * 50), (float)(100 + r * 50), 40, 40 };
            seats[r][c].isReserved = (r == 0);
            seats[r][c].isSelected = false;
        }
    }
}
App::~App() {

    CloseWindow();
}

void App::Run() {
    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (!seats[r][c].isReserved && CheckCollisionPointRec(mousePos, seats[r][c].rect)) {
                        seats[r][c].isSelected = !seats[r][c].isSelected;
                    }
                }
            }
        }
        int selectedCount = 0;
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                if (seats[r][c].isSelected) selectedCount++;
            }
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangle(150, 60, 490, 10, DARKGRAY);
        DrawText("SCREEN", 360, 40, 20, LIGHTGRAY);
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {

                Color seatColor = LIGHTGRAY;
                if (seats[r][c].isReserved) seatColor = RED;
                else if (seats[r][c].isSelected) seatColor = LIME; 
                DrawRectangleRec(seats[r][c].rect, seatColor);
                DrawRectangleLinesEx(seats[r][c].rect, 1, BLACK);
            }
        }
        DrawText(TextFormat("Selected: %i", selectedCount), 150, 520, 20, DARKGRAY);
        DrawText(TextFormat("Total: %i BGN", selectedCount * 12), 400, 520, 20, MAROON);
        DrawText("Legend: Gray=Free, Red=Full, Green=Selected", 150, 560, 15, GRAY);
        EndDrawing();
    }
}