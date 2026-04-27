#pragma once
#include "raylib.h"
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
public:
	App(int width, int height, const char* title);
	~App();
	void Run();
};