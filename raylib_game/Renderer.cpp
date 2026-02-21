#include "renderer.h"
#include "map.h"
#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <cstring>

#define FOV 1.0472f

// ------------------------------------------------------------------ helpers

static inline Color* pixelAt(Color* pixels, int x, int y)
{
    return &pixels[y * RENDER_WIDTH + x];
}

static inline Color imagePixel(const Image& img, int x, int y)
{
    x = x % img.width;
    y = y % img.height;
    return ((Color*)img.data)[y * img.width + x];
}

static inline Color darken(Color c, float factor)
{
    return Color{
        (unsigned char)(c.r * factor),
        (unsigned char)(c.g * factor),
        (unsigned char)(c.b * factor),
        255
    };
}

// ------------------------------------------------------------------ init / shutdown

void rendererInit(Renderer* r)
{
    // Load images
    r->textures.wallImg = LoadImage("assets/textures/wall.png");
    r->textures.ceilImg = LoadImage("assets/textures/ceiling.png");
    r->textures.floorImg = LoadImage("assets/textures/floor.png");

    ImageFormat(&r->textures.wallImg, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    ImageFormat(&r->textures.ceilImg, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    ImageFormat(&r->textures.floorImg, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    // Framebuffer — we draw into this CPU-side then upload once per frame
    r->framebuffer = GenImageColor(RENDER_WIDTH, RENDER_HEIGHT, BLACK);
    ImageFormat(&r->framebuffer, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    r->pixels = (Color*)r->framebuffer.data;
    r->frameTexture = LoadTextureFromImage(r->framebuffer);

    // FPS display
    r->fpsTimer = 0.0f;
    r->fpsDisplay = 0;
}

void rendererShutdown(Renderer* r)
{
    UnloadImage(r->textures.wallImg);
    UnloadImage(r->textures.ceilImg);
    UnloadImage(r->textures.floorImg);
    UnloadImage(r->framebuffer);
    UnloadTexture(r->frameTexture);
}

// ------------------------------------------------------------------ draw

void rendererDraw(Renderer* r, const Player* p, int screenWidth, int screenHeight)
{
    Color* pixels = r->pixels;
    int W = RENDER_WIDTH;
    int H = RENDER_HEIGHT;
    int halfH = H / 2;

    // --- Floor and ceiling casting ---
    float rayDirX0 = cosf(p->angle - FOV / 2.0f);
    float rayDirY0 = sinf(p->angle - FOV / 2.0f);
    float rayDirX1 = cosf(p->angle + FOV / 2.0f);
    float rayDirY1 = sinf(p->angle + FOV / 2.0f);

    float posZ = 0.5f * H; // camera height in screen space

    for (int y = halfH + 1; y < H; y++)
    {
        float rowDist = posZ / (y - halfH);

        float floorStepX = rowDist * (rayDirX1 - rayDirX0) / W;
        float floorStepY = rowDist * (rayDirY1 - rayDirY0) / W;

        float floorX = p->x + rowDist * rayDirX0;
        float floorY = p->y + rowDist * rayDirY0;

        float shade = Clamp(1.0f - rowDist * 0.08f, 0.2f, 1.0f);

        for (int x = 0; x < W; x++)
        {
            int tx = (int)(r->textures.floorImg.width * (floorX - floorf(floorX))) & (r->textures.floorImg.width - 1);
            int ty = (int)(r->textures.floorImg.height * (floorY - floorf(floorY))) & (r->textures.floorImg.height - 1);

            floorX += floorStepX;
            floorY += floorStepY;

            float ceilShade = Clamp(1.0f - rowDist * 0.08f, 0.2f, 1.0f);

            // Floor
            *pixelAt(pixels, x, y) = darken(imagePixel(r->textures.floorImg, tx, ty), shade);
            // Ceiling (mirror)
            *pixelAt(pixels, x, H - y - 1) = darken(imagePixel(r->textures.ceilImg, tx, ty), ceilShade);
        }
    }

    // --- Raycasted walls ---
    for (int col = 0; col < W; col++)
    {
        float rayAngle = (p->angle - FOV / 2.0f) + ((float)col / W) * FOV;
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

        int lineHeight = (int)(H / perpDist);
        int drawStart = halfH - lineHeight / 2;
        int drawEnd = halfH + lineHeight / 2;

        // Texture column
        float wallX;
        if (side == 0) wallX = p->y + perpDist * rayDirY;
        else           wallX = p->x + perpDist * rayDirX;
        wallX -= floorf(wallX);

        int texCol = (int)(wallX * r->textures.wallImg.width);
        if ((side == 0 && rayDirX > 0) || (side == 1 && rayDirY < 0))
            texCol = r->textures.wallImg.width - texCol - 1;

        float distShade = Clamp(1.0f - perpDist * 0.1f, 0.1f, 1.0f);
        float sideShade = (side == 1) ? 0.5f : 1.0f;
        float shade = distShade * sideShade;

        for (int row = drawStart; row < drawEnd; row++)
        {
            if (row < 0 || row >= H) continue;

            int texRow = (int)(((float)(row - drawStart) / lineHeight) * r->textures.wallImg.height);
            texRow = texRow % r->textures.wallImg.height;

            Color c = imagePixel(r->textures.wallImg, texCol, texRow);
            *pixelAt(pixels, col, row) = darken(c, shade);
        }
    }

    // --- Upload framebuffer to GPU and scale to screen ---
    UpdateTexture(r->frameTexture, pixels);

    Rectangle src = { 0, 0, (float)W, (float)H };
    Rectangle dst = { 0, 0, (float)screenWidth, (float)screenHeight };
    Vector2   orig = { 0, 0 };
    DrawTexturePro(r->frameTexture, src, dst, orig, 0.0f, WHITE);

    // --- FPS counter (updates every 0.5s) ---
    r->fpsTimer += GetFrameTime();
    if (r->fpsTimer >= 0.5f)
    {
        r->fpsDisplay = GetFPS();
        r->fpsTimer = 0.0f;
    }
    DrawText(TextFormat("FPS: %d", r->fpsDisplay), 8, 8, 20, YELLOW);
}