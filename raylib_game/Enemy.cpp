#include "enemy.h"
#include "raylib.h"
#include "raymath.h"
#include "map.h"
#include <cmath>

#define FOV      1.0472f
#define SPRITE_W 150
#define SPRITE_H 80

// Wave spawn positions — add more sets as needed
// Safe spawn positions per wave - all guaranteed open map cells
static const float WAVE_SPAWNS[][4][2] = {
    { {4.5f, 4.5f}, {8.5f, 3.5f}, {6.5f, 2.5f}, {10.5f, 5.5f} },  // wave 0
    { {6.5f, 8.5f}, {3.5f, 7.5f}, {9.5f, 7.5f}, {5.5f, 10.5f} },  // wave 1
    { {5.5f, 5.5f}, {9.5f, 9.5f}, {2.5f, 9.5f}, {7.5f,  6.5f} },  // wave 2
};
static const int WAVE_COUNT = 3;
static const int ENEMIES_PER_WAVE = 4;

void enemyManagerInit(EnemyManager* em)
{
    em->sprite = LoadTexture("assets/textures/enemy.png");
    SetTextureFilter(em->sprite, TEXTURE_FILTER_POINT);
    em->count = 0;
    em->wave = 0;

    for (int i = 0; i < MAX_ENEMIES; i++)
        em->enemies[i].active = false;
}

void enemyManagerShutdown(EnemyManager* em)
{
    UnloadTexture(em->sprite);
}

void enemySpawn(EnemyManager* em, float x, float y)
{
    // Check centre and 4 cardinal points around spawn
    if (mapIsWall((int)x, (int)y) ||
        mapIsWall((int)(x + 0.4f), (int)y) ||
        mapIsWall((int)(x - 0.4f), (int)y) ||
        mapIsWall((int)x, (int)(y + 0.4f)) ||
        mapIsWall((int)x, (int)(y - 0.4f)))
    {
        TraceLog(LOG_WARNING, "Skipping spawn at (%.1f, %.1f) - too close to wall", x, y);
        return;
    }

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!em->enemies[i].active)
        {
            em->enemies[i].x = x;
            em->enemies[i].y = y;
            em->enemies[i].active = true;
            em->count++;
            return;
        }
    }
}

bool enemyManagerAllDead(EnemyManager* em)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (em->enemies[i].active) return false;

    TraceLog(LOG_INFO, "All enemies dead, triggering next wave");
    return true;
}

// Try to hit the nearest enemy close to the hitscan hit point
bool enemyTryHit(EnemyManager* em, float hitX, float hitY, float playerX, float playerY)
{
    float bestDist = 0.4f;
    int   bestIdx = -1;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!em->enemies[i].active) continue;

        float dx = em->enemies[i].x - hitX;
        float dy = em->enemies[i].y - hitY;
        float d = sqrtf(dx * dx + dy * dy);

        if (d < bestDist)
        {
            bestDist = d;
            bestIdx = i;
        }
    }

    if (bestIdx != -1)
    {
        em->enemies[bestIdx].active = false;
        em->count--;
        TraceLog(LOG_INFO, "Enemy killed, remaining: %d", em->count);
        return true;
    }
    return false;
}

void enemyManagerUpdate(EnemyManager* em)
{
    if (!enemyManagerAllDead(em)) return;

    int waveIdx = em->wave % WAVE_COUNT;
    TraceLog(LOG_INFO, "Spawning wave %d (waveIdx %d)", em->wave, waveIdx);

    for (int i = 0; i < ENEMIES_PER_WAVE; i++)
    {
        float x = WAVE_SPAWNS[waveIdx][i][0];
        float y = WAVE_SPAWNS[waveIdx][i][1];
        TraceLog(LOG_INFO, "Spawning at (%.1f, %.1f) isWall: %d", x, y, mapIsWall((int)x, (int)y));
        enemySpawn(em, x, y);
    }

    em->wave++;
}

static void sortEnemies(Enemy* enemies, int count, float px, float py)
{
    for (int i = 1; i < count; i++)
    {
        Enemy key = enemies[i];
        float keyDist = (key.x - px) * (key.x - px) + (key.y - py) * (key.y - py);
        int j = i - 1;
        while (j >= 0)
        {
            float jDist = (enemies[j].x - px) * (enemies[j].x - px) +
                (enemies[j].y - py) * (enemies[j].y - py);
            if (jDist < keyDist) { enemies[j + 1] = enemies[j]; j--; }
            else break;
        }
        enemies[j + 1] = key;
    }
}

void enemyManagerDraw(EnemyManager* em, float playerX, float playerY, float playerAngle,
    float* zBuffer, int zBufferLen,
    int screenWidth, int screenHeight)
{
    Enemy sorted[MAX_ENEMIES];
    int activeCount = 0;
    for (int i = 0; i < MAX_ENEMIES; i++)
        if (em->enemies[i].active)
            sorted[activeCount++] = em->enemies[i];

    sortEnemies(sorted, activeCount, playerX, playerY);

    for (int i = 0; i < activeCount; i++)
    {
        Enemy* e = &sorted[i];

        float dx = e->x - playerX;
        float dy = e->y - playerY;

        float dist = sqrtf(dx * dx + dy * dy);
        if (dist < 0.1f) continue;

        float angleToEnemy = atan2f(dy, dx);
        float relAngle = angleToEnemy - playerAngle;

        while (relAngle > PI) relAngle -= 2.0f * PI;
        while (relAngle < -PI) relAngle += 2.0f * PI;

        if (fabsf(relAngle) > FOV * 0.75f) continue;

        float camX = tanf(relAngle) / tanf(FOV / 2.0f);
        int   screenX = (int)((camX * 0.5f + 0.5f) * screenWidth);

        int spriteHeight = (int)(screenHeight / dist);
        int spriteWidth = (int)(spriteHeight * ((float)SPRITE_W / SPRITE_H));

        int drawX = screenX - spriteWidth / 2;
        int drawY = screenHeight / 2 - spriteHeight / 2;

        float        brightness = Clamp(1.0f - dist * 0.1f, 0.1f, 1.0f);
        unsigned char b = (unsigned char)(255 * brightness);
        Color         tint = { b, b, b, 255 };

        for (int col = drawX; col < drawX + spriteWidth; col++)
        {
            if (col < 0 || col >= screenWidth) continue;

            int zCol = (int)((float)col / screenWidth * zBufferLen);
            zCol = Clamp(zCol, 0, zBufferLen - 1);

            if (zBuffer[zCol] < dist) continue;

            float spriteColNorm = (float)(col - drawX) / spriteWidth;
            int   texCol = (int)(spriteColNorm * SPRITE_W);

            Rectangle src = { (float)texCol, 0, 1, (float)SPRITE_H };
            Rectangle dst = { (float)col, (float)drawY, 1, (float)spriteHeight };

            DrawTexturePro(em->sprite, src, dst, { 0, 0 }, 0.0f, tint);
        }
    }

    // DEBUG - show all active enemy positions as text
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!em->enemies[i].active) continue;
        DrawText(TextFormat("Enemy %d: (%.1f, %.1f)", i, em->enemies[i].x, em->enemies[i].y),
            8, 100 + i * 20, 16, RED);
    }
}