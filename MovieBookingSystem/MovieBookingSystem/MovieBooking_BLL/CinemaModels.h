#pragma once
#include <string>
#include <vector>

namespace BLL {

    struct Show {
        int id;
        int movieId;
        std::string time;
        int hallId;
    };

    struct Hall {
        int id;
        std::string name;
        int rows;
        int cols;
        std::vector<Show> shows;
    };

    struct Cinema {
        int id;
        std::string name;
        std::vector<Hall> halls;
    };

}
