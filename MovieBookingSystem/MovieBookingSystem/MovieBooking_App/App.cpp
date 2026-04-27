#include "App.h"
App::App(int width, int height, const char* title) {
    InitWindow(width, height, title);
    SetTargetFPS(60);
    // Initialize the grid: 8 rows, 10 columns
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            seats[r][c].rect = { (float)(150 + c * 50), (float)(100 + r * 50), 40, 40 };
            seats[r][c].isReserved = (r == 2); // Just for testing: make row 2 reserved
            seats[r][c].isSelected = false;
        }
    }
}
App::~App() { CloseWindow(); }
void App::Run() {
    while (!WindowShouldClose()) {
        // --- INPUT ---
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    // Check if we clicked a seat that isn't already reserved
                    if (!seats[r][c].isReserved && CheckCollisionPointRec(mousePos, seats[r][c].rect)) {
                        seats[r][c].isSelected = !seats[r][c].isSelected;
                    }
                }
            }
        }
        // --- DRAWING ---
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("SCREEN THIS WAY", 300, 50, 20, LIGHTGRAY);
        DrawRectangle(150, 75, 500, 5, DARKGRAY); // The "Screen" line
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                Color seatColor = LIGHTGRAY; // Default
                if (seats[r][c].isReserved) seatColor = RED;
                else if (seats[r][c].isSelected) seatColor = LIME;
                DrawRectangleRec(seats[r][c].rect, seatColor);
                DrawRectangleLinesEx(seats[r][c].rect, 1, BLACK);
            }
        }
        DrawText("Gray = Available | Red = Reserved | Green = Selected", 150, 550, 18, DARKGRAY);
        EndDrawing();
    }
}