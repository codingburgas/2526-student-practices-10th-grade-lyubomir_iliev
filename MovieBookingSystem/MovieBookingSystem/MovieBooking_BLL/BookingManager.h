#pragma once
#include "../MovieBooking_DAL/FileHandler.h"
#include <vector>
namespace BLL {
    class BookingManager {
    public:
        void FinalizeBooking(const std::vector<int>& seatingData) {
            DAL::FileHandler::SaveSeatingPlan("room1.txt", seatingData);
        }
        std::vector<int> GetRoomState() {
            return DAL::FileHandler::LoadSeatingPlan("room1.txt");
        }
    };
}