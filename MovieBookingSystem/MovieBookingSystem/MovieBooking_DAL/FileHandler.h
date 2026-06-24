#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

namespace DAL {

    struct SeatData {
        int showId;
        int seatNumber;
        bool isBooked;
        std::string seatType;
    };

    class FileHandler {
    public:
        static std::vector<std::string> ReadFile(const std::string& filename) {
            std::vector<std::string> lines;
            std::ifstream file(filename);
            if (file.is_open()) {
                std::string line;
                while (std::getline(file, line)) {
                    if (!line.empty()) lines.push_back(line);
                }
                file.close();
            }
            return lines;
        }

        static void WriteFile(const std::string& filename, const std::vector<std::string>& data) {
            std::ofstream file(filename, std::ios::trunc);
            if (file.is_open()) {
                for (const auto& line : data) {
                    file << line << "\n";
                }
                file.close();
            }
        }

        static void SaveSeats(const std::string& filename, const std::vector<SeatData>& seats) {
            std::ofstream file(filename, std::ios::trunc);
            if (file.is_open()) {
                for (const auto& seat : seats) {
                    file << seat.showId << "|"
                        << seat.seatNumber << "|"
                        << (seat.isBooked ? "1" : "0") << "|"
                        << seat.seatType << "\n";
                }
                file.close();
            }
        }

        static std::vector<SeatData> LoadSeats(const std::string& filename) {
            std::vector<SeatData> seats;
            auto lines = ReadFile(filename);
            for (const auto& line : lines) {
                std::stringstream ss(line);
                std::string item;
                SeatData seat;
                try {
                    if (std::getline(ss, item, '|')) seat.showId = std::stoi(item);
                    if (std::getline(ss, item, '|')) seat.seatNumber = std::stoi(item);
                    if (std::getline(ss, item, '|')) seat.isBooked = (item == "1");
                    if (std::getline(ss, item, '|')) seat.seatType = item;
                    seats.push_back(seat);
                }
                catch (...) { continue; }
            }
            return seats;
        }
    };

}