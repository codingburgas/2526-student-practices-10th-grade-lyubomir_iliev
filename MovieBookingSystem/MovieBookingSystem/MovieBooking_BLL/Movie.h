#pragma once
#include <string>

namespace BLL {

    class Movie {
    public:
        int id;
        std::string title;
        std::string info;
        float price;
        std::string genre;
        std::string language;
        std::string releaseDate;

        Movie() : id(0), price(0.0f) {}

        Movie(int id, std::string t, std::string i, float p,
            std::string g = "", std::string lang = "", std::string r = "")
            : id(id), title(t), info(i), price(p),
            genre(g), language(lang), releaseDate(r) {}
    };

}