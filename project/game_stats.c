#include "game_stats.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <fcntl.h>
#include <unistd.h>

#include "nitrofs.h"
#include "bullets.h"

int nitroLoad(char *path, uint16_t* buffer, uint32_t size) {
  int fd_reuse = open(path, O_RDONLY);
  read(fd_reuse, buffer, size);
  close(fd_reuse);
}

// Load an 32x32 sprite into A/B OBJ RAM, return pointer.
uint16_t* loadSpriteA32( char* path ) {
	uint16_t* newSprite = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
	nitroLoad( path, newSprite, 32*32 );
	return( newSprite );
}

// Load an 16x16 sprite into A OBJ RAM, return pointer.
uint16_t* loadSpriteA16( char* path ) {
	uint16_t* newSprite = oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);
	nitroLoad( path, newSprite, 16*16 );
	return( newSprite );
}

// Load an 16x16 sprite into B OBJ RAM, return pointer.
uint16_t* loadSpriteB16( char* path ) {
	uint16_t* newSprite = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);
	nitroLoad( path, newSprite, 16*16 );
	return( newSprite );
}

// Load an 64x64 sprite into B OBJ RAM, return pointer.
uint16_t* loadSpriteB64( char* path ) {
	uint16_t* newSprite = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_256Color);
	nitroLoad( path, newSprite, 64*64 );
	return( newSprite );
}


int noise[RESX+1][RESY+1];
int yp = 0;
Kitten Cat;
int highscore;
int score;
int catShot;
int catBullets;

void initGame() {
	yp = 0;
	catShot = 0;
	catBullets = 20;
	KittenInit(&Cat);
	initStuffSprites();
	lowerscreen_init();
}

void resetGame() {
	yp = 0;
	catShot = 0;
	catBullets = 20;
	KittenReset(&Cat);
	resetBullets();
	resetUnicorns();
	resetEvilUnicorns();
	lowerscreen_reset();
}

int floorLeftHeight() {
	return( noise[(Cat.x-3)/6+4][(yp+13)%40] );
}

int floorCenterHeight() {
	return( noise[(Cat.x)/6+4][(yp+13)%40] );
}

int floorRightHeight() {
	return( noise[(Cat.x+3)/6+4][(yp+13)%40] );
}

int catOnLava() {
	return(
		floorLeftHeight() <= -2 &&
		floorCenterHeight() <= -2 &&
		floorRightHeight() <= -2
	);
}

int distToCat(int x, int y) {
	return sqrt((x-(Cat.x+16))*(x-(Cat.x+16))+(y-(Cat.y+16))*(y-(Cat.y+16)));
}

int catShotLast = 0;
int catShoot(int t) {
	if(t - catShotLast > 30) {
		if(catBullets > 0) {
			catBullets--;
			catShotLast = t;
			spawnBullet(Cat.x+16,Cat.y+192+16,0,-3000,0,-700,0,1);
		}
	}
}

void unicornLeft() {
	spawnUnicorn(256,-64,-1200,600);
}

void unicornRight() {
	spawnUnicorn(-64,-64,1200,600);
}

void evilLeft() {
	spawnEvilUnicorn(-64,10,200+(((int32_t)Random())%800),300+(((int32_t)Random())%100)-(((int32_t)Random())%100),20);
}

void evilRight() {
	spawnEvilUnicorn(256,10,-(200+(((int32_t)Random())%800)),300+(((int32_t)Random())%100)-(((int32_t)Random())%100),20);
}

void printOSD() {
	gotoxy(1,1);
	iprintf("Score: %d\n", score);
	gotoxy(1,2);	
	iprintf("Bullets: %d\n", catBullets);
	if(catOnLava()) {
		gotoxy(1,2);
		iprintf("Cat status: Dead\n");
	}
}

void scoreAdd(int howMuch) {
	score += howMuch;
}

