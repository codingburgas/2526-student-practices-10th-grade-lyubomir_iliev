#include "App.h"
#include "UIComponents.h"
#include <string>
#include <fstream>
#include <algorithm>

namespace BLL {
    void BookingManager::LoginAsSpectator() { screen = CATALOG; adminMode = false; }
    void BookingManager::Logout() { screen = LOGIN; adminMode = false; }
    bool BookingManager::IsAdmin() { return adminMode; }
    ScreenState BookingManager::GetCurrentScreen() { return screen; }
    void BookingManager::SetScreen(ScreenState s) { screen = s; }
    std::vector<Movie>& BookingManager::GetMovies() { return movies; }
    Movie& BookingManager::GetSelectedMovie() { return movies[selectedIdx]; }
    void BookingManager::SelectMovie(int idx) { selectedIdx = idx; screen = BOOKING; }

    bool BookingManager::CheckAdminPassword(const char* pass) {
        if (std::string(pass) == "admin123") {
            adminMode = true;
            screen = CATALOG;
            return true;
        }
        return false;
    }

    void BookingManager::AddMovie(std::string t, std::string i, float p) {
        movies.push_back({ t, i, p });
    }

    void BookingManager::DeleteMovie(int index) {
        if (index >= 0 && index < (int)movies.size()) {
            movies.erase(movies.begin() + index);
        }
    }

    void BookingManager::FinalizeBooking(std::vector<int> states) {
        screen = CATALOG;
    }
}

App::App(int width, int height, const char* title) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, title);
    SetTargetFPS(60);
    infoVisibleIdx = -1;
    ResetSeats();

    bookingMgr.AddMovie("The Batman", "Action | 2h 56m", 12.50f);
    bookingMgr.AddMovie("Dune: Part Two", "Sci-Fi | 2h 46m", 15.00f);
    bookingMgr.AddMovie("Oppenheimer", "Drama | 3h 00m", 11.00f);
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
    std::string cleanTime = time;
    std::replace(cleanTime.begin(), cleanTime.end(), ':', '-');
    std::string filename = movieTitle + "_" + cleanTime + "_seats.txt";
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

        if (UI::Button({ (float)sw / 2 - 150, (float)sh / 2 + 100, 300, 60 }, "ADMIN LOGIN", RED, 28) || IsKeyPressed(KEY_ENTER)) {
            if (!bookingMgr.CheckAdminPassword(passwordInput)) showError = true;
            else { showError = false; letterCount = 0; passwordInput[0] = '\0'; }
        }
    }
    else if (bookingMgr.GetCurrentScreen() == BLL::CATALOG) {
        if (bookingMgr.IsAdmin()) {
            if (UI::Button({ (float)sw - 220, (float)sh - 60, 200, 40 }, "NEW MOVIE", ORANGE, 20)) {
                bookingMgr.SetScreen(BLL::ADD_MOVIE);
            }
        }

        auto& list = bookingMgr.GetMovies();
        float currentY = 110.0f;
        const char* hours[] = { "18:00", "20:30", "22:15" };

        for (int i = 0; i < (int)list.size(); i++) {
            Rectangle card = { 40, currentY, (float)sw - 80, 160 };
            if (!bookingMgr.IsAdmin()) {
                for (int h = 0; h < 3; h++) {
                    if (UI::Button({ card.x + 120 + (h * 90), card.y + 100, 80, 45 }, hours[h], ORANGE, 20)) {
                        bookingMgr.SelectMovie(i);
                        LoadSeatingPlan(list[i].title, hours[h]);
                        return;
                    }
                }
            }
            if (bookingMgr.IsAdmin() && UI::Button({ card.x + card.width - 80, card.y, 80, 35 }, "DEL", RED, 18)) {
                bookingMgr.DeleteMovie(i);
                return;
            }
            currentY += 190.0f;
        }

        if (UI::Button({ (float)sw - 120, 20, 100, 35 }, "LOGOUT", GRAY, 18)) {
            bookingMgr.Logout();
        }
    }
    else if (bookingMgr.GetCurrentScreen() == BLL::ADD_MOVIE) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            if (CheckCollisionPointRec(m, { (float)sw / 2 - 300, (float)sh / 2 - 65, 600, 45 })) activeField = 0;
            else if (CheckCollisionPointRec(m, { (float)sw / 2 - 300, (float)sh / 2 + 20, 600, 45 })) activeField = 1;
            else if (CheckCollisionPointRec(m, { (float)sw / 2 - 300, (float)sh / 2 + 105, 200, 45 })) activeField = 2;
        }

        int key = GetCharPressed();
        char* currentStr = (activeField == 0) ? movieTitleInput : (activeField == 1) ? movieInfoInput : moviePriceInput;
        while (key > 0) {
            int len = (int)strlen(currentStr);
            if (key >= 32 && key <= 125 && len < 30) {
                currentStr[len] = (char)key;
                currentStr[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = (int)strlen(currentStr);
            if (len > 0) currentStr[len - 1] = '\0';
        }

        if (UI::Button({ (float)sw / 2 - 150, (float)sh / 2 + 200, 300, 60 }, "CONFIRM", GREEN, 25)) {
            if (strlen(movieTitleInput) > 0) {
                bookingMgr.AddMovie(movieTitleInput, movieInfoInput, (float)atof(moviePriceInput));
                movieTitleInput[0] = '\0'; movieInfoInput[0] = '\0'; moviePriceInput[0] = '\0';
                bookingMgr.SetScreen(BLL::CATALOG);
            }
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

        if (UI::Button({ (float)sw - 275, (float)sh - 80, 200, 60 }, "CONFIRM", GREEN, 25)) {
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
            bookingMgr.SetScreen(BLL::CATALOG);
        }
    }
}

void App::RenderLogin() {
    int sw = GetScreenWidth(); int sh = GetScreenHeight();
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
    int sh = GetScreenHeight();
    const char* days[] = { "TODAY", "TUE", "WED", "THU", "FRI", "SAT", "SUN" };
    float dayX = 40;
    for (int d = 0; d < 7; d++) {
        DrawText(days[d], dayX, 40, 22, (d == 0) ? ORANGE : DARKGRAY);
        if (d == 0) DrawRectangle(dayX, 65, 60, 3, ORANGE);
        dayX += 80;
    }
    DrawLine(40, 80, sw - 40, 80, LIGHTGRAY);

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
            if (!bookingMgr.IsAdmin())
                UI::Button({ card.x + 120 + (h * 90), card.y + 100, 80, 45 }, hours[h], ORANGE, 20);
            else {
                DrawRectangle(card.x + 120 + (h * 90), card.y + 100, 80, 45, GRAY);
                DrawText(hours[h], card.x + 135 + (h * 90), card.y + 112, 20, WHITE);
            }
        }
        if (bookingMgr.IsAdmin()) UI::Button({ card.x + card.width - 80, card.y, 80, 35 }, "DEL", RED, 18);
        currentY += 190.0f;
    }

    if (bookingMgr.IsAdmin()) {
        UI::Button({ (float)sw - 220, (float)sh - 60, 200, 40 }, "NEW MOVIE", ORANGE, 20);
    }
}

void App::RenderAddMovie() {
    int sw = GetScreenWidth(); int sh = GetScreenHeight();
    DrawRectangle(0, 0, sw, sh, RAYWHITE);
    DrawText("ADD NEW MOVIE", sw / 2 - 130, 50, 35, MAROON);
    DrawText("TITLE", sw / 2 - 300, sh / 2 - 90, 18, DARKGRAY);
    DrawRectangle(sw / 2 - 300, sh / 2 - 65, 600, 45, LIGHTGRAY);
    DrawText(movieTitleInput, sw / 2 - 290, sh / 2 - 55, 25, BLACK);
    DrawText("DETAILS", sw / 2 - 300, sh / 2 - 5, 18, DARKGRAY);
    DrawRectangle(sw / 2 - 300, sh / 2 + 20, 600, 45, LIGHTGRAY);
    DrawText(movieInfoInput, sw / 2 - 290, sh / 2 + 30, 25, BLACK);
    DrawText("PRICE (EUR)", sw / 2 - 300, sh / 2 + 80, 18, DARKGRAY);
    DrawRectangle(sw / 2 - 300, sh / 2 + 105, 200, 45, LIGHTGRAY);
    DrawText(moviePriceInput, sw / 2 - 290, sh / 2 + 115, 25, BLACK);
    UI::Button({ (float)sw / 2 - 150, (float)sh / 2 + 200, 300, 60 }, "CONFIRM", GREEN, 25);
}

void App::RenderHall() {
    int sw = GetScreenWidth(); int sh = GetScreenHeight();
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
    DrawRectangle(30, sh - 135, 20, 20, LIGHTGRAY); DrawText("Available", 60, sh - 135, 20, DARKGRAY);
    DrawRectangle(30, sh - 105, 20, 20, LIME); DrawText("Selected", 60, sh - 105, 20, DARKGRAY);
    DrawRectangle(30, sh - 75, 20, 20, RED); DrawText("Reserved", 60, sh - 75, 20, DARKGRAY);
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