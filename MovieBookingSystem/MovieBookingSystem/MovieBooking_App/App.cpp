#include "App.h"
#include "UIComponents.h"
#include <string>
App::App(int width, int height, const char* title) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, title);
    SetTargetFPS(60);
    infoVisibleIdx = -1;
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            seats[r][c].isReserved = false;
            seats[r][c].isSelected = false;
        }
    }
}
App::~App() {
    CloseWindow();
}
void App::HandleInput() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    float currentY = 120.0f;
    if (bookingMgr.GetCurrentScreen() == BLL::LOGIN) {
        if (UI::Button({ (float)sw / 2 - 150, (float)sh / 2 - 140, 300, 70 }, "SPECTATOR", BLUE, 28)) {
            bookingMgr.LoginAsSpectator();
        }
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125 && letterCount < 31) {
                passwordInput[letterCount] = (char)key;
                passwordInput[letterCount + 1] = '\0';
                letterCount++;
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && letterCount > 0) {
            letterCount--;
            passwordInput[letterCount] = '\0';
        }
        if ((UI::Button({ (float)sw / 2 - 150, (float)sh / 2 + 100, 300, 60 }, "ADMIN LOGIN", RED, 28)) || IsKeyPressed(KEY_ENTER)) {
            if (!bookingMgr.CheckAdminPassword(passwordInput)) showError = true;
            else { showError = false; letterCount = 0; passwordInput[0] = '\0'; }
        }
    }
    else if (bookingMgr.GetCurrentScreen() == BLL::CATALOG) {
        auto& list = bookingMgr.GetMovies();
        for (int i = 0; i < (int)list.size(); i++) {
            Rectangle card = { (float)sw / 2 - 400, currentY, 800, 100 };
            if (UI::Button({ card.x + 650, card.y + 25, 130, 50 }, "BUY", GREEN, 22)) {
                bookingMgr.SelectMovie(i);
                return;
            }
            if (UI::Button({ card.x + 510, card.y + 25, 120, 50 }, "INFO", ORANGE, 22)) {
                infoVisibleIdx = (infoVisibleIdx == i) ? -1 : i;
            }
            if (bookingMgr.IsAdmin()) {
                if (UI::Button({ card.x + 420, card.y + 25, 70, 50 }, "DEL", RED, 20)) {
                    bookingMgr.DeleteMovie(i);
                    return;
                }
            }
            if (infoVisibleIdx == i) currentY += 170.0f;
            else currentY += 110.0f;
        }
        if (UI::Button({ 20, 20, 120, 40 }, "LOGOUT", GRAY)) {
            infoVisibleIdx = -1;
            bookingMgr.Logout();
        }
    }
    else if (bookingMgr.GetCurrentScreen() == BLL::BOOKING) {
        Vector2 mousePos = GetMousePosition();
        float startX = (sw - (COLS * 65)) / 2.0f;
        float startY = (sh - (ROWS * 65)) / 2.0f;
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                seats[r][c].rect = { startX + c * 65, startY + r * 65, 55, 55 };
                if (!seats[r][c].isReserved && CheckCollisionPointRec(mousePos, seats[r][c].rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    seats[r][c].isSelected = !seats[r][c].isSelected;
                }
            }
        }
        if (UI::Button({ (float)sw - 220, (float)sh - 80, 200, 60 }, "CONFIRM", GREEN, 25)) {
            std::vector<int> state;
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (seats[r][c].isSelected) seats[r][c].isReserved = true;
                    state.push_back(seats[r][c].isReserved ? 1 : 0);
                    seats[r][c].isSelected = false;
                }
            }
            bookingMgr.FinalizeBooking(state);
        }
        if (UI::Button({ 20, 20, 120, 40 }, "BACK", GRAY)) {
            infoVisibleIdx = -1;
            bookingMgr.LoginAsSpectator();
        }
    }
}
void App::RenderLogin() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    DrawText("CINEMA WORLD", sw / 2 - 200, sh / 2 - 280, 60, DARKGRAY);
    DrawLine(sw / 2 - 200, sh / 2 - 20, sw / 2 + 200, sh / 2 - 20, LIGHTGRAY);
    DrawText("ADMIN SECTION", sw / 2 - 80, sh / 2 - 5, 20, GRAY);
    DrawRectangleLinesEx({ (float)sw / 2 - 150, (float)sh / 2 + 30, 300, 50 }, 3, DARKGRAY);
    std::string dots(letterCount, '*');
    DrawText(dots.c_str(), sw / 2 - 135, sh / 2 + 40, 30, DARKGRAY);
    if (letterCount == 0) DrawText("Password...", sw / 2 - 135, sh / 2 + 42, 25, GRAY);
    if (showError) DrawText("Wrong password!", sw / 2 - 80, sh / 2 + 180, 20, RED);
}
void App::RenderCatalog() {
    int sw = GetScreenWidth();
    DrawText("MOVIE CATALOG", sw / 2 - 150, 40, 40, DARKGRAY);
    auto& list = bookingMgr.GetMovies();
    float currentY = 120.0f;
    for (int i = 0; i < (int)list.size(); i++) {
        Rectangle card = { (float)sw / 2 - 400, currentY, 800, 100 };
        DrawRectangleRec(card, LIGHTGRAY);
        DrawRectangleLinesEx(card, 2, GRAY);
        DrawText(list[i].title.c_str(), card.x + 20, card.y + 20, 30, MAROON);
        DrawText(TextFormat("Price: %.2f EUR", list[i].price), card.x + 20, card.y + 60, 20, DARKGREEN);
        UI::Button({ card.x + 650, card.y + 25, 130, 50 }, "BUY", GREEN, 22);
        UI::Button({ card.x + 510, card.y + 25, 120, 50 }, "INFO", ORANGE, 22);
        if (bookingMgr.IsAdmin()) UI::Button({ card.x + 420, card.y + 25, 70, 50 }, "DEL", RED, 20);
        if (infoVisibleIdx == i) {
            DrawRectangle(card.x, card.y + 100, card.width, 60, ORANGE);
            DrawText(list[i].info.c_str(), card.x + 20, card.y + 120, 20, WHITE);
            currentY += 170.0f;
        }
        else {
            currentY += 110.0f;
        }
    }
}
void App::RenderHall() {
    int sw = GetScreenWidth();
    DrawRectangle(sw / 2 - 325, 45, 650, 15, DARKGRAY);
    DrawText("SCREEN", sw / 2 - 60, 15, 30, LIGHTGRAY);
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            Color color = seats[r][c].isReserved ? RED : (seats[r][c].isSelected ? LIME : LIGHTGRAY);
            DrawRectangleRec(seats[r][c].rect, color);
            DrawRectangleLinesEx(seats[r][c].rect, 2, BLACK);
        }
    }
}
void App::Run() {
    while (!WindowShouldClose()) {
        HandleInput();
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BLL::ScreenState state = bookingMgr.GetCurrentScreen();
        if (state == BLL::LOGIN) RenderLogin();
        else if (state == BLL::CATALOG) RenderCatalog();
        else if (state == BLL::BOOKING) RenderHall();
        EndDrawing();
    }
}