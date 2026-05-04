#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

namespace DAL {
    struct MovieData {
        int id;
        std::string title;
        std::string info;
        float price;
    };

    class FileHandler {
    public:
        static std::vector<MovieData> LoadMovies(const std::string& filename) {
            std::vector<MovieData> movies;
            std::ifstream file(filename);
            std::string line;
            if (file.is_open()) {
                while (std::getline(file, line)) {
                    std::stringstream ss(line);
                    std::string item;
                    MovieData m;
                    try {
                        if (std::getline(ss, item, '|')) m.id = std::stoi(item);
                        if (std::getline(ss, item, '|')) m.title = item;
                        if (std::getline(ss, item, '|')) m.info = item;
                        if (std::getline(ss, item, '|')) m.price = std::stof(item);
                        movies.push_back(m);
                    }
                    catch (...) { continue; }
                }
                file.close();
            }
            return movies;
        }

        static void SaveMovies(const std::string& filename, const std::vector<MovieData>& movies) {
            std::ofstream file(filename, std::ios::trunc);
            if (file.is_open()) {
                for (const auto& m : movies) {
                    file << m.id << "|" << m.title << "|" << m.info << "|" << m.price << "\n";
                }
                file.close();
            }
        }

        static void SaveSeatingPlan(const std::string& filename, const std::vector<int>& seatingData) {
            std::ofstream file(filename, std::ios::trunc);
            if (file.is_open()) {
                for (int i = 0; i < (int)seatingData.size(); i++) {
                    file << seatingData[i] << " ";
                    if ((i + 1) % 10 == 0) file << "\n";
                }
                file.close();
            }
        }
    };
}