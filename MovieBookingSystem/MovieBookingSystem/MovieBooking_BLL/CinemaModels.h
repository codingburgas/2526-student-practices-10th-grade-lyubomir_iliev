#pragma once
#include <string>
#include <vector>

namespace BLL {

    enum SeatType { SEAT_SILVER, SEAT_GOLD, SEAT_PLATINUM };

    struct Seat {
        int number;
        SeatType type;
        bool isBooked;
        double price;
    };

    struct Show {
        int id;
        int movieId;
        std::string time;
        int hallId;
        std::vector<Seat> seats;
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
        std::string address;
        std::vector<Hall> halls;
    };

}