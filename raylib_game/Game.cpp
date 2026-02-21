#include "game.h"
#include "player.h"
#include "renderer.h"
#include "raylib.h"

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

void gameRun()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Retro Shooter");
    SetTargetFPS(60);

    Player player;
    playerInit(&player);

    while (!WindowShouldClose())
    {
        playerUpdate(&player);

        BeginDrawing();
        rendererDraw(&player, SCREEN_WIDTH, SCREEN_HEIGHT);
        EndDrawing();
    }

    CloseWindow();
}