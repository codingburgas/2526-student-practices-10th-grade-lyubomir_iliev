#include "App.h"
#include "UIComponents.h"
#include <string>
#include <fstream>
#include <algorithm>

App::App(int width, int height, const char* title) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, title);
    SetTargetFPS(60);
    infoVisibleIdx = -1;
    ResetSeats();
}

App::~App() {
    CloseWindow();
}

void App::ResetSeats() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            seats[r][c].isReserved = false;
            seats[r][c].isSelected = false;
        }
    }
}

void App::LoadSeatingPlan(std::string movieTitle, std::string time) {
    ResetSeats();
    std::string filename = movieTitle + "_seats.txt";
    std::replace(filename.begin(), filename.end(), ' ', '_');

    std::ifstream file(filename);
    if (file.is_open()) {
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                int val;
                if (file >> val) seats[r][c].isReserved = (val == 1);
            }
        }
        file.close();
    }
}

void App::HandleInput() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    float dt = GetFrameTime();
    caretTimer += dt;

    static bool bsPrev = false;
    static float bsHold = 0.0f;
    static float bsRepeat = 0.0f;

    auto backspace = [&](char* buf, int fontSize, int maxWidth, int* offsetPtr) {
        bool down = IsKeyDown(KEY_BACKSPACE);
        if (!down) {
            bsPrev = false;
            bsHold = 0.0f;
            bsRepeat = 0.0f;
            return;
        }
        bool del = false;
        if (!bsPrev) {
            del = true;
            bsHold = 0.0f;
            bsRepeat = 0.0f;
        }
        else {
            bsHold += dt;
            if (bsHold > 0.35f) {
                bsRepeat += dt;
                if (bsRepeat > 0.05f) {
                    del = true;
                    bsRepeat = 0.0f;
                }
            }
        }
        bsPrev = true;
        if (del) {
            int len = strlen(buf);
            if (len > 0) {
                buf[len - 1] = '\0';
                if (strlen(buf) == 0) *offsetPtr = 0;
                else {
                    int tw = MeasureText(buf, fontSize);
                    int off = tw - maxWidth;
                    if (off < 0) off = 0;
                    *offsetPtr = off;
                }
            }
        }
        };

    if (bookingMgr.GetCurrentScreen() == BLL::LOGIN) {

        if (UI::Button({ (float)sw / 2 - 150, (float)sh / 2 - 140, 300, 70 }, "SPECTATOR", BLUE, 28)) {
            bookingMgr.LoginAsSpectator();
        }

        Rectangle passRect = { (float)sw / 2 - 150, (float)sh / 2 + 30, 300, 50 };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            activeField = CheckCollisionPointRec(m, passRect) ? 100 : -1;
            if (activeField != 100) passwordOffset = 0;
        }

        if (activeField == 100) {
            int key = GetCharPressed();
            while (key > 0) {
                int len = strlen(passwordInput);
                if (key >= 32 && key <= 125 && len < 31) {
                    passwordInput[len] = key;
                    passwordInput[len + 1] = '\0';
                }
                key = GetCharPressed();
            }

            int maxw = 300 - 30;
            if (strlen(passwordInput) == 0) passwordOffset = 0;
            else {
                int tw = MeasureText(passwordInput, 30);
                passwordOffset = tw - maxw;
                if (passwordOffset < 0) passwordOffset = 0;
            }

            backspace(passwordInput, 30, maxw, &passwordOffset);
        }

        if (UI::Button({ (float)sw / 2 - 150, (float)sh / 2 + 100, 300, 60 }, "ADMIN LOGIN", RED, 28) || IsKeyPressed(KEY_ENTER)) {
            if (!bookingMgr.CheckAdminPassword(passwordInput)) showError = true;
            else {
                showError = false;
                passwordInput[0] = '\0';
                passwordOffset = 0;
                activeField = -1;
            }
        }

        return;
    }

    if (bookingMgr.GetCurrentScreen() == BLL::CATALOG) {
        Rectangle searchRect = { (float)sw - 400, 40, 220, 35 };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            activeField = CheckCollisionPointRec(m, searchRect) ? 200 : -1;
            if (activeField != 200) searchOffset = 0;
        }

        if (activeField == 200) {
            int key = GetCharPressed();
            while (key > 0) {
                int len = strlen(searchInput);
                if (key >= 32 && key <= 125 && len < 63) {
                    searchInput[len] = key;
                    searchInput[len + 1] = '\0';
                    searching = true;
                }
                key = GetCharPressed();
            }

            int maxw = 220 - 20;
            if (strlen(searchInput) == 0) searchOffset = 0;
            else {
                int tw = MeasureText(searchInput, 20);
                searchOffset = tw - maxw;
                if (searchOffset < 0) searchOffset = 0;
            }

            backspace(searchInput, 20, maxw, &searchOffset);
            if (strlen(searchInput) == 0) searching = false;
        }

        return;
    }

    if (bookingMgr.GetCurrentScreen() == BLL::ADD_MOVIE) {

        Rectangle r1 = { (float)sw / 2 - 300, (float)sh / 2 - 65, 600, 45 };
        Rectangle r2 = { (float)sw / 2 - 300, (float)sh / 2 + 20, 600, 45 };
        Rectangle r3 = { (float)sw / 2 - 300, (float)sh / 2 + 105, 200, 45 };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            if (CheckCollisionPointRec(m, r1)) activeField = 0;
            else if (CheckCollisionPointRec(m, r2)) activeField = 1;
            else if (CheckCollisionPointRec(m, r3)) activeField = 2;
            else {
                activeField = -1;
                titleOffset = infoOffset = priceOffset = 0;
            }
        }

        char* buf = nullptr;
        int* off = nullptr;
        int fontSize = 25;
        int maxw = 0;

        if (activeField == 0) { buf = movieTitleInput; off = &titleOffset; maxw = 600 - 20; }
        if (activeField == 1) { buf = movieInfoInput; off = &infoOffset; maxw = 600 - 20; }
        if (activeField == 2) { buf = moviePriceInput; off = &priceOffset; maxw = 200 - 20; }

        if (buf) {
            int key = GetCharPressed();
            while (key > 0) {
                int len = strlen(buf);
                if (key >= 32 && key <= 125 && len < 250) {
                    buf[len] = key;
                    buf[len + 1] = '\0';
                }
                key = GetCharPressed();
            }

            if (strlen(buf) == 0) *off = 0;
            else {
                int tw = MeasureText(buf, fontSize);
                int o = tw - maxw;
                if (o < 0) o = 0;
                *off = o;
            }

            backspace(buf, fontSize, maxw, off);
        }

        return;
    }
}



void App::RenderLogin() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawText("CINEMA WORLD", sw / 2 - 200, sh / 2 - 280, 60, DARKGRAY);
    DrawLine(sw / 2 - 200, sh / 2 - 20, sw / 2 + 200, sh / 2 - 20, LIGHTGRAY);
    DrawText("ADMIN SECTION", sw / 2 - 80, sh / 2 - 5, 20, GRAY);

    Rectangle passRect = { (float)sw / 2 - 150, (float)sh / 2 + 30, 300, 50 };
    DrawRectangleLinesEx(passRect, 3, DARKGRAY);

    std::string dots(strlen(passwordInput), '*');
    int tw = MeasureText(dots.c_str(), 30);

    DrawText(dots.c_str(), passRect.x + 10 - passwordOffset, passRect.y + 10, 30, DARKGRAY);

    if (strlen(passwordInput) == 0)
        DrawText("Password...", passRect.x + 10 - passwordOffset, passRect.y + 12, 25, GRAY);

    if (activeField == 100 && ((int)(caretTimer * 2) % 2 == 0)) {
        int cx = passRect.x + 10 + tw - passwordOffset;
        DrawRectangle(cx, passRect.y + 10, 3, 30, DARKGRAY);
    }

    if (showError)
        DrawText("Wrong password!", sw / 2 - 80, sh / 2 + 180, 20, RED);
}


void App::RenderCatalog() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    const char* days[] = { "TODAY", "TUE", "WED", "THU", "FRI", "SAT", "SUN" };
    float dayX = 40;
    for (int d = 0; d < 7; d++) {
        DrawText(days[d], dayX, 40, 22, (d == 0) ? ORANGE : DARKGRAY);
        if (d == 0) DrawRectangle(dayX, 65, 60, 3, ORANGE);
        dayX += 80;
    }

    DrawLine(40, 80, sw - 40, 80, LIGHTGRAY);

    Rectangle searchRect = { (float)sw - 400, 40, 220, 35 };
    DrawRectangleRec(searchRect, LIGHTGRAY);
    DrawRectangleLinesEx(searchRect, 2, DARKGRAY);

    const char* txt = searchInput[0] ? searchInput : "Search...";
    Color col = searchInput[0] ? BLACK : GRAY;

    int tw = MeasureText(searchInput, 20);
    int maxw = 220 - 20;
    searchOffset = (strlen(searchInput) == 0) ? 0 : tw - maxw;
    if (searchOffset < 0) searchOffset = 0;

    DrawText(txt, searchRect.x + 10 - searchOffset, searchRect.y + 8, 20, col);

    if (activeField == 200 && ((int)(caretTimer * 2) % 2 == 0)) {
        int cx = searchRect.x + 10 + tw - searchOffset;
        DrawRectangle(cx, searchRect.y + 8, 2, 20, BLACK);
    }

    if (UI::Button({ (float)sw - 150, 40, 120, 35 }, "LOGOUT", GRAY, 18)) {
        bookingMgr.Logout();
        activeField = -1;
        searchInput[0] = '\0';
        searchOffset = 0;
        searching = false;
        return;
    }


    auto& list = bookingMgr.GetMovies();
    float currentY = 110.0f;
    const char* hours[] = { "18:00", "20:30", "22:15" };

    for (int i = 0; i < (int)list.size(); i++) {
        Rectangle card = { 40, currentY, (float)sw - 80, 160 };

        DrawLine(card.x, card.y + card.height + 15, card.x + card.width, card.y + card.height + 15, LIGHTGRAY);
        DrawRectangle(card.x, card.y, 100, 150, DARKGRAY);
        DrawText("POSTER", card.x + 15, card.y + 65, 15, LIGHTGRAY);

        DrawText(list[i].title.c_str(), card.x + 120, card.y, 28, BLACK);
        DrawText(list[i].info.c_str(), card.x + 120, card.y + 35, 18, GRAY);
        DrawText("2D | English", card.x + 120, card.y + 65, 16, DARKGRAY);

        for (int h = 0; h < 3; h++) {
            DrawRectangle(card.x + 120 + (h * 90), card.y + 100, 80, 45, ORANGE);
            DrawText(hours[h], card.x + 135 + (h * 90), card.y + 112, 20, WHITE);
        }

        if (bookingMgr.IsAdmin()) {
            UI::Button({ card.x + card.width - 80, card.y, 80, 35 }, "DEL", RED, 18);
        }

        currentY += 190.0f;
    }

    if (bookingMgr.IsAdmin()) {
        UI::Button({ (float)sw - 220, (float)sh - 60, 200, 40 }, "NEW MOVIE", ORANGE, 20);
    }
}


void App::RenderAddMovie() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawRectangle(0, 0, sw, sh, RAYWHITE);
    DrawText("ADD NEW MOVIE", sw / 2 - 130, 50, 35, MAROON);

    Rectangle r1 = { (float)sw / 2 - 300, (float)sh / 2 - 65, 600, 45 };
    Rectangle r2 = { (float)sw / 2 - 300, (float)sh / 2 + 20, 600, 45 };
    Rectangle r3 = { (float)sw / 2 - 300, (float)sh / 2 + 105, 200, 45 };

    DrawText("TITLE", r1.x, r1.y - 25, 18, DARKGRAY);
    DrawRectangleRec(r1, LIGHTGRAY);
    DrawText(movieTitleInput, r1.x + 10 - titleOffset, r1.y + 10, 25, BLACK);

    if (strlen(movieTitleInput) == 0)
        DrawText("Title...", r1.x + 10 - titleOffset, r1.y + 10, 25, GRAY);

    if (activeField == 0 && ((int)(caretTimer * 2) % 2 == 0)) {
        int tw = MeasureText(movieTitleInput, 25);
        int cx = r1.x + 10 + tw - titleOffset;
        DrawRectangle(cx, r1.y + 10, 2, 25, BLACK);
    }

    DrawText("DETAILS", r2.x, r2.y - 25, 18, DARKGRAY);
    DrawRectangleRec(r2, LIGHTGRAY);
    DrawText(movieInfoInput, r2.x + 10 - infoOffset, r2.y + 10, 25, BLACK);

    if (strlen(movieInfoInput) == 0)
        DrawText("Details...", r2.x + 10 - infoOffset, r2.y + 10, 25, GRAY);

    if (activeField == 1 && ((int)(caretTimer * 2) % 2 == 0)) {
        int tw = MeasureText(movieInfoInput, 25);
        int cx = r2.x + 10 + tw - infoOffset;
        DrawRectangle(cx, r2.y + 10, 2, 25, BLACK);
    }

    DrawText("PRICE (EUR)", r3.x, r3.y - 25, 18, DARKGRAY);
    DrawRectangleRec(r3, LIGHTGRAY);
    DrawText(moviePriceInput, r3.x + 10 - priceOffset, r3.y + 10, 25, BLACK);

    if (strlen(moviePriceInput) == 0)
        DrawText("0.00", r3.x + 10 - priceOffset, r3.y + 10, 25, GRAY);

    if (activeField == 2 && ((int)(caretTimer * 2) % 2 == 0)) {
        int tw = MeasureText(moviePriceInput, 25);
        int cx = r3.x + 10 + tw - priceOffset;
        DrawRectangle(cx, r3.y + 10, 2, 25, BLACK);
    }

    UI::Button({ (float)sw / 2 - 150, (float)sh / 2 + 200, 300, 60 }, "CONFIRM", GREEN, 25);
}


void App::RenderHall() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawRectangle(sw / 2 - 325, 45, 650, 15, DARKGRAY);
    DrawText("SCREEN", sw / 2 - 60, 15, 30, LIGHTGRAY);

    int selectedCount = 0;

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            Color color = seats[r][c].isReserved ? RED : (seats[r][c].isSelected ? LIME : LIGHTGRAY);
            if (seats[r][c].isSelected) selectedCount++;
            DrawRectangleRec(seats[r][c].rect, color);
            DrawRectangleLinesEx(seats[r][c].rect, 2, BLACK);
        }
    }

    DrawRectangle(20, sh - 150, 220, 130, Fade(LIGHTGRAY, 0.5f));
    DrawRectangle(30, sh - 135, 20, 20, LIGHTGRAY);
    DrawText("Available", 60, sh - 135, 20, DARKGRAY);
    DrawRectangle(30, sh - 105, 20, 20, LIME);
    DrawText("Selected", 60, sh - 105, 20, DARKGRAY);
    DrawRectangle(30, sh - 75, 20, 20, RED);
    DrawText("Reserved", 60, sh - 75, 20, DARKGRAY);

    float totalPrice = selectedCount * bookingMgr.GetSelectedMovie().price;
    DrawText(TextFormat("Selected: %d", selectedCount), sw - 275, sh - 150, 25, DARKGRAY);
    DrawText(TextFormat("Total: %.2f EUR", totalPrice), sw - 275, sh - 120, 30, MAROON);
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
        else if (state == BLL::ADD_MOVIE) RenderAddMovie();

        EndDrawing();
    }
}
