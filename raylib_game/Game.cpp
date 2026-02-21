#include "game.h"
#include "player.h"
#include "renderer.h"
#include "ui.h"
#include "gamestate.h"
#include "raylib.h"

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

void gameRun()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Retro Shooter");
    SetTargetFPS(60);

    Player player;
    playerInit(&player);

    Renderer renderer;
    rendererInit(&renderer);

    GameState state = GameState::START_MENU;

    // Start with cursor visible for the menu
    EnableCursor();

    while (!WindowShouldClose())
    {
        // --- Update ---
        if (state == GameState::PLAYING)
        {
            // Escape to pause
            if (IsKeyPressed(KEY_P))
            {
                state = GameState::PAUSED;
                EnableCursor();
            }
            else
            {
                playerUpdate(&player);
            }
        }

        // --- Draw ---
        BeginDrawing();

        if (state == GameState::START_MENU)
        {
            ClearBackground(BLACK);
            uiDrawStartMenu(state, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
        else if (state == GameState::PLAYING)
        {
            rendererDraw(&renderer, &player, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
        else if (state == GameState::PAUSED)
        {
            // Draw the game world behind the pause overlay
            rendererDraw(&renderer, &player, SCREEN_WIDTH, SCREEN_HEIGHT);
            uiDrawPauseMenu(state, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
        EndDrawing();
    }

    rendererShutdown(&renderer);
    CloseWindow();
}