#pragma once
#include <fstream>
#include <vector>
#include <string>
namespace DAL {
    class FileHandler {
    public:
        static void SaveSeatingPlan(const std::string& filename, const std::vector<int>& data) {
            std::ofstream file(filename);
            if (file.is_open()) {
                for (int status : data) {
                    file << status << " ";
                }
                file.close();
            }
        }
        static std::vector<int> LoadSeatingPlan(const std::string& filename) {
            std::vector<int> data;
            std::ifstream file(filename);
            int status;
            while (file >> status) {
                data.push_back(status);
            }
            return data;
        }
    };
}