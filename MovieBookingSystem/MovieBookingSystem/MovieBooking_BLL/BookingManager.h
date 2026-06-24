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
        ScreenState screen;
        bool adminMode;

        std::vector<Movie> movies;
        std::vector<Cinema> cinemas;

        int selectedMovieIdx;

        std::string db_name;

    public:
        BookingManager() {
            screen = LOGIN;
            adminMode = false;
            selectedMovieIdx = -1;
            db_name = "movies_db.txt";

            LoadMoviesFromFile();
            AddTestCinemas();
        }

        void AddTestCinemas() {
            Cinema c1;
            c1.id = 1;
            c1.name = "Cinema City";
            c1.address = "Sofia";

            Hall h1;
            h1.id = 1;
            h1.name = "Hall 1";
            h1.rows = 8;
            h1.cols = 10;

            Show s1;
            s1.id = 1;
            s1.movieId = 1;
            s1.time = "18:00";
            s1.hallId = 1;

            for (int i = 1; i <= 80; i++) {
                Seat seat;
                seat.number = i;
                seat.isBooked = false;

                if (i <= 20) {
                    seat.type = SEAT_PLATINUM;
                    seat.price = 20.0;
                }
                else if (i <= 40) {
                    seat.type = SEAT_GOLD;
                    seat.price = 15.0;
                }
                else {
                    seat.type = SEAT_SILVER;
                    seat.price = 10.0;
                }
                s1.seats.push_back(seat);
            }

            h1.shows.push_back(s1);
            c1.halls.push_back(h1);
            cinemas.push_back(c1);
        }

        void LoadMoviesFromFile() {
            std::ifstream file(db_name);
            if (!file.is_open()) {
                movies.push_back(Movie(1, "The Batman", "Action | 2h 56m", 12.50, "Action", "English", "2022"));
                movies.push_back(Movie(2, "Dune: Part Two", "Sci-Fi | 2h 46m", 15.00, "Sci-Fi", "English", "2024"));
                return;
            }

            movies.clear();
            std::string line;
            while (std::getline(file, line)) {
                if (line.empty()) continue;

                size_t p1 = line.find('|');
                size_t p2 = line.find('|', p1 + 1);
                size_t p3 = line.find('|', p2 + 1);
                size_t p4 = line.find('|', p3 + 1);
                size_t p5 = line.find('|', p4 + 1);

                if (p1 == std::string::npos || p2 == std::string::npos) continue;

                std::string t = line.substr(0, p1);
                std::string i = line.substr(p1 + 1, p2 - p1 - 1);
                float p = std::stof(line.substr(p2 + 1, p3 - p2 - 1));
                std::string g = line.substr(p3 + 1, p4 - p3 - 1);
                std::string lang = line.substr(p4 + 1, p5 - p4 - 1);
                std::string r = line.substr(p5 + 1);

                movies.push_back(Movie((int)movies.size() + 1, t, i, p, g, lang, r));
            }
            file.close();
        }

        void SaveMoviesToFile() {
            std::ofstream file(db_name, std::ios::trunc);
            if (file.is_open()) {
                for (const auto& m : movies) {
                    file << m.title << "|" << m.info << "|" << m.price << "|"
                        << m.genre << "|" << m.language << "|" << m.releaseDate << "\n";
                }
                file.close();
            }
        }

        void LoginAsSpectator() {
            screen = CATALOG;
            adminMode = false;
        }

        void Logout() {
            screen = LOGIN;
            adminMode = false;
            selectedMovieIdx = -1;
        }

        bool CheckAdminPassword(const char* pass) {
            if (std::string(pass) == "admin123") {
                adminMode = true;
                screen = CATALOG;
                return true;
            }
            return false;
        }

        bool IsAdmin() { return adminMode; }
        ScreenState GetCurrentScreen() { return screen; }
        void SetScreen(ScreenState s) { screen = s; }

        std::vector<Movie>& GetMovies() { return movies; }
        std::vector<Cinema>& GetCinemas() { return cinemas; }

        Movie& GetSelectedMovie() {
            if (selectedMovieIdx >= 0 && selectedMovieIdx < (int)movies.size()) {
                return movies[selectedMovieIdx];
            }
            static Movie empty;
            return empty;
        }

        void SelectMovie(int idx) {
            selectedMovieIdx = idx;
            screen = BOOKING;
        }

        void AddMovie(std::string t, std::string i, float p,
            std::string g = "", std::string lang = "", std::string r = "") {
            movies.push_back(Movie((int)movies.size() + 1, t, i, p, g, lang, r));
            SaveMoviesToFile();
        }

        void DeleteMovie(int index) {
            if (index >= 0 && index < (int)movies.size()) {
                movies.erase(movies.begin() + index);
                SaveMoviesToFile();
            }
        }

        std::vector<Movie> SearchMovies(const std::string& query) {
            std::vector<Movie> result;
            for (auto& m : movies) {
                if (query.empty()) {
                    result.push_back(m);
                }
                else if (m.title.find(query) != std::string::npos ||
                    m.genre.find(query) != std::string::npos ||
                    m.language.find(query) != std::string::npos ||
                    m.releaseDate.find(query) != std::string::npos) {
                    result.push_back(m);
                }
            }
            return result;
        }

        void BookSeats(std::vector<int> seatNumbers) {
            if (selectedMovieIdx < 0) return;

            std::string filename = movies[selectedMovieIdx].title + "_seats.txt";
            std::replace(filename.begin(), filename.end(), ' ', '_');

            std::ofstream file(filename);
            if (file.is_open()) {
                for (int num : seatNumbers) {
                    file << num << " ";
                }
                file.close();
            }

            screen = CATALOG;
        }
    };

}