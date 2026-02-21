#include "player.h"
#include "map.h"
#include "raylib.h"
#include <cmath>

#define MOVE_SPEED    0.05f
#define STRAFE_SPEED  0.04f
#define MOUSE_SENS    0.002f

void playerInit(Player* p)
{
    p->x = 2.0f;
    p->y = 2.0f;
    p->angle = 0.0f;

    DisableCursor();
}

void playerUpdate(Player* p)
{
    // Mouse look
    float mouseDelta = GetMouseDelta().x;
    p->angle += mouseDelta * MOUSE_SENS;

    float dirX = cosf(p->angle);
    float dirY = sinf(p->angle);

    // Strafe direction (perpendicular to facing)
    float strafeX = cosf(p->angle + PI / 2.0f);
    float strafeY = sinf(p->angle + PI / 2.0f);

    float newX = p->x;
    float newY = p->y;

    // Forward / back
    if (IsKeyDown(KEY_W)) { newX += dirX * MOVE_SPEED; newY += dirY * MOVE_SPEED; }
    if (IsKeyDown(KEY_S)) { newX -= dirX * MOVE_SPEED; newY -= dirY * MOVE_SPEED; }

    // Strafe
    if (IsKeyDown(KEY_D)) { newX += strafeX * STRAFE_SPEED; newY += strafeY * STRAFE_SPEED; }
    if (IsKeyDown(KEY_A)) { newX -= strafeX * STRAFE_SPEED; newY -= strafeY * STRAFE_SPEED; }

    // Collision - check X and Y independently for wall sliding
    if (!mapIsWall((int)newX, (int)p->y)) p->x = newX;
    if (!mapIsWall((int)p->x, (int)newY)) p->y = newY;
}