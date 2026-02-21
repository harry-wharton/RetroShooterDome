#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    float x;
    float y;
    float angle;
} Player;

void playerInit(Player* p);
void playerUpdate(Player* p);

#endif