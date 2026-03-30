#pragma once
#include "raylib.h"

class App {
public:
    App(int width, int height, const char* title);
    ~App();
    void Run();
};