#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include "Movie.h"
#include "CinemaModels.h"

namespace BLL {

    enum ScreenState { LOGIN, CATALOG, BOOKING, ADD_MOVIE };

    struct ShowTime {
        int id;
        int movieId;
        std::string time;
        std::string day;
        std::string hallName;
        std::string subtitle;
        bool is3D;
    };

    class BookingManager {
    private:
        ScreenState screen;
        bool adminMode;
        std::vector<Movie> movies;
        std::vector<ShowTime> showtimes;
        int selectedMovieIdx;
        std::string db_name;
        std::string show_db_name;

    public:
        int selectedDayIndex;

        BookingManager() {
            screen = LOGIN;
            adminMode = false;
            selectedMovieIdx = -1;
            selectedDayIndex = 0;
            db_name = "movies_db.txt";
            show_db_name = "showtimes_db.txt";

            LoadMoviesFromFile();
            LoadShowtimesFromFile();

            if (movies.empty()) {
                AddDefaultMovies();
                SaveMoviesToFile();
            }
            if (showtimes.empty()) {
                AddDefaultShowtimes();
                SaveShowtimesToFile();
            }
        }

        void AddDefaultMovies() {
            movies.push_back(Movie(1, "SUPERGIRL", "Action, Adventure, Fantasy | 108 min.", 12.50, "Action", "English", "2022"));
            movies.push_back(Movie(2, "OBSESSION", "Horror, Romance | 108 min.", 15.00, "Horror", "English", "2024"));
            movies.push_back(Movie(3, "TOY STORY 5", "Adventure, Animation, Comedy, Family | 102 min.", 10.00, "Animation", "Bulgarian", "2023"));
        }

        void AddDefaultShowtimes() {
            showtimes.clear();
            showtimes.push_back({ 1, 1, "17:40", "TODAY", "Hall 1", "English (BUL subs)", true });
            showtimes.push_back({ 2, 1, "20:00", "TODAY", "Hall 1", "English (BUL subs)", true });
            showtimes.push_back({ 3, 1, "22:20", "TODAY", "Hall 2", "English (BUL subs)", true });
            showtimes.push_back({ 4, 2, "17:20", "TODAY", "Hall 2", "English (BUL subs)", false });
            showtimes.push_back({ 5, 2, "19:40", "TODAY", "Hall 2", "English (BUL subs)", false });
            showtimes.push_back({ 6, 2, "22:00", "TODAY", "Hall 1", "English (BUL subs)", false });
            showtimes.push_back({ 7, 3, "17:15", "TODAY", "Hall 3", "Bulgarian Dub", false });
            showtimes.push_back({ 8, 3, "19:30", "TODAY", "Hall 3", "Bulgarian Dub", false });
            showtimes.push_back({ 9, 3, "18:30", "TODAY", "Hall 3", "Bulgarian Dub", true });
            showtimes.push_back({ 10, 1, "14:00", "TOMORROW", "Hall 1", "English (BUL subs)", true });
            showtimes.push_back({ 11, 1, "17:00", "TOMORROW", "Hall 1", "English (BUL subs)", true });
            showtimes.push_back({ 12, 1, "20:00", "TOMORROW", "Hall 1", "English (BUL subs)", true });
            showtimes.push_back({ 13, 2, "15:00", "TOMORROW", "Hall 2", "English (BUL subs)", false });
            showtimes.push_back({ 14, 2, "18:00", "TOMORROW", "Hall 2", "English (BUL subs)", false });
            showtimes.push_back({ 15, 2, "21:00", "TOMORROW", "Hall 2", "English (BUL subs)", false });
            showtimes.push_back({ 16, 3, "14:30", "TOMORROW", "Hall 3", "Bulgarian Dub", false });
            showtimes.push_back({ 17, 3, "17:00", "TOMORROW", "Hall 3", "Bulgarian Dub", false });
            showtimes.push_back({ 18, 3, "19:30", "TOMORROW", "Hall 3", "Bulgarian Dub", false });
            showtimes.push_back({ 19, 1, "15:30", "SUN", "Hall 1", "English (BUL subs)", true });
            showtimes.push_back({ 20, 1, "18:30", "SUN", "Hall 1", "English (BUL subs)", true });
            showtimes.push_back({ 21, 1, "21:00", "SUN", "Hall 2", "English (BUL subs)", true });
            showtimes.push_back({ 22, 2, "16:00", "SUN", "Hall 2", "English (BUL subs)", false });
            showtimes.push_back({ 23, 2, "18:30", "SUN", "Hall 2", "English (BUL subs)", false });
            showtimes.push_back({ 24, 2, "20:45", "SUN", "Hall 1", "English (BUL subs)", false });
            showtimes.push_back({ 25, 3, "14:00", "SUN", "Hall 3", "Bulgarian Dub", false });
            showtimes.push_back({ 26, 3, "16:30", "SUN", "Hall 3", "Bulgarian Dub", false });
            showtimes.push_back({ 27, 3, "19:00", "SUN", "Hall 3", "Bulgarian Dub", false });
        }

        void LoadMoviesFromFile() {
            std::ifstream file(db_name);
            if (!file.is_open()) return;
            movies.clear();
            std::string line;
            while (std::getline(file, line)) {
                if (line.empty()) continue;
                size_t p1 = line.find('|');
                size_t p2 = line.find('|', p1 + 1);
                size_t p3 = line.find('|', p2 + 1);
                if (p1 == std::string::npos || p2 == std::string::npos) continue;
                std::string t = line.substr(0, p1);
                std::string i = line.substr(p1 + 1, p2 - p1 - 1);
                float p = std::stof(line.substr(p2 + 1, p3 - p2 - 1));
                movies.push_back(Movie((int)movies.size() + 1, t, i, p));
            }
            file.close();
        }

        void SaveMoviesToFile() {
            std::ofstream file(db_name, std::ios::trunc);
            if (file.is_open()) {
                for (const auto& m : movies) {
                    file << m.title << "|" << m.info << "|" << m.price << "\n";
                }
                file.close();
            }
        }

        void LoadShowtimesFromFile() {
            std::ifstream file(show_db_name);
            if (!file.is_open()) return;
            showtimes.clear();
            std::string line;
            while (std::getline(file, line)) {
                if (line.empty()) continue;
                size_t p1 = line.find('|');
                size_t p2 = line.find('|', p1 + 1);
                size_t p3 = line.find('|', p2 + 1);
                size_t p4 = line.find('|', p3 + 1);
                size_t p5 = line.find('|', p4 + 1);
                size_t p6 = line.find('|', p5 + 1);
                if (p1 == std::string::npos) continue;
                ShowTime st;
                st.id = std::stoi(line.substr(0, p1));
                st.movieId = std::stoi(line.substr(p1 + 1, p2 - p1 - 1));
                st.time = line.substr(p2 + 1, p3 - p2 - 1);
                st.day = line.substr(p3 + 1, p4 - p3 - 1);
                st.hallName = line.substr(p4 + 1, p5 - p4 - 1);
                st.subtitle = line.substr(p5 + 1, p6 - p5 - 1);
                st.is3D = (line.substr(p6 + 1) == "1");
                showtimes.push_back(st);
            }
            file.close();
        }

        void SaveShowtimesToFile() {
            std::ofstream file(show_db_name, std::ios::trunc);
            if (file.is_open()) {
                for (const auto& st : showtimes) {
                    file << st.id << "|" << st.movieId << "|" << st.time << "|"
                        << st.day << "|" << st.hallName << "|" << st.subtitle << "|"
                        << (st.is3D ? "1" : "0") << "\n";
                }
                file.close();
            }
        }

        void LoginAsSpectator() { screen = CATALOG; adminMode = false; }
        void Logout() { screen = LOGIN; adminMode = false; selectedMovieIdx = -1; }
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
        Movie& GetSelectedMovie() {
            if (selectedMovieIdx >= 0 && selectedMovieIdx < (int)movies.size()) {
                return movies[selectedMovieIdx];
            }
            static Movie empty;
            return empty;
        }
        void SelectMovie(int idx) { selectedMovieIdx = idx; screen = BOOKING; }

        void AddMovie(std::string t, std::string i, float p) {
            movies.push_back(Movie((int)movies.size() + 1, t, i, p));
            SaveMoviesToFile();
        }

        void DeleteMovie(int index) {
            if (index >= 0 && index < (int)movies.size()) {
                movies.erase(movies.begin() + index);
                SaveMoviesToFile();
            }
        }

        void BookSeats(std::vector<int> seatNumbers) {
            if (selectedMovieIdx < 0) return;

            std::string filename = movies[selectedMovieIdx].title + "_seats.txt";
            std::replace(filename.begin(), filename.end(), ' ', '_');

            std::vector<int> existingSeats;
            std::ifstream readFile(filename);
            if (readFile.is_open()) {
                int num;
                while (readFile >> num) {
                    existingSeats.push_back(num);
                }
                readFile.close();
            }

            for (int num : seatNumbers) {
                bool alreadyBooked = false;
                for (int existing : existingSeats) {
                    if (existing == num) { alreadyBooked = true; break; }
                }
                if (!alreadyBooked) {
                    existingSeats.push_back(num);
                }
            }

            std::ofstream file(filename, std::ios::trunc);
            if (file.is_open()) {
                for (int num : existingSeats) {
                    file << num << " ";
                }
                file.close();
            }

            screen = CATALOG;
        }

        std::vector<ShowTime> GetShowtimesForDay(int dayIndex) {
            std::vector<ShowTime> result;
            std::string dayNames[] = { "TODAY", "TOMORROW", "SUN", "MON", "TUE", "WED", "THU" };
            if (dayIndex >= 0 && dayIndex < 7) {
                for (int i = 0; i < (int)showtimes.size(); i++) {
                    if (showtimes[i].day == dayNames[dayIndex]) {
                        result.push_back(showtimes[i]);
                    }
                }
            }
            return result;
        }

        std::vector<Movie> SearchMovies(const std::string& query) {
            std::vector<Movie> result;
            for (int i = 0; i < (int)movies.size(); i++) {
                if (query.empty()) {
                    result.push_back(movies[i]);
                }
                else if (movies[i].title.find(query) != std::string::npos) {
                    result.push_back(movies[i]);
                }
            }
            return result;
        }
    };

}