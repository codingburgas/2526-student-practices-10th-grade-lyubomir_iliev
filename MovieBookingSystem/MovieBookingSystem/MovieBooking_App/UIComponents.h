#pragma once
#include "raylib.h"

namespace UI {
    static bool Button(Rectangle rect, const char* text, Color color, int fontSize = 20) {
        Vector2 mouse = GetMousePosition();
        bool hover = CheckCollisionPointRec(mouse, rect);
        DrawRectangleRec(rect, hover ? ColorAlpha(color, 0.8f) : color);
        DrawRectangleLinesEx(rect, 2, DARKGRAY);
        int tw = MeasureText(text, fontSize);
        DrawText(text, rect.x + (rect.width - tw) / 2, rect.y + (rect.height - fontSize) / 2, fontSize, WHITE);
        return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    }
}