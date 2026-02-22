#pragma once
#ifndef WEAPON_H
#define WEAPON_H

#include "raylib.h"
#include "player.h"

typedef struct {
    Texture2D sprite;
    bool       firing;
    Vector2    hitPos;
    bool       hasHit;
} Weapon;

void weaponInit(Weapon* w);
void weaponShutdown(Weapon* w);
void weaponUpdate(Weapon* w, const Player* p);
void weaponDraw(const Weapon* w, int screenWidth, int screenHeight);

#endif