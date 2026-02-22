#include "weapon.h"
#include "enemy.h"
#include "map.h"
#include "raylib.h"
#include <cmath>

#define WEAPON_SCALE   4
#define MAX_SHOOT_DIST 20.0f

void weaponInit(Weapon* w)
{
    w->sprite = LoadTexture("assets/textures/pistol.png");
    SetTextureFilter(w->sprite, TEXTURE_FILTER_POINT);
    w->firing = false;
    w->hasHit = false;
    w->hitPos = { 0, 0 };
    w->killCount = 0;
}

void weaponShutdown(Weapon* w)
{
    UnloadTexture(w->sprite);
}

static void doHitscan(Weapon* w, const Player* p, EnemyManager* em)
{
    float rayDirX = cosf(p->angle);
    float rayDirY = sinf(p->angle);

    // Check each active enemy for ray intersection first
    float closestEnemyDist = 1e30f;
    int   hitEnemyIdx = -1;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!em->enemies[i].active) continue;

        float dx = em->enemies[i].x - p->x;
        float dy = em->enemies[i].y - p->y;

        // Project enemy onto ray to find closest point
        float t = dx * rayDirX + dy * rayDirY;
        if (t < 0) continue; // enemy is behind player

        // Closest point on ray to enemy centre
        float closestX = p->x + rayDirX * t;
        float closestY = p->y + rayDirY * t;

        float distToRay = sqrtf(
            (closestX - em->enemies[i].x) * (closestX - em->enemies[i].x) +
            (closestY - em->enemies[i].y) * (closestY - em->enemies[i].y)
        );

        if (distToRay < 0.4f && t < closestEnemyDist)
        {
            closestEnemyDist = t;
            hitEnemyIdx = i;
        }
    }

    // Now find wall distance using DDA
    int   mapX = (int)p->x;
    int   mapY = (int)p->y;
    float deltaDistX = (rayDirX == 0) ? 1e30f : fabsf(1.0f / rayDirX);
    float deltaDistY = (rayDirY == 0) ? 1e30f : fabsf(1.0f / rayDirY);
    float sideDistX, sideDistY;
    int   stepX, stepY;

    if (rayDirX < 0) { stepX = -1; sideDistX = (p->x - mapX) * deltaDistX; }
    else { stepX = 1; sideDistX = (mapX + 1.0f - p->x) * deltaDistX; }
    if (rayDirY < 0) { stepY = -1; sideDistY = (p->y - mapY) * deltaDistY; }
    else { stepY = 1; sideDistY = (mapY + 1.0f - p->y) * deltaDistY; }

    int   hit = 0, side = 0;
    float wallDist = 0.0f;

    while (!hit && wallDist < MAX_SHOOT_DIST)
    {
        if (sideDistX < sideDistY) { sideDistX += deltaDistX; mapX += stepX; side = 0; }
        else { sideDistY += deltaDistY; mapY += stepY; side = 1; }
        wallDist = (side == 0) ? (sideDistX - deltaDistX) : (sideDistY - deltaDistY);
        if (mapIsWall(mapX, mapY)) hit = 1;
    }

    // Enemy hit only counts if it's closer than the wall
    if (hitEnemyIdx != -1 && closestEnemyDist < wallDist)
    {
        em->enemies[hitEnemyIdx].active = false;
        em->count--;
        w->killCount++;
        w->hasHit = true;
        TraceLog(LOG_INFO, "Enemy %d hit at dist %.2f, remaining: %d", hitEnemyIdx, closestEnemyDist, em->count);
        return;
    }

    // Wall hit
    w->hasHit = hit;
    if (hit) w->hitPos = { p->x + rayDirX * wallDist, p->y + rayDirY * wallDist };
}

void weaponUpdate(Weapon* w, const Player* p, EnemyManager* em)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        w->firing = true;
        doHitscan(w, p, em);
    }
    else
    {
        w->firing = false;
    }
}

void weaponDraw(const Weapon* w, int screenWidth, int screenHeight)
{
    Rectangle src = { 0, 0, (float)w->sprite.width, (float)w->sprite.height };
    Rectangle dst = { 0, 0, (float)screenWidth, (float)screenHeight };

    if (w->firing) dst.y += screenHeight / 30;

    DrawTexturePro(w->sprite, src, dst, { 0, 0 }, 0.0f, WHITE);

    // Crosshair
    int cx = screenWidth / 2;
    int cy = screenHeight / 2;
    int cs = 10;
    Color crosshairColor = w->firing ? RED : RAYWHITE;
    DrawLine(cx - cs, cy, cx + cs, cy, crosshairColor);
    DrawLine(cx, cy - cs, cx, cy + cs, crosshairColor);

    // Kill counter top right
    const char* killText = TextFormat("KILLS: %d", w->killCount);
    int fontSize = 20;
    int textWidth = MeasureText(killText, fontSize);
    DrawText(killText, screenWidth - textWidth - 16, 8, fontSize, YELLOW);
}