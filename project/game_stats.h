#ifndef _GAME_STATS_H
#define _GAME_STATS_H

#include <nds.h>
#include <stdio.h>

#include <nds/ndstypes.h>
#include <nds/interrupts.h>
#include <string.h>

#include "kitten.h"

#define RESX 40
#define RESY 40

extern int score;
extern int highscore;
extern Kitten Cat;
extern int noise[RESX+1][RESY+1];
extern int yp;
extern int catShot;
extern int catBullets;

void resetGame();
int catOnLava();
void printOSD();
void scoreAdd(int howMuch);
int floorLeftHeight();
int floorCenterHeight();
int floorRightHeight();

int nitroLoad(char *path, uint16_t* buffer, uint32_t size);
uint16_t* loadSpriteA32( char* path );
uint16_t* loadSpriteA16( char* path );
uint16_t* loadSpriteB16( char* path );

int distToCat(int x, int y);

#endif