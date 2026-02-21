#include "player.h"
#include "map.h"
#include "raylib.h"
#include "math.h"

#define MOVE_SPEED 0.05f
#define ROT_SPEED  0.03f

void playerInit(Player* p)
{
    p->x = 2.0f;
    p->y = 2.0f;
    p->angle = 0.0f;
}

void playerUpdate(Player* p)
{
    if (IsKeyDown(KEY_LEFT))  p->angle -= ROT_SPEED;
    if (IsKeyDown(KEY_RIGHT)) p->angle += ROT_SPEED;

    float newX = p->x + cosf(p->angle) * MOVE_SPEED * IsKeyDown(KEY_UP);
    float newY = p->y + sinf(p->angle) * MOVE_SPEED * IsKeyDown(KEY_UP);
    if (!mapIsWall((int)newX, (int)newY)) { p->x = newX; p->y = newY; }

    float backX = p->x - cosf(p->angle) * MOVE_SPEED * IsKeyDown(KEY_DOWN);
    float backY = p->y - sinf(p->angle) * MOVE_SPEED * IsKeyDown(KEY_DOWN);
    if (!mapIsWall((int)backX, (int)backY)) { p->x = backX; p->y = backY; }
}