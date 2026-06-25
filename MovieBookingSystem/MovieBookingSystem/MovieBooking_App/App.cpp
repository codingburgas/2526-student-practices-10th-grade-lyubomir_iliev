#include "App.h"
#include "UIComponents.h"
#include <string>
#include <fstream>
#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <sstream>

App::App(int width, int height, const char* title) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, title);
    SetTargetFPS(60);

    infoVisibleIdx = -1;
    showError = false;
    searching = false;
    activeField = -1;
    caretTimer = 0.0;
    searchOffset = 0;
    passwordOffset = 0;
    titleOffset = 0;
    infoOffset = 0;
    priceOffset = 0;
    selectedDay = 0;
    selectedShowId = -1;

    passwordInput[0] = '\0';
    searchInput[0] = '\0';
    movieTitleInput[0] = '\0';
    movieInfoInput[0] = '\0';
    moviePriceInput[0] = '\0';

    InitSeats();
}

App::~App() {
    CloseWindow();
}

void App::InitSeats() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    float seatSize = 45.0f;
    float startX = (sw - (COLS * seatSize + (COLS - 1) * 8)) / 2;
    float startY = 100.0f;

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            seats[r][c].rect.x = startX + c * (seatSize + 8);
            seats[r][c].rect.y = startY + r * (seatSize + 8);
            seats[r][c].rect.width = seatSize;
            seats[r][c].rect.height = seatSize;
            seats[r][c].isReserved = false;
            seats[r][c].isSelected = false;
        }
    }
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
        int seatNum;
        while (file >> seatNum) {
            int r = (seatNum - 1) / COLS;
            int c = (seatNum - 1) % COLS;
            if (r < ROWS && c < COLS) {
                seats[r][c].isReserved = true;
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

    BLL::ScreenState state = bookingMgr.GetCurrentScreen();

    if (state == BLL::LOGIN) {
        if (UI::Button({ (float)sw / 2 - 150, (float)sh / 2 - 140, 300, 70 }, "SPECTATOR", BLUE, 28)) {
            bookingMgr.LoginAsSpectator();
        }

        Rectangle passRect = { (float)sw / 2 - 150, (float)sh / 2 + 30, 300, 50 };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            activeField = CheckCollisionPointRec(m, passRect) ? 1 : -1;
        }

        if (activeField == 1) {
            int key = GetCharPressed();
            while (key > 0) {
                int len = strlen(passwordInput);
                if (key >= 32 && key <= 125 && len < 31) {
                    passwordInput[len] = key;
                    passwordInput[len + 1] = '\0';
                }
                key = GetCharPressed();
            }
        }

        if (UI::Button({ (float)sw / 2 - 150, (float)sh / 2 + 100, 300, 60 }, "ADMIN LOGIN", RED, 28) || IsKeyPressed(KEY_ENTER)) {
            if (!bookingMgr.CheckAdminPassword(passwordInput)) {
                showError = true;
            }
            else {
                showError = false;
                passwordInput[0] = '\0';
                activeField = -1;
            }
        }
        return;
    }

    if (state == BLL::CATALOG) {
        Rectangle searchRect = { (float)sw - 400, 20, 220, 35 };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            if (CheckCollisionPointRec(m, searchRect)) {
                activeField = 2;
            }
            else {
                activeField = -1;
            }
        }

        if (activeField == 2) {
            int key = GetCharPressed();
            while (key > 0) {
                int len = strlen(searchInput);
                if (key >= 32 && key <= 125 && len < 63) {
                    searchInput[len] = key;
                    searchInput[len + 1] = '\0';
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                int len = strlen(searchInput);
                if (len > 0) {
                    searchInput[len - 1] = '\0';
                }
            }
        }
        return;
    }

    if (state == BLL::ADD_MOVIE) {
        Rectangle r1 = { (float)sw / 2 - 300, (float)sh / 2 - 65, 600, 45 };
        Rectangle r2 = { (float)sw / 2 - 300, (float)sh / 2 + 20, 600, 45 };
        Rectangle r3 = { (float)sw / 2 - 300, (float)sh / 2 + 105, 200, 45 };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            if (CheckCollisionPointRec(m, r1)) activeField = 3;
            else if (CheckCollisionPointRec(m, r2)) activeField = 4;
            else if (CheckCollisionPointRec(m, r3)) activeField = 5;
            else activeField = -1;
        }

        char* buf = nullptr;
        int maxLen = 0;
        if (activeField == 3) { buf = movieTitleInput; maxLen = 63; }
        else if (activeField == 4) { buf = movieInfoInput; maxLen = 255; }
        else if (activeField == 5) { buf = moviePriceInput; maxLen = 31; }

        if (buf) {
            int key = GetCharPressed();
            while (key > 0) {
                int len = strlen(buf);
                if (key >= 32 && key <= 125 && len < maxLen) {
                    buf[len] = (char)key;
                    buf[len + 1] = '\0';
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                int len = strlen(buf);
                if (len > 0) {
                    buf[len - 1] = '\0';
                }
            }
        }
        return;
    }

    if (state == BLL::BOOKING) {
        Vector2 mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (CheckCollisionPointRec(mouse, seats[r][c].rect)) {
                        if (!seats[r][c].isReserved) {
                            seats[r][c].isSelected = !seats[r][c].isSelected;
                        }
                    }
                }
            }
        }

        if (UI::Button({ (float)sw / 2 - 100, (float)sh - 60, 200, 40 }, "CONFIRM", GREEN, 20)) {
            std::vector<int> selectedSeats;
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (seats[r][c].isSelected) {
                        selectedSeats.push_back(r * COLS + c + 1);
                    }
                }
            }
            if (!selectedSeats.empty()) {
                bookingMgr.BookSeats(selectedSeats);
                ResetSeats();
            }
        }

        if (UI::Button({ (float)sw / 2 - 100, (float)sh - 110, 200, 40 }, "BACK", GRAY, 20)) {
            ResetSeats();
            bookingMgr.SetScreen(BLL::CATALOG);
        }
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
    DrawText(dots.c_str(), passRect.x + 10, passRect.y + 10, 30, DARKGRAY);

    if (strlen(passwordInput) == 0) {
        DrawText("Password...", passRect.x + 10, passRect.y + 12, 25, GRAY);
    }

    if (showError) {
        DrawText("Wrong password!", sw / 2 - 80, sh / 2 + 180, 20, RED);
    }
}

void App::RenderCatalog() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawText("SHOWTIMES", 40, 20, 30, DARKGRAY);

    const char* days[] = { "TODAY", "TOMORROW", "SUN", "MON", "TUE", "WED", "THU" };
    float dayX = 40;
    float dayY = 60;

    for (int d = 0; d < 7; d++) {
        Rectangle dayRect = { dayX, dayY, 75, 35 };
        Color dayColor = (d == selectedDay) ? ORANGE : LIGHTGRAY;

        if (UI::Button(dayRect, days[d], dayColor, 14)) {
            selectedDay = d;
            bookingMgr.selectedDayIndex = d;
        }
        dayX += 85;
    }

    const char* dateStrings[] = {
        "FRIDAY 26/06/2026",     
        "SATURDAY 27/06/2026",   
        "SUNDAY 28/06/2026",     
        "MONDAY 29/06/2026",     
        "TUESDAY 30/06/2026",    
        "WEDNESDAY 01/07/2026",  
        "THURSDAY 02/07/2026"   
    };
    DrawText(dateStrings[selectedDay], 40, dayY + 50, 16, GRAY);
    DrawLine(40, dayY + 75, sw - 40, dayY + 75, LIGHTGRAY);

    Rectangle searchRect = { (float)sw - 400, 20, 220, 35 };
    DrawRectangleRec(searchRect, LIGHTGRAY);
    DrawRectangleLinesEx(searchRect, 2, DARKGRAY);
    DrawText(searchInput[0] ? searchInput : "Search...", searchRect.x + 10, searchRect.y + 8, 20, searchInput[0] ? BLACK : GRAY);

    std::vector<BLL::Movie> searchResults;
    if (searchInput[0] != '\0') {
        searchResults = bookingMgr.SearchMovies(searchInput);
    }
    else {
        searchResults = bookingMgr.GetMovies();
    }

    std::vector<BLL::ShowTime> shows = bookingMgr.GetShowtimesForDay(selectedDay);
    float currentY = dayY + 100;

    if (shows.empty()) {
        DrawText("No showtimes for this day", sw / 2 - 100, sh / 2, 20, GRAY);
    }

    for (int i = 0; i < (int)searchResults.size(); i++) {
        BLL::Movie& movie = searchResults[i];

        bool hasShows = false;
        for (int j = 0; j < (int)shows.size(); j++) {
            if (shows[j].movieId == movie.id) { hasShows = true; break; }
        }
        if (!hasShows) continue;

        Rectangle card = { 40, currentY, (float)sw - 80, 130 };
        DrawRectangleRec(card, RAYWHITE);
        DrawRectangleLinesEx(card, 1, LIGHTGRAY);

        DrawRectangle(card.x + 10, card.y + 10, 80, 100, DARKGRAY);
        DrawText("POSTER", card.x + 20, card.y + 55, 15, LIGHTGRAY);

        DrawText(movie.title.c_str(), card.x + 110, card.y + 10, 22, BLACK);
        DrawText(movie.info.c_str(), card.x + 110, card.y + 40, 13, GRAY);

        float timeX = card.x + 110;
        float timeY = card.y + 70;

        for (int j = 0; j < (int)shows.size(); j++) {
            BLL::ShowTime& s = shows[j];
            if (s.movieId != movie.id) continue;

            std::string label = s.is3D ? "3D" : "2D";
            DrawText(label.c_str(), timeX, timeY - 12, 10, s.is3D ? ORANGE : BLUE);

            Rectangle timeRect = { timeX, timeY, 65, 40 };
            if (UI::Button(timeRect, s.time.c_str(), ORANGE, 16)) {
                selectedShowId = s.id;
                LoadSeatingPlan(movie.title, s.time);
                bookingMgr.SelectMovie(movie.id - 1);
            }

            timeX += 80;
        }

        currentY += 150;
    }

    if (UI::Button({ (float)sw - 150, 20, 120, 35 }, "LOGOUT", GRAY, 16)) {
        bookingMgr.Logout();
        activeField = -1;
        searchInput[0] = '\0';
    }

    if (bookingMgr.IsAdmin()) {
        if (UI::Button({ (float)sw - 220, (float)sh - 60, 200, 40 }, "NEW MOVIE", ORANGE, 18)) {
            bookingMgr.SetScreen(BLL::ADD_MOVIE);
        }
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
    DrawText(movieTitleInput[0] ? movieTitleInput : "Title...", r1.x + 10, r1.y + 10, 25, movieTitleInput[0] ? BLACK : GRAY);

    DrawText("DETAILS", r2.x, r2.y - 25, 18, DARKGRAY);
    DrawRectangleRec(r2, LIGHTGRAY);
    DrawText(movieInfoInput[0] ? movieInfoInput : "Details...", r2.x + 10, r2.y + 10, 25, movieInfoInput[0] ? BLACK : GRAY);

    DrawText("PRICE (EUR)", r3.x, r3.y - 25, 18, DARKGRAY);
    DrawRectangleRec(r3, LIGHTGRAY);
    DrawText(moviePriceInput[0] ? moviePriceInput : "0.00", r3.x + 10, r3.y + 10, 25, moviePriceInput[0] ? BLACK : GRAY);

    if (UI::Button({ (float)sw / 2 - 150, (float)sh / 2 + 200, 300, 60 }, "CONFIRM", GREEN, 25)) {
        if (strlen(movieTitleInput) > 0 && strlen(movieInfoInput) > 0 && strlen(moviePriceInput) > 0) {
            float price = std::stof(moviePriceInput);
            bookingMgr.AddMovie(movieTitleInput, movieInfoInput, price);
            movieTitleInput[0] = '\0';
            movieInfoInput[0] = '\0';
            moviePriceInput[0] = '\0';
            bookingMgr.SetScreen(BLL::CATALOG);
        }
    }

    if (UI::Button({ (float)sw / 2 - 150, (float)sh / 2 + 270, 300, 40 }, "CANCEL", GRAY, 20)) {
        movieTitleInput[0] = '\0';
        movieInfoInput[0] = '\0';
        moviePriceInput[0] = '\0';
        bookingMgr.SetScreen(BLL::CATALOG);
    }
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

            int seatNum = r * COLS + c + 1;
            DrawText(std::to_string(seatNum).c_str(),
                seats[r][c].rect.x + 15,
                seats[r][c].rect.y + 15,
                12, DARKGRAY);
        }
    }

    DrawRectangle(20, sh - 150, 220, 130, ColorAlpha(LIGHTGRAY, 0.5f));
    DrawRectangle(30, sh - 135, 20, 20, LIGHTGRAY);
    DrawText("Available", 60, sh - 135, 20, DARKGRAY);
    DrawRectangle(30, sh - 105, 20, 20, LIME);
    DrawText("Selected", 60, sh - 105, 20, DARKGRAY);
    DrawRectangle(30, sh - 75, 20, 20, RED);
    DrawText("Reserved", 60, sh - 75, 20, DARKGRAY);

    float totalPrice = selectedCount * bookingMgr.GetSelectedMovie().price;

    std::string text = "Selected: " + std::to_string(selectedCount);
    DrawText(text.c_str(), sw - 275, sh - 150, 25, DARKGRAY);

    char priceBuffer[50];
    sprintf_s(priceBuffer, "Total: %.2f EUR", totalPrice);
    DrawText(priceBuffer, sw - 275, sh - 120, 30, MAROON);
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