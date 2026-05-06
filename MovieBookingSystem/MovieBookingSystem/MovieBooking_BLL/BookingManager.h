#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

#include "Movie.h"
#include "CinemaModels.h"

namespace BLL {

    enum ScreenState { LOGIN, CATALOG, BOOKING, ADD_MOVIE };

    class BookingManager {
    private:
        ScreenState screen = LOGIN;
        bool adminMode = false;

        std::vector<Movie> movies;
        int selectedIdx = -1;

        std::vector<Cinema> cinemas;
        int selectedCinema = -1;
        int selectedHall = -1;
        int selectedShow = -1;

        const std::string db_name = "movies_db.txt";

    public:
        BookingManager() {
            LoadMoviesFromFile();
        }

        inline void SaveMoviesToFile() {
            std::ofstream file(db_name, std::ios::trunc);
            if (file.is_open()) {
                for (const auto& m : movies) {
                    file << m.title << "|" << m.info << "|" << m.price << "\n";
                }
                file.close();
            }
        }

        inline void LoadMoviesFromFile() {
            std::ifstream file(db_name);
            if (!file.is_open()) {
                AddMovie("The Batman", "Action | 2h 56m", 12.50f);
                AddMovie("Dune: Part Two", "Sci-Fi | 2h 46m", 15.00f);
                return;
            }
            movies.clear();
            std::string line;
            while (std::getline(file, line)) {
                if (line.empty()) continue;
                size_t p1 = line.find('|');
                size_t p2 = line.rfind('|');
                if (p1 != std::string::npos && p2 != std::string::npos && p1 != p2) {
                    std::string t = line.substr(0, p1);
                    std::string i = line.substr(p1 + 1, p2 - p1 - 1);
                    float p = std::stof(line.substr(p2 + 1));
                    movies.push_back({ (int)movies.size() + 1, t, i, p });
                }
            }
            file.close();
        }

        inline void LoginAsSpectator() { screen = CATALOG; adminMode = false; }
        inline void Logout() { screen = LOGIN; adminMode = false; selectedIdx = -1; }

        inline bool IsAdmin() { return adminMode; }
        inline ScreenState GetCurrentScreen() { return screen; }
        inline void SetScreen(ScreenState s) { screen = s; }

        inline std::vector<Movie>& GetMovies() { return movies; }

        inline Movie& GetSelectedMovie() {
            if (selectedIdx >= 0 && selectedIdx < (int)movies.size()) return movies[selectedIdx];
            static Movie empty(0, "", "", 0.0f);
            return empty;
        }

        inline void SelectMovie(int idx) { selectedIdx = idx; screen = BOOKING; }

        inline bool CheckAdminPassword(const char* pass) {
            if (std::string(pass) == "admin123") {
                adminMode = true;
                screen = CATALOG;
                return true;
            }
            return false;
        }

        inline void AddMovie(std::string t, std::string i, float p) {
            movies.push_back({ (int)movies.size() + 1, t, i, p });
            SaveMoviesToFile();
        }

        inline void DeleteMovie(int index) {
            if (index >= 0 && index < (int)movies.size()) {
                movies.erase(movies.begin() + index);
                SaveMoviesToFile();
            }
        }

        inline void FinalizeBooking(std::vector<int> states) {
            std::string filename = GetSelectedMovie().title + "_seats.txt";
            std::replace(filename.begin(), filename.end(), ' ', '_');
            std::ofstream file(filename);
            if (file.is_open()) {
                for (int s : states) file << s << " ";
                file.close();
            }
            screen = CATALOG;
        }

        inline void SelectCinema(int id) { selectedCinema = id; }
        inline void SelectHall(int id) { selectedHall = id; }
        inline void SelectShow(int id) { selectedShow = id; screen = BOOKING; }

        inline std::vector<Show>& GetShows(int cinemaId, int hallId) {
            return cinemas[cinemaId].halls[hallId].shows;
        }
    };
}
