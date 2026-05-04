#pragma once
#include "Movie.h"
#include "../MovieBooking_DAL/FileHandler.h"
#include <vector>
#include <string>
namespace BLL {
    enum ScreenState { LOGIN, CATALOG, BOOKING };
    class BookingManager {
    private:
        ScreenState currentState = LOGIN;
        bool adminMode = false;
        std::vector<Movie> movies;
        int selectedMovieIdx = -1;
    public:
        BookingManager() {
            auto data = DAL::FileHandler::LoadMovies("movies.txt");
            for (const auto& d : data) {
                movies.push_back(Movie(d.id, d.title, d.info, d.price));
            }
            if (movies.empty()) {
                movies.push_back(Movie(1, "INTERSTELLAR", "Space exploration epic", 12.50f));
                movies.push_back(Movie(2, "INCEPTION", "Dream invasion thriller", 10.00f));
            }
        }
        bool IsAdmin() { return adminMode; }
        ScreenState GetCurrentScreen() { return currentState; }
        const std::vector<Movie>& GetMovies() { return movies; }
        Movie GetSelectedMovie() { return movies[selectedMovieIdx]; }
        bool CheckAdminPassword(const char* input) {
            if (std::string(input) == "admin123") {
                adminMode = true;
                currentState = CATALOG;
                return true;
            }
            return false;
        }
        void LoginAsSpectator() {
            adminMode = false;
            currentState = CATALOG;
        }
        void SelectMovie(int index) {
            if (index >= 0 && index < (int)movies.size()) {
                selectedMovieIdx = index;
                currentState = BOOKING;
            }
        }
        void DeleteMovie(int index) {
            if (index >= 0 && index < (int)movies.size()) {
                movies.erase(movies.begin() + index);
                std::vector<DAL::MovieData> dataToSave;
                for (const auto& m : movies) {
                    dataToSave.push_back({ m.id, m.title, m.info, m.price });
                }
                DAL::FileHandler::SaveMovies("movies.txt", dataToSave);
            }
        }
        void FinalizeBooking(const std::vector<int>& seatingData) {
            if (selectedMovieIdx != -1) {
                std::string filename = movies[selectedMovieIdx].title + "_seats.txt";
                DAL::FileHandler::SaveSeatingPlan(filename, seatingData);
            }
        }
        void Logout() {
            currentState = LOGIN;
            adminMode = false;
            selectedMovieIdx = -1;
        }
    };
}