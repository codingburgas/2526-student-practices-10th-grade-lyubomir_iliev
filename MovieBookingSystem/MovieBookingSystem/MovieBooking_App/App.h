#pragma once
#include "raylib.h"
#include <string>
#include <vector>

namespace BLL {
    enum ScreenState { LOGIN, CATALOG, BOOKING, ADD_MOVIE };

    struct Movie {
        std::string title;
        std::string info;
        float price;
    };

    class BookingManager {
    public:
        void LoginAsSpectator();
        void Logout();
        bool CheckAdminPassword(const char* pass);
        ScreenState GetCurrentScreen();
        void SetScreen(ScreenState state);
        bool IsAdmin();
        std::vector<Movie>& GetMovies();
        void AddMovie(std::string t, std::string i, float p);
        void DeleteMovie(int index);
        void SelectMovie(int index);
        Movie& GetSelectedMovie();
        void FinalizeBooking(std::vector<int> states);

    private:
        ScreenState screen = LOGIN;
        bool adminMode = false;
        std::vector<Movie> movies;
        int selectedIdx = 0;
    };
}

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