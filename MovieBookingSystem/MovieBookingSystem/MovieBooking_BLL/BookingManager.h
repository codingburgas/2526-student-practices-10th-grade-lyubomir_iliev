#pragma once
#include "Movie.h"
#include "../MovieBooking_DAL/FileHandler.h"
#include <vector>
#include <string>

namespace BLL {
    enum ScreenState { LOGIN, CATALOG, BOOKING, ADD_MOVIE };

    class BookingManager {
    private:
        ScreenState currentState = LOGIN;
        bool adminMode = false;
        std::vector<Movie> movies;
        int selectedMovieIdx = -1;

    public:
        BookingManager() {
            auto data = DAL::FileHandler::LoadMovies("movies.txt");
            for (const auto& d : data) movies.push_back(Movie(d.id, d.title, d.info, d.price));
        }

        bool IsAdmin() const { return adminMode; }
        ScreenState GetCurrentScreen() const { return currentState; }
        void SetScreen(ScreenState state) { currentState = state; } 
        const std::vector<Movie>& GetMovies() const { return movies; }

        Movie GetSelectedMovie() const {
            if (selectedMovieIdx >= 0 && selectedMovieIdx < (int)movies.size()) return movies[selectedMovieIdx];
            return Movie(0, "", "", 0.0f);
        }

        bool CheckAdminPassword(const char* input) {
            if (std::string(input) == "admin123") {
                adminMode = true;
                currentState = CATALOG;
                return true;
            }
            return false;
        }

        void AddMovie(std::string title, std::string info, float price) {
            int newId = movies.empty() ? 1 : movies.back().id + 1;
            movies.push_back(Movie(newId, title, info, price));

            std::vector<DAL::MovieData> data;
            for (const auto& m : movies) data.push_back({ m.id, m.title, m.info, m.price });
            DAL::FileHandler::SaveMovies("movies.txt", data);
        }

        void LoginAsSpectator() { adminMode = false; currentState = CATALOG; }
        void Logout() { currentState = LOGIN; adminMode = false; selectedMovieIdx = -1; }
        void SelectMovie(int index) { selectedMovieIdx = index; currentState = BOOKING; }

        void FinalizeBooking(const std::vector<int>& seatingData) {
            if (selectedMovieIdx != -1) {
                std::string filename = movies[selectedMovieIdx].title + "_seats.txt";
                DAL::FileHandler::SaveSeatingPlan(filename, seatingData);
            }
        }

        void DeleteMovie(int index) {
            if (index >= 0 && index < (int)movies.size()) {
                movies.erase(movies.begin() + index);
                std::vector<DAL::MovieData> data;
                for (const auto& m : movies) data.push_back({ m.id, m.title, m.info, m.price });
                DAL::FileHandler::SaveMovies("movies.txt", data);
            }
        }
    };
}