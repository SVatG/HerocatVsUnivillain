#include "game_stats.h"

uint16_t* bulletSpritesA[10];
uint16_t* bulletSpritesB[10];

typedef struct
{
	int32_t x;
	int32_t y;
	int32_t dx;
	int32_t dy;
	int16_t rot;
	int16_t drot;
	int type;
	int alleg; // 0 -> cat, 1 -> evildoer, 2 -> for cat
	int active;
} Bullet;

int nextBullet = 0;

#define MAXBUL 28

Bullet bullets[MAXBUL];

void initBullets() {
	nextBullet = 0;
	
	bulletSpritesA[0] = loadSpriteA16("nitro:/gfx/mallow.img.bin");
	bulletSpritesA[1] = loadSpriteA16("nitro:/gfx/mallow_pink.img.bin");

	bulletSpritesB[0] = loadSpriteB16("nitro:/gfx/mallow.img.bin");
	bulletSpritesB[1] = loadSpriteB16("nitro:/gfx/mallow_pink.img.bin");
	
	for( int i = 0; i < MAXBUL; i++ ) {
		bullets[i].active = 0;
	}
}

void spawnBullet(int x, int y,  int dx,  int dy, int16_t rot, int16_t drot, int alleg, int type ) {
	if( bullets[nextBullet].active == 1 ) {
		for(int i = 0; i < MAXBUL; i++) {
			nextBullet++;
			nextBullet = nextBullet % MAXBUL;
			if(bullets[nextBullet].active == 0) {
				break;
			}
		}
		if(bullets[nextBullet].active == 1) {
			return;
		}
	}
	bullets[nextBullet].active = 1;

	bullets[nextBullet].x = x*256;
	bullets[nextBullet].y = y*256;
	bullets[nextBullet].dx = dx;
	bullets[nextBullet].dy = dy;

	bullets[nextBullet].rot = rot;
	bullets[nextBullet].drot = drot;

	bullets[nextBullet].alleg = alleg;
	bullets[nextBullet].type = type;
}

int updateBullets() {
	for( int i = 0; i < MAXBUL; i++ ) {
		if( bullets[i].active == 1 ) {
			bullets[i].x += bullets[i].dx;
			bullets[i].y += bullets[i].dy;
			bullets[i].rot += bullets[i].drot;
			
			int suby = (bullets[i].y/256);
			int mainy = (bullets[i].y/256)-192;
			
			if(bullets[i].x / 256 > 256 || bullets[i].x / 256 < -32 || mainy > 192 || suby < -32) {
				bullets[i].active = 0;
				continue;
			}

			if(distToCat(bullets[i].x/256,bullets[i].y/256-192) <= 6) {
				if(bullets[i].alleg == 2) {
					catBullets++;
					bullets[i].active = 0;
					bullets[i].x = 257*256;
				}
				if(bullets[i].alleg == 1) {
					catShot = 1;
					bullets[i].active = 0;
					bullets[i].x = 257*256;					
				}
			}
			
			if( suby > 192 ) {
				suby = 193;
			}

			if( mainy < -32 ) {
				mainy = -32;
			}


			oamRotateScale(
				&oamMain,
				i+2,
				bullets[i].rot,
				252,
				252
			);

			oamRotateScale(
				&oamSub,
				i+2,
				bullets[i].rot,
				252,
				252
			);

			oamSet(
				&oamSub, i+2,
				(bullets[i].x>>8),
				suby,
				0, 0,
				SpriteSize_16x16,
				SpriteColorFormat_256Color,
				bulletSpritesB[bullets[i].type],
				i+2,
				true, false, false, false, false
			);

			oamSet(
				&oamMain, i+2,
				(bullets[i].x>>8),
				mainy,
				0, 0,
				SpriteSize_16x16,
				SpriteColorFormat_256Color,
				bulletSpritesA[bullets[i].type],
				i+2,
				true, false, false, false, false
			);
		}
	}

	oamUpdate(&oamMain);
	oamUpdate(&oamSub);
}
