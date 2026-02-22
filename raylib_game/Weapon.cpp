#include "weapon.h"
#include "map.h"
#include "raylib.h"
#include <cmath>

#define WEAPON_SCALE  4      // 48x48 * 4 = 192x192 on screen
#define MAX_SHOOT_DIST 20.0f

void weaponInit(Weapon* w)
{
    w->sprite = LoadTexture("assets/textures/pistol.png");
    SetTextureFilter(w->sprite, TEXTURE_FILTER_POINT); // keep it crispy
    w->firing = false;
    w->hasHit = false;
    w->hitPos = { 0, 0 };
}

void weaponShutdown(Weapon* w)
{
    UnloadTexture(w->sprite);
}

static void doHitscan(Weapon* w, const Player* p)
{
    // Cast a ray dead centre of the screen (player's exact facing direction)
    float rayDirX = cosf(p->angle);
    float rayDirY = sinf(p->angle);

    int mapX = (int)p->x;
    int mapY = (int)p->y;

    float deltaDistX = (rayDirX == 0) ? 1e30f : fabsf(1.0f / rayDirX);
    float deltaDistY = (rayDirY == 0) ? 1e30f : fabsf(1.0f / rayDirY);

    float sideDistX, sideDistY;
    int stepX, stepY;

    if (rayDirX < 0) { stepX = -1; sideDistX = (p->x - mapX) * deltaDistX; }
    else { stepX = 1; sideDistX = (mapX + 1.0f - p->x) * deltaDistX; }
    if (rayDirY < 0) { stepY = -1; sideDistY = (p->y - mapY) * deltaDistY; }
    else { stepY = 1; sideDistY = (mapY + 1.0f - p->y) * deltaDistY; }

    int   hit = 0, side = 0;
    float dist = 0.0f;

    while (!hit && dist < MAX_SHOOT_DIST)
    {
        if (sideDistX < sideDistY) { sideDistX += deltaDistX; mapX += stepX; side = 0; }
        else { sideDistY += deltaDistY; mapY += stepY; side = 1; }

        dist = (side == 0) ? (sideDistX - deltaDistX) : (sideDistY - deltaDistY);

        if (mapIsWall(mapX, mapY)) hit = 1;
    }

    if (hit)
    {
        w->hasHit = true;
        w->hitPos = { p->x + rayDirX * dist, p->y + rayDirY * dist };
        TraceLog(LOG_INFO, "Hitscan hit wall at map cell (%d, %d), dist: %.2f", mapX, mapY, dist);
    }
    else
    {
        w->hasHit = false;
    }
}

void weaponUpdate(Weapon* w, const Player* p)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        w->firing = true;
        doHitscan(w, p);
    }
    else
    {
        w->firing = false;
    }
}

void weaponDraw(const Weapon* w, int screenWidth, int screenHeight)
{
    // Sprite is 320x180 with gun in bottom right corner - stretch to fill screen
    Rectangle src = { 0, 0, (float)w->sprite.width, (float)w->sprite.height };
    Rectangle dst = { 0, 0, (float)screenWidth, (float)screenHeight };

    // Recoil kick on fire
    if (w->firing) dst.y += screenHeight / 30;

    DrawTexturePro(w->sprite, src, dst, { 0, 0 }, 0.0f, WHITE);

    // Crosshair
    int cx = screenWidth / 2;
    int cy = screenHeight / 2;
    int cs = 10;
    Color crosshairColor = w->firing ? RED : RAYWHITE;
    DrawLine(cx - cs, cy, cx + cs, cy, crosshairColor);
    DrawLine(cx, cy - cs, cx, cy + cs, crosshairColor);
}