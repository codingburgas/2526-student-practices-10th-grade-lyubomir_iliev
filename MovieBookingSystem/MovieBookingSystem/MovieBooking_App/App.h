#pragma once
#include "raylib.h"
#include <string>
#include <vector>
#include "BookingManager.h"

class App {
public:
    App(int width, int height, const char* title);
    ~App();
    void Run();

private:
    void HandleInput();
    void RenderLogin();
    void RenderCatalog();
    void RenderHall();
    void RenderAddMovie();
    void ResetSeats();
    void LoadSeatingPlan(std::string movieTitle, std::string time);
    void InitSeats();

    BLL::BookingManager bookingMgr;

    static const int ROWS = 8;
    static const int COLS = 10;

    struct SeatUI {
        Rectangle rect;
        bool isReserved;
        bool isSelected;
    };

    SeatUI seats[ROWS][COLS];

    char passwordInput[32];
    char searchInput[64];
    char movieTitleInput[64];
    char movieInfoInput[256];
    char moviePriceInput[32];

    int activeField;
    int infoVisibleIdx;
    int selectedDay;
    int selectedShowId;
    int selectedGenre;
    float seatSize;
    bool seatsInitialized;

    int searchOffset;
    int passwordOffset;
    int titleOffset;
    int infoOffset;
    int priceOffset;

    double caretTimer;
    bool showError;
    bool searching;
};