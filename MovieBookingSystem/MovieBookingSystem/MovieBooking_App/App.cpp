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
    selectedGenre = -1;
    seatSize = 42.0f;
    seatsInitialized = false;

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
    float gap = 6.0f;

    if (sw > 1400) seatSize = 48.0f;
    else if (sw > 1000) seatSize = 42.0f;
    else seatSize = 36.0f;

    float totalWidth = COLS * seatSize + (COLS - 1) * gap;
    float totalHeight = ROWS * seatSize + (ROWS - 1) * gap;
    float startX = (sw - totalWidth) / 2;
    float startY = (sh - totalHeight) / 2 - 20;

    bool oldReserved[ROWS][COLS];
    bool oldSelected[ROWS][COLS];

    if (seatsInitialized) {
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                oldReserved[r][c] = seats[r][c].isReserved;
                oldSelected[r][c] = seats[r][c].isSelected;
            }
        }
    }

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            seats[r][c].rect.x = startX + c * (seatSize + gap);
            seats[r][c].rect.y = startY + r * (seatSize + gap);
            seats[r][c].rect.width = seatSize;
            seats[r][c].rect.height = seatSize;
            if (seatsInitialized) {
                seats[r][c].isReserved = oldReserved[r][c];
                seats[r][c].isSelected = oldSelected[r][c];
            }
            else {
                seats[r][c].isReserved = false;
                seats[r][c].isSelected = false;
            }
        }
    }

    seatsInitialized = true;
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

            static float bsTimer = 0.0f;
            static bool bsHeld = false;
            if (IsKeyDown(KEY_BACKSPACE)) {
                if (!bsHeld) {
                    int len = strlen(passwordInput);
                    if (len > 0) passwordInput[len - 1] = '\0';
                    bsHeld = true;
                    bsTimer = 0.0f;
                }
                else {
                    bsTimer += dt;
                    if (bsTimer > 0.15f) {
                        int len = strlen(passwordInput);
                        if (len > 0) passwordInput[len - 1] = '\0';
                        bsTimer = 0.0f;
                    }
                }
            }
            else {
                bsHeld = false;
                bsTimer = 0.0f;
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
        Rectangle searchRect = { (float)sw - 400, 20, 240, 40 };

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

            static float bsTimer = 0.0f;
            static bool bsHeld = false;
            if (IsKeyDown(KEY_BACKSPACE)) {
                if (!bsHeld) {
                    int len = strlen(searchInput);
                    if (len > 0) searchInput[len - 1] = '\0';
                    bsHeld = true;
                    bsTimer = 0.0f;
                }
                else {
                    bsTimer += dt;
                    if (bsTimer > 0.15f) {
                        int len = strlen(searchInput);
                        if (len > 0) searchInput[len - 1] = '\0';
                        bsTimer = 0.0f;
                    }
                }
            }
            else {
                bsHeld = false;
                bsTimer = 0.0f;
            }
        }
        return;
    }

    if (state == BLL::ADD_MOVIE) {
        Rectangle r1 = { (float)sw / 2 - 300, (float)sh / 2 - 120, 600, 45 };
        Rectangle r3 = { (float)sw / 2 - 300, (float)sh / 2 - 40, 200, 45 };

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            if (CheckCollisionPointRec(m, r1)) activeField = 3;
            else if (CheckCollisionPointRec(m, r3)) activeField = 5;
            else activeField = -1;
        }

        if (activeField == 3) {
            int key = GetCharPressed();
            while (key > 0) {
                int len = strlen(movieTitleInput);
                if (key >= 32 && key <= 125 && len < 63) {
                    movieTitleInput[len] = key;
                    movieTitleInput[len + 1] = '\0';
                }
                key = GetCharPressed();
            }

            static float bsTimer = 0.0f;
            static bool bsHeld = false;
            if (IsKeyDown(KEY_BACKSPACE)) {
                if (!bsHeld) {
                    int len = strlen(movieTitleInput);
                    if (len > 0) movieTitleInput[len - 1] = '\0';
                    bsHeld = true;
                    bsTimer = 0.0f;
                }
                else {
                    bsTimer += dt;
                    if (bsTimer > 0.15f) {
                        int len = strlen(movieTitleInput);
                        if (len > 0) movieTitleInput[len - 1] = '\0';
                        bsTimer = 0.0f;
                    }
                }
            }
            else {
                bsHeld = false;
                bsTimer = 0.0f;
            }
        }

        if (activeField == 5) {
            int key = GetCharPressed();
            while (key > 0) {
                int len = strlen(moviePriceInput);
                if (key >= 32 && key <= 125 && len < 31) {
                    moviePriceInput[len] = key;
                    moviePriceInput[len + 1] = '\0';
                }
                key = GetCharPressed();
            }

            static float bsTimer = 0.0f;
            static bool bsHeld = false;
            if (IsKeyDown(KEY_BACKSPACE)) {
                if (!bsHeld) {
                    int len = strlen(moviePriceInput);
                    if (len > 0) moviePriceInput[len - 1] = '\0';
                    bsHeld = true;
                    bsTimer = 0.0f;
                }
                else {
                    bsTimer += dt;
                    if (bsTimer > 0.15f) {
                        int len = strlen(moviePriceInput);
                        if (len > 0) moviePriceInput[len - 1] = '\0';
                        bsTimer = 0.0f;
                    }
                }
            }
            else {
                bsHeld = false;
                bsTimer = 0.0f;
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

    if (strlen(passwordInput) > 0) {
        std::string dots;
        for (int i = 0; i < (int)strlen(passwordInput); i++) {
            dots += "●";
        }
        int textWidth = MeasureText(dots.c_str(), 30);
        DrawText(dots.c_str(), passRect.x + 10, passRect.y + 10, 30, DARKGRAY);

        if (activeField == 1 && ((int)(caretTimer * 2) % 2 == 0)) {
            int cursorX = passRect.x + 10 + textWidth;
            DrawRectangle(cursorX, passRect.y + 10, 2, 30, DARKGRAY);
        }
    }
    else {
        DrawText("Password...", passRect.x + 10, passRect.y + 12, 25, GRAY);
    }

    if (showError) {
        DrawText("Wrong password!", sw / 2 - 80, sh / 2 + 180, 20, RED);
    }
}

void App::RenderCatalog() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawText("SHOWTIMES", 40, 20, 35, DARKGRAY);

    const char* days[] = { "TODAY", "TOMORROW", "SUN", "MON", "TUE", "WED", "THU" };
    float dayX = 40;
    float dayY = 65;

    for (int d = 0; d < 7; d++) {
        Rectangle dayRect = { dayX, dayY, 85, 40 };
        Color dayColor = (d == selectedDay) ? ORANGE : LIGHTGRAY;

        if (UI::Button(dayRect, days[d], dayColor, 16)) {
            selectedDay = d;
            bookingMgr.selectedDayIndex = d;
        }
        dayX += 95;
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
    DrawText(dateStrings[selectedDay], 40, dayY + 55, 18, GRAY);
    DrawLine(40, dayY + 85, sw - 40, dayY + 85, LIGHTGRAY);

    Rectangle searchRect = { (float)sw - 400, 20, 240, 40 };
    DrawRectangleRec(searchRect, LIGHTGRAY);
    DrawRectangleLinesEx(searchRect, 2, DARKGRAY);

    int textWidth = MeasureText(searchInput, 22);
    DrawText(searchInput[0] ? searchInput : "Search...", searchRect.x + 12, searchRect.y + 10, 22, searchInput[0] ? BLACK : GRAY);

    if (activeField == 2 && ((int)(caretTimer * 2) % 2 == 0)) {
        int cursorX = searchRect.x + 12 + textWidth;
        DrawRectangle(cursorX, searchRect.y + 10, 2, 22, BLACK);
    }

    std::vector<BLL::Movie> searchResults;
    if (searchInput[0] != '\0') {
        searchResults = bookingMgr.SearchMovies(searchInput);
    }
    else {
        searchResults = bookingMgr.GetMovies();
    }

    std::vector<BLL::ShowTime> shows = bookingMgr.GetShowtimesForDay(selectedDay);
    float currentY = dayY + 110;

    if (shows.empty()) {
        DrawText("No showtimes for this day", sw / 2 - 120, sh / 2, 24, GRAY);
    }

    for (int i = 0; i < (int)searchResults.size(); i++) {
        BLL::Movie& movie = searchResults[i];

        bool hasShows = false;
        for (int j = 0; j < (int)shows.size(); j++) {
            if (shows[j].movieId == movie.id) { hasShows = true; break; }
        }
        if (!hasShows) continue;

        Rectangle card = { 40, currentY, (float)sw - 80, 150 };
        DrawRectangleRec(card, RAYWHITE);
        DrawRectangleLinesEx(card, 2, LIGHTGRAY);

        // ЗАРЕЖДАНЕ НА СНИМКА
        std::string imgPath = "posters/" + movie.title + ".png";
        std::replace(imgPath.begin(), imgPath.end(), ' ', '_');
        Texture2D poster = LoadTexture(imgPath.c_str());

        if (poster.id != 0) {
            float posterWidth = 100;
            float posterHeight = 120;
            float scaleX = posterWidth / poster.width;
            float scaleY = posterHeight / poster.height;
            float scale = (scaleX < scaleY) ? scaleX : scaleY;
            float drawWidth = poster.width * scale;
            float drawHeight = poster.height * scale;
            float drawX = card.x + 15 + (100 - drawWidth) / 2;
            float drawY = card.y + 15 + (120 - drawHeight) / 2;
            DrawTexturePro(poster,
                { 0, 0, (float)poster.width, (float)poster.height },
                { drawX, drawY, drawWidth, drawHeight },
                { 0, 0 }, 0, WHITE);
            UnloadTexture(poster);
        }
        else {
            DrawRectangle(card.x + 15, card.y + 15, 100, 120, DARKGRAY);
            DrawText("POSTER", card.x + 25, card.y + 70, 16, LIGHTGRAY);
        }

        DrawText(movie.title.c_str(), card.x + 135, card.y + 15, 26, BLACK);
        DrawText(movie.genre.c_str(), card.x + 135, card.y + 50, 18, BLUE);

        if (bookingMgr.IsAdmin()) {
            if (UI::Button({ card.x + card.width - 90, card.y + 10, 70, 30 }, "DEL", RED, 14)) {
                bookingMgr.DeleteMovieById(movie.id);
                return;
            }
        }

        std::string priceText = std::to_string((int)movie.price) + " EUR";
        DrawText(priceText.c_str(), card.x + 135, card.y + 75, 16, GRAY);

        float timeX = card.x + 135;
        float timeY = card.y + 105;

        for (int j = 0; j < (int)shows.size(); j++) {
            BLL::ShowTime& s = shows[j];
            if (s.movieId != movie.id) continue;

            std::string label = s.is3D ? "3D" : "2D";
            DrawText(label.c_str(), timeX, timeY - 14, 12, s.is3D ? ORANGE : BLUE);

            Rectangle timeRect = { timeX, timeY, 75, 40 };
            if (UI::Button(timeRect, s.time.c_str(), ORANGE, 16)) {
                selectedShowId = s.id;
                LoadSeatingPlan(movie.title, s.time);
                bookingMgr.SelectMovie(movie.id - 1);
            }

            timeX += 90;
        }

        currentY += 170;
    }

    if (UI::Button({ (float)sw - 150, 20, 130, 40 }, "LOGOUT", GRAY, 16)) {
        bookingMgr.Logout();
        activeField = -1;
        searchInput[0] = '\0';
    }

    if (bookingMgr.IsAdmin()) {
        if (UI::Button({ (float)sw - 240, (float)sh - 70, 220, 45 }, "NEW MOVIE", ORANGE, 18)) {
            bookingMgr.SetScreen(BLL::ADD_MOVIE);
        }
    }
}
void App::RenderAddMovie() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawRectangle(0, 0, sw, sh, RAYWHITE);
    DrawText("ADD NEW MOVIE", sw / 2 - 160, 30, 40, MAROON);

    Rectangle r1 = { (float)sw / 2 - 300, (float)sh / 2 - 140, 600, 40 };
    Rectangle r3 = { (float)sw / 2 - 300, (float)sh / 2 - 70, 200, 40 };

    DrawText("TITLE", r1.x, r1.y - 22, 16, DARKGRAY);
    DrawRectangleRec(r1, LIGHTGRAY);
    DrawText(movieTitleInput[0] ? movieTitleInput : "Title...", r1.x + 10, r1.y + 8, 22, movieTitleInput[0] ? BLACK : GRAY);

    if (activeField == 3 && ((int)(caretTimer * 2) % 2 == 0)) {
        int cursorX = r1.x + 10 + MeasureText(movieTitleInput, 22);
        DrawRectangle(cursorX, r1.y + 8, 2, 22, BLACK);
    }

    DrawText("PRICE (EUR)", r3.x, r3.y - 22, 16, DARKGRAY);
    DrawRectangleRec(r3, LIGHTGRAY);
    DrawText(moviePriceInput[0] ? moviePriceInput : "0.00", r3.x + 10, r3.y + 8, 22, moviePriceInput[0] ? BLACK : GRAY);

    if (activeField == 5 && ((int)(caretTimer * 2) % 2 == 0)) {
        int cursorX = r3.x + 10 + MeasureText(moviePriceInput, 22);
        DrawRectangle(cursorX, r3.y + 8, 2, 22, BLACK);
    }

    DrawText("GENRE", sw / 2 - 300, r3.y + 55, 16, DARKGRAY);

    const char* genres[] = { "Action", "Comedy", "Drama", "Horror", "Sci-Fi", "Animation", "Romance", "Thriller" };
    float genreX = sw / 2 - 300;
    float genreY = r3.y + 78;

    for (int i = 0; i < 8; i++) {
        Rectangle genreRect = { genreX, genreY, 90, 32 };
        Color genreColor = (selectedGenre == i) ? ORANGE : LIGHTGRAY;

        if (UI::Button(genreRect, genres[i], genreColor, 12)) {
            selectedGenre = i;
        }

        genreX += 100;
        if ((i + 1) % 4 == 0) {
            genreX = sw / 2 - 300;
            genreY += 40;
        }
    }

    DrawText("FORMAT", sw / 2 - 300, genreY + 30, 16, DARKGRAY);

    const char* formats[] = { "2D", "3D" };
    float formatX = sw / 2 - 300;
    float formatY = genreY + 52;

    static int selectedFormat = 0;

    for (int i = 0; i < 2; i++) {
        Rectangle formatRect = { formatX, formatY, 100, 35 };
        Color formatColor = (selectedFormat == i) ? ORANGE : LIGHTGRAY;

        if (UI::Button(formatRect, formats[i], formatColor, 16)) {
            selectedFormat = i;
        }
        formatX += 120;
    }

    DrawText("POSTER IMAGE", sw / 2 - 300, formatY + 55, 16, DARKGRAY);

    DrawText("Place .png file in 'posters' folder", sw / 2 - 300, formatY + 80, 13, GRAY);
    DrawText("with the same name as the movie title", sw / 2 - 300, formatY + 98, 13, GRAY);
    DrawText("Example: 'The_Batman.png'", sw / 2 - 300, formatY + 116, 13, BLUE);

    if (UI::Button({ (float)sw / 2 - 150, (float)sh / 2 + 200, 300, 55 }, "CONFIRM", GREEN, 24)) {
        if (strlen(movieTitleInput) > 0 && strlen(moviePriceInput) > 0 && selectedGenre >= 0) {
            float price = std::stof(moviePriceInput);
            const char* genreNames[] = { "Action", "Comedy", "Drama", "Horror", "Sci-Fi", "Animation", "Romance", "Thriller" };

            std::string imgName = std::string(movieTitleInput) + ".png";
            std::replace(imgName.begin(), imgName.end(), ' ', '_');

            bookingMgr.AddMovie(movieTitleInput, "", price, genreNames[selectedGenre], "", "", imgName);

            movieTitleInput[0] = '\0';
            moviePriceInput[0] = '\0';
            selectedGenre = -1;
            bookingMgr.SetScreen(BLL::CATALOG);
        }
    }

    if (UI::Button({ (float)sw / 2 - 150, (float)sh / 2 + 270, 300, 40 }, "CANCEL", GRAY, 20)) {
        movieTitleInput[0] = '\0';
        moviePriceInput[0] = '\0';
        selectedGenre = -1;
        bookingMgr.SetScreen(BLL::CATALOG);
    }
}
void App::RenderHall() {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawText("SCREEN", sw / 2 - 60, 20, 30, DARKGRAY);
    DrawRectangle(sw / 2 - 250, 50, 500, 8, DARKGRAY);

    int selectedCount = 0;

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            Color color = seats[r][c].isReserved ? RED : (seats[r][c].isSelected ? LIME : LIGHTGRAY);
            if (seats[r][c].isSelected) selectedCount++;
            DrawRectangleRec(seats[r][c].rect, color);
            DrawRectangleLinesEx(seats[r][c].rect, 2, DARKGRAY);

            int seatNum = r * COLS + c + 1;
            int fontSize = (seatSize > 35) ? 13 : 10;
            int textX = seats[r][c].rect.x + (seats[r][c].rect.width - MeasureText(std::to_string(seatNum).c_str(), fontSize)) / 2;
            int textY = seats[r][c].rect.y + (seats[r][c].rect.height - fontSize) / 2;
            DrawText(std::to_string(seatNum).c_str(), textX, textY, fontSize, DARKGRAY);
        }
    }

    int legendX = 30;
    int legendY = sh - 140;
    DrawRectangle(legendX, legendY, 210, 120, Fade(LIGHTGRAY, 0.5f));
    DrawRectangle(legendX + 10, legendY + 10, 22, 22, LIGHTGRAY);
    DrawRectangleLinesEx({ (float)legendX + 10, (float)legendY + 10, 22, 22 }, 1, DARKGRAY);
    DrawText("Available", legendX + 40, legendY + 10, 18, DARKGRAY);
    DrawRectangle(legendX + 10, legendY + 45, 22, 22, LIME);
    DrawRectangleLinesEx({ (float)legendX + 10, (float)legendY + 45, 22, 22 }, 1, DARKGRAY);
    DrawText("Selected", legendX + 40, legendY + 45, 18, DARKGRAY);
    DrawRectangle(legendX + 10, legendY + 80, 22, 22, RED);
    DrawRectangleLinesEx({ (float)legendX + 10, (float)legendY + 80, 22, 22 }, 1, DARKGRAY);
    DrawText("Reserved", legendX + 40, legendY + 80, 18, DARKGRAY);

    float totalPrice = selectedCount * bookingMgr.GetSelectedMovie().price;
    std::string text = "Selected: " + std::to_string(selectedCount);
    DrawText(text.c_str(), sw - 200, sh - 120, 20, DARKGRAY);
    char priceBuffer[50];
    sprintf_s(priceBuffer, "Total: %.2f EUR", totalPrice);
    DrawText(priceBuffer, sw - 200, sh - 90, 22, MAROON);

    float btnW = 130;
    float btnH = 38;
    float gap = 20;
    float totalW = btnW * 2 + gap;
    float startX = (sw - totalW) / 2;
    float startY = sh - 55;

    if (UI::Button({ startX, startY, btnW, btnH }, "BACK", GRAY, 16)) {
        ResetSeats();
        bookingMgr.SetScreen(BLL::CATALOG);
    }

    if (UI::Button({ startX + btnW + gap, startY, btnW, btnH }, "CONFIRM", GREEN, 16)) {
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
}

void App::Run() {
    int lastWidth = GetScreenWidth();
    int lastHeight = GetScreenHeight();

    while (!WindowShouldClose()) {
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();

        if (sw != lastWidth || sh != lastHeight) {
            InitSeats();
            lastWidth = sw;
            lastHeight = sh;
        }

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