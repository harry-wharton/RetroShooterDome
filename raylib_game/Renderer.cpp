#include "renderer.h"
#include "map.h"
#include "raylib.h"
#include "math.h"

#define FOV 1.0472f  // 60 degrees in radians

void rendererDraw(const Player* p, int screenWidth, int screenHeight)
{
    // Ceiling and floor
    DrawRectangle(0, 0, screenWidth, screenHeight / 2, DARKGRAY);
    DrawRectangle(0, screenHeight / 2, screenWidth, screenHeight / 2, Color{ 50, 50, 50, 255 });

    // Raycasting
    for (int col = 0; col < screenWidth; col++)
    {
        float rayAngle = (p->angle - FOV / 2.0f) + ((float)col / screenWidth) * FOV;
        float rayDirX = cosf(rayAngle);
        float rayDirY = sinf(rayAngle);

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

        int hit = 0, side = 0;
        while (!hit)
        {
            if (sideDistX < sideDistY) { sideDistX += deltaDistX; mapX += stepX; side = 0; }
            else { sideDistY += deltaDistY; mapY += stepY; side = 1; }
            if (mapIsWall(mapX, mapY)) hit = 1;
        }

        float perpDist = (side == 0) ? (sideDistX - deltaDistX) : (sideDistY - deltaDistY);

        int lineHeight = (int)(screenHeight / perpDist);
        int drawStart = screenHeight / 2 - lineHeight / 2;
        int drawEnd = screenHeight / 2 + lineHeight / 2;

        int brightness = (int)(255.0f / (1.0f + perpDist * 0.3f));
        if (side == 1) brightness /= 2;
        Color wallColor = { brightness, brightness, brightness, 255 };

        DrawLine(col, drawStart, col, drawEnd, wallColor);
    }
}