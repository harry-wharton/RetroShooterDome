#ifndef ENEMY_H
#define ENEMY_H

#include "raylib.h"

typedef struct {
    float x;
    float y;
    bool  active;
} Enemy;

#define MAX_ENEMIES 16

typedef struct {
    Enemy     enemies[MAX_ENEMIES];
    Texture2D sprite;
    int       count;
    int       wave;
} EnemyManager;

void  enemyManagerInit(EnemyManager* em);
void  enemyManagerShutdown(EnemyManager* em);
void  enemyManagerUpdate(EnemyManager* em);
void  enemyManagerDraw(EnemyManager* em, float playerX, float playerY, float playerAngle,
    float* zBuffer, int zBufferLen,
    int screenWidth, int screenHeight);
void  enemySpawn(EnemyManager* em, float x, float y);
bool  enemyManagerAllDead(EnemyManager* em);
bool  enemyTryHit(EnemyManager* em, float hitX, float hitY, float playerX, float playerY);

#endif