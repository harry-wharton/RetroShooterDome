#include "game.h"
#include "player.h"
#include "renderer.h"
#include "weapon.h"
#include "enemy.h"
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

    EnemyManager enemies;
    enemyManagerInit(&enemies);
    enemyManagerUpdate(&enemies);

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
                weaponUpdate(&weapon, &player, &enemies);
                enemyManagerUpdate(&enemies);
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
            enemyManagerDraw(&enemies, player.x, player.y, player.angle,
                renderer.zBuffer, RENDER_WIDTH,
                SCREEN_WIDTH, SCREEN_HEIGHT);
            weaponDraw(&weapon, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
        else if (state == GameState::PAUSED)
        {
            rendererDraw(&renderer, &player, SCREEN_WIDTH, SCREEN_HEIGHT);
            enemyManagerDraw(&enemies, player.x, player.y, player.angle,
                renderer.zBuffer, RENDER_WIDTH,
                SCREEN_WIDTH, SCREEN_HEIGHT);
            weaponDraw(&weapon, SCREEN_WIDTH, SCREEN_HEIGHT);
            uiDrawPauseMenu(state, SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        EndDrawing();
    }

    enemyManagerShutdown(&enemies);
    weaponShutdown(&weapon);
    rendererShutdown(&renderer);
    CloseWindow();
}