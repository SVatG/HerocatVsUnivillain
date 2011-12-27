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

void resetGame();
int catOnLava();
void printOSD();
void scoreAdd(int howMuch);
int floorLeftHeight();
int floorCenterHeight();
int floorRightHeight();

#endif