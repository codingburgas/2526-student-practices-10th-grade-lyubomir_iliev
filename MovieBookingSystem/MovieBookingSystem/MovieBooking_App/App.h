#pragma once
#include "raylib.h"
#include <string>
#include <vector>
#include "BookingManager.h"   

struct Seat {
    Rectangle rect;
    bool isReserved;
    bool isSelected;
};

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
    void LoadSeatingPlan(std::string movieTitle, std::string time);
    void ResetSeats();

    BLL::BookingManager bookingMgr;   

    static const int ROWS = 8;
    static const int COLS = 10;
    Seat seats[ROWS][COLS];

    char passwordInput[32] = { 0 };
    int letterCount = 0;
    bool showError = false;

    char movieTitleInput[64] = { 0 };
    char movieInfoInput[256] = { 0 };
    char moviePriceInput[32] = { 0 };
    int activeField = 0;
    int infoVisibleIdx;
};
