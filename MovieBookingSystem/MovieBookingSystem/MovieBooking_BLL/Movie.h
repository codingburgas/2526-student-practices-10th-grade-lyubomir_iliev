#pragma once
#include <string>
namespace BLL {
    class Movie {
    public:
        int id;
        std::string title;
        std::string info;
        float price;
        Movie(int id, std::string t, std::string i, float p)
            : id(id), title(t), info(i), price(p) {
        }
    };
}