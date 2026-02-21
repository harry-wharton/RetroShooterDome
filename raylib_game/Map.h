#pragma once
#ifndef MAP_H
#define MAP_H

#define MAP_WIDTH  16
#define MAP_HEIGHT 16

extern int map[MAP_HEIGHT][MAP_WIDTH];

int mapIsWall(int x, int y);

#endif