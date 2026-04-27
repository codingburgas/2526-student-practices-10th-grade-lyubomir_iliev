#include "App.h"
#include "../MovieBooking_BLL/BookingManager.h"
BLL::BookingManager bookingMgr;
App::App(int width, int height, const char* title) {
    InitWindow(width, height, title);
    SetTargetFPS(60);
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            seats[r][c].rect = { (float)(150 + c * 50), (float)(80 + r * 50), 40, 40 };
            seats[r][c].isReserved = (r == 0);
            seats[r][c].isSelected = false;
        }
    }
}
App::~App() {
    CloseWindow();
}
void App::Run() {
    Rectangle confirmBtn = { 600, 530, 150, 40 };
    while (!WindowShouldClose()) {
        int selectedCount = 0;
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                if (seats[r][c].isSelected) selectedCount++;
            }
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (!seats[r][c].isReserved && CheckCollisionPointRec(mousePos, seats[r][c].rect)) {
                        seats[r][c].isSelected = !seats[r][c].isSelected;
                    }
                }
            }
            if (CheckCollisionPointRec(mousePos, confirmBtn)) {
                std::vector<int> hallState;
                for (int r = 0; r < ROWS; r++) {
                    for (int c = 0; c < COLS; c++) {
                        if (seats[r][c].isSelected) seats[r][c].isReserved = true;
                        hallState.push_back(seats[r][c].isReserved ? 1 : 0);
                        seats[r][c].isSelected = false;
                    }
                }
                bookingMgr.FinalizeBooking(hallState);
            }
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangle(150, 40, 490, 10, DARKGRAY);
        DrawText("SCREEN", 360, 20, 20, LIGHTGRAY);
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                Color seatColor = LIGHTGRAY;
                if (seats[r][c].isReserved) seatColor = RED;
                else if (seats[r][c].isSelected) seatColor = LIME;
                DrawRectangleRec(seats[r][c].rect, seatColor);
                DrawRectangleLinesEx(seats[r][c].rect, 1, BLACK);
            }
        }
        DrawRectangle(150, 500, 20, 20, LIGHTGRAY);
        DrawText("Available", 180, 503, 15, DARKGRAY);
        DrawRectangle(280, 500, 20, 20, RED);
        DrawText("Reserved", 310, 503, 15, DARKGRAY);
        DrawRectangle(410, 500, 20, 20, LIME);
        DrawText("Selected", 440, 503, 15, DARKGRAY);
        bool isHover = CheckCollisionPointRec(GetMousePosition(), confirmBtn);
        DrawRectangleRec(confirmBtn, isHover ? DARKGREEN : GREEN);
        DrawText("CONFIRM", (int)confirmBtn.x + 35, (int)confirmBtn.y + 10, 20, WHITE);
        DrawText(TextFormat("Seats: %i", selectedCount), 150, 540, 20, DARKGRAY);
        DrawText(TextFormat("Total: %.2f EUR", selectedCount * 6.50f), 350, 540, 20, MAROON);
        EndDrawing();
    }
}