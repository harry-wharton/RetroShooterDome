#pragma once
#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "gamestate.h"

void uiDrawStartMenu(GameState& state, int screenWidth, int screenHeight);
void uiDrawPauseMenu(GameState& state, int screenWidth, int screenHeight);

#endif