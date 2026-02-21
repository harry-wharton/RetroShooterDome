#include "ui.h"
#include "raylib.h"

// Simple helper — returns true if button was clicked
static bool drawButton(const char* label, int x, int y, int w, int h, Color idle, Color hover)
{
    Rectangle rect = { (float)x, (float)y, (float)w, (float)h };
    bool hovered = CheckCollisionPointRec(GetMousePosition(), rect);
    DrawRectangleRec(rect, hovered ? hover : idle);
    DrawRectangleLinesEx(rect, 2, RAYWHITE);

    int fontSize = 20;
    int textWidth = MeasureText(label, fontSize);
    DrawText(label, x + (w - textWidth) / 2, y + (h - fontSize) / 2, fontSize, RAYWHITE);

    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void drawOverlay(int screenWidth, int screenHeight, unsigned char alpha)
{
    DrawRectangle(0, 0, screenWidth, screenHeight, Color{ 0, 0, 0, alpha });
}

static void drawTitle(const char* title, int screenWidth, int y, int fontSize)
{
    int w = MeasureText(title, fontSize);
    DrawText(title, (screenWidth - w) / 2, y, fontSize, RAYWHITE);
}

// ------------------------------------------------------------------ menus

void uiDrawStartMenu(GameState& state, int screenWidth, int screenHeight)
{
    drawOverlay(screenWidth, screenHeight, 200);

    drawTitle("RETRO SHOOTER", screenWidth, screenHeight / 4, 60);

    int btnW = 200;
    int btnH = 50;
    int btnX = (screenWidth - btnW) / 2;

    if (drawButton("PLAY", btnX, screenHeight / 2, btnW, btnH, DARKGRAY, GRAY))
    {
        state = GameState::PLAYING;
        DisableCursor();
    }

    if (drawButton("QUIT", btnX, screenHeight / 2 + 70, btnW, btnH, DARKGRAY, GRAY))
    {
        // Signal quit by setting a flag via CloseWindow request
        // We handle this in game.cpp by checking WindowShouldClose
        // so we just call this directly
        CloseWindow();
    }
}

void uiDrawPauseMenu(GameState& state, int screenWidth, int screenHeight)
{
    drawOverlay(screenWidth, screenHeight, 150);

    drawTitle("PAUSED", screenWidth, screenHeight / 4, 60);

    int btnW = 200;
    int btnH = 50;
    int btnX = (screenWidth - btnW) / 2;

    if (drawButton("RESUME", btnX, screenHeight / 2, btnW, btnH, DARKGRAY, GRAY))
    {
        state = GameState::PLAYING;
        DisableCursor();
    }

    if (drawButton("MAIN MENU", btnX, screenHeight / 2 + 70, btnW, btnH, DARKGRAY, GRAY))
    {
        state = GameState::START_MENU;
        EnableCursor();
    }
}