#ifndef RENDERER_H
#define RENDERER_H

#include "player.h"
#include "raylib.h"

#define RENDER_WIDTH  320
#define RENDER_HEIGHT 200

typedef struct {
    Image wallImg;
    Image ceilImg;
    Image floorImg;
} RendererTextures;

typedef struct {
    RendererTextures textures;
    Image            framebuffer;
    Texture2D        frameTexture;
    Color* pixels;
    float            fpsTimer;
    int              fpsDisplay;
} Renderer;

void rendererInit(Renderer* r);
void rendererShutdown(Renderer* r);
void rendererDraw(Renderer* r, const Player* p, int screenWidth, int screenHeight);

#endif