#pragma once
#include "raylib.h"
#include "../MovieBooking_BLL/BookingManager.h"
#include <vector>
struct Seat {
	Rectangle rect;
	bool isReserved;
	bool isSelected;
};
class App {
private:
	static const int ROWS = 8;
	static const int COLS = 10;
	Seat seats[ROWS][COLS];
	BLL::BookingManager bookingMgr;
	char passwordInput[32] = "\0";
	int letterCount = 0;
	bool showError = false;
	int infoVisibleIdx = -1;
public:
	App(int width, int height, const char* title);
	~App();
	void Run();
	void RenderLogin();
	void RenderCatalog();
	void RenderHall();
	void HandleInput();
};