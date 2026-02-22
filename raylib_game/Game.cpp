#include "game.h"
#include "player.h"
#include "renderer.h"
#include "weapon.h"
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

    Weapon weapon;
    weaponInit(&weapon);

    GameState state = GameState::START_MENU;
    EnableCursor();

    while (!WindowShouldClose())
    {
        if (state == GameState::PLAYING)
        {
            if (IsKeyPressed(KEY_ESCAPE))
            {
                state = GameState::PAUSED;
                EnableCursor();
            }
            else
            {
                playerUpdate(&player);
                weaponUpdate(&weapon, &player);
            }
        }

        BeginDrawing();

        if (state == GameState::START_MENU)
        {
            ClearBackground(BLACK);
            uiDrawStartMenu(state, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
        else if (state == GameState::PLAYING)
        {
            rendererDraw(&renderer, &player, SCREEN_WIDTH, SCREEN_HEIGHT);
            weaponDraw(&weapon, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
        else if (state == GameState::PAUSED)
        {
            rendererDraw(&renderer, &player, SCREEN_WIDTH, SCREEN_HEIGHT);
            weaponDraw(&weapon, SCREEN_WIDTH, SCREEN_HEIGHT);
            uiDrawPauseMenu(state, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
        EndDrawing();
    }

    weaponShutdown(&weapon);
    rendererShutdown(&renderer);
    CloseWindow();
}