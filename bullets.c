#include "game_stats.h"
#include <maxmod9.h>
#include "music.h"

uint16_t* bulletSpritesA[10];
uint16_t* bulletSpritesB[10];
uint16_t* unicornSprite;
uint16_t* evilUnicornSprite;
uint16_t* deadUnicornSprite;
uint16_t* bossUnicornL;
uint16_t* bossUnicornR;

extern int gameFinished;

typedef struct
{
	int32_t x;
	int32_t y;
	int32_t dx;
	int32_t dy;
	int spc;
	int active;
} NiceUnicorn;

typedef struct
{
	int32_t x;
	int32_t y;
	int32_t dx;
	int32_t dy;
	int spc;
	int sf;
	int active;
	int lrc;
	int lrmax;
	int dead;
	int life; 
} EvilUnicorn;

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
int nextUnicorn = 0;
int nextEvilUnicorn = 0;

#define MAXBUL 28

Bullet bullets[MAXBUL];
NiceUnicorn niceUnicorns[5];
EvilUnicorn evilUnicorns[4];

void initStuffSprites() {
	bulletSpritesA[0] = loadSpriteA16("nitro:/gfx/mallow.img.bin");
	bulletSpritesA[1] = loadSpriteA16("nitro:/gfx/mallow_pink.img.bin");
	bulletSpritesA[2] = loadSpriteA16("nitro:/gfx/unibeam.img.bin");

	bulletSpritesB[0] = loadSpriteB16("nitro:/gfx/mallow.img.bin");
	bulletSpritesB[1] = loadSpriteB16("nitro:/gfx/mallow_pink.img.bin");
	bulletSpritesB[2] = loadSpriteB16("nitro:/gfx/unibeam.img.bin");

	unicornSprite = loadSpriteB64("nitro:/gfx/unicorn_marsh.img.bin");

	evilUnicornSprite = loadSpriteB64("nitro:/gfx/unicorn_front.img.bin");
	deadUnicornSprite = loadSpriteB64("nitro:/gfx/unicorn_blacked.img.bin");

	bossUnicornL = loadSpriteB64("nitro:/gfx/unicorn_boss_l.img.bin");
	bossUnicornR = loadSpriteB64("nitro:/gfx/unicorn_boss_r.img.bin");
}

void resetBullets() {
	nextBullet = 0;
	
	for( int i = 0; i < MAXBUL; i++ ) {
		bullets[i].active = 0;
		bullets[i].x = 257;
	}
}

void resetUnicorns() {
	nextUnicorn = 0;
	
	for( int i = 0; i < 5; i++ ) {
		niceUnicorns[i].active = 0;
		niceUnicorns[i].x = 257;
		niceUnicorns[i].spc = 0;
	}
}

void resetEvilUnicorns() {
	nextEvilUnicorn = 0;
	
	for( int i = 0; i < 4; i++ ) {
		evilUnicorns[i].active = 0;
		evilUnicorns[i].x = 257;
		evilUnicorns[i].spc = 0;
		evilUnicorns[i].lrc = 0;
		evilUnicorns[i].dead = 0;
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

			int dist = distToCat(bullets[i].x/256,bullets[i].y/256-192);
			if( dist <= 9  && bullets[i].alleg == 2) {
				catBullets++;
				mmEffect(SFX_POWERUP);
				bullets[i].active = 0;
				bullets[i].x = 257*256;
				scoreAdd(10);
			}
			if( dist <= 6  && bullets[i].alleg == 1) {
				catShot = 1;
				bullets[i].active = 0;
				bullets[i].x = 257*256;
			}

			if(bullets[i].alleg == 0) {
				for( int u = 0; u < 3; u++ ) {
					if( evilUnicorns[u].active == 1 && evilUnicorns[u].dead == 0 ) {
						int unix = abs(((evilUnicorns[u].x/256)+32) - ((bullets[i].x/256)+8));
						int uniy = abs(((evilUnicorns[u].y/256)+32) - ((bullets[i].y/256)+8));
						if(
							(unix <= 5 && uniy <= 25) || (unix <= 30 && uniy <= 8)
						) {
							scoreAdd(200);
							bullets[i].active = 0;
							bullets[i].x = 257*256;
							evilUnicorns[u].dead = 1;
							evilUnicorns[u].dy = -3000;
							evilUnicorns[u].dx = 0;
							mmEffect(SFX_SLAUGHTER1);
						}
					}
					if( evilUnicorns[3].active == 1  ) {
						int unix = abs((((evilUnicorns[3].x/256)+32)) - ((bullets[i].x/256)+8));
						int uniy = abs((((evilUnicorns[3].y/256)-32)) - ((bullets[i].y/256)+8));
						if(
							(unix <= 5 && uniy <= 25) || (unix <= 64 && uniy <= 9)
						) {
							evilUnicorns[3].life--;
							if(evilUnicorns[3].life == 0) {
								gameFinished = 1;
								evilUnicorns[3].active = 0;
							}
							bullets[i].active = 0;
							bullets[i].x = 257*256;
							mmEffect(SFX_SLAUGHTER2);
							scoreAdd(300);
						}
						
					}
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

void spawnUnicorn(int x, int y,  int dx,  int dy) {
	if( niceUnicorns[nextUnicorn].active == 1 ) {
		for(int i = 0; i < 5; i++) {
			nextUnicorn++;
			nextUnicorn = nextUnicorn % 5;
			if(niceUnicorns[nextUnicorn].active == 0) {
				break;
			}
		}
		if(niceUnicorns[nextUnicorn].active == 1) {
			return;
		}
	}
	niceUnicorns[nextUnicorn].active = 1;

	niceUnicorns[nextUnicorn].x = x*256;
	niceUnicorns[nextUnicorn].y = y*256;
	niceUnicorns[nextUnicorn].dx = dx;
	niceUnicorns[nextUnicorn].dy = dy;
}

void spawnEvilUnicorn(int x, int y,  int dx,  int dy, int sf) {
	if( evilUnicorns[nextEvilUnicorn].active == 1 ) {
		for(int i = 0; i < 3; i++) {
			nextEvilUnicorn++;
			nextEvilUnicorn = nextEvilUnicorn % 3;
			if(evilUnicorns[nextEvilUnicorn].active == 0) {
				break;
			}
		}
		if(evilUnicorns[nextEvilUnicorn].active == 1) {
			return;
		}
	}
	evilUnicorns[nextEvilUnicorn].active = 1;

	evilUnicorns[nextEvilUnicorn].x = x*256;
	evilUnicorns[nextEvilUnicorn].y = y*256;
	evilUnicorns[nextEvilUnicorn].dx = dx;
	evilUnicorns[nextEvilUnicorn].dy = dy;

	evilUnicorns[nextEvilUnicorn].spc = 0;
	evilUnicorns[nextEvilUnicorn].sf = sf;

	evilUnicorns[nextEvilUnicorn].lrc = 0;
	evilUnicorns[nextEvilUnicorn].lrmax = 0;

	evilUnicorns[nextEvilUnicorn].dead = 0;
}

void spawnBoss() {
	if(evilUnicorns[3].active == 0) {
		evilUnicorns[3].active = 1;
		evilUnicorns[3].life = 10;
		evilUnicorns[3].x = (256/2)*256;
		evilUnicorns[3].y = (-64)*256;
		evilUnicorns[3].dx = 1000;
		evilUnicorns[3].dy = 600;
		evilUnicorns[3].lrc = 40;
	}
}

int updateAllUnicorns(int t) {
	for( int i = 0; i < 5; i++ ) {
		if(niceUnicorns[i].active == 1) {
			niceUnicorns[i].x += niceUnicorns[i].dx;
			niceUnicorns[i].y += niceUnicorns[i].dy;
			niceUnicorns[i].spc++;
			if(niceUnicorns[i].spc >= 20) {
				niceUnicorns[i].spc = 0;
				int bulx = niceUnicorns[i].dx < 0 ? niceUnicorns[i].x/256+41 : niceUnicorns[i].x/256+(64-41);
				spawnBullet(bulx,niceUnicorns[i].y/256+21,(t%700)-350,1000,0,700,2,0);
			}
			if(niceUnicorns[i].x/256 > 256 || niceUnicorns[i].x/256 < -64) {
				niceUnicorns[i].active = 0;
			}
			
			int suby = (niceUnicorns[i].y/256);
			int mainy = (niceUnicorns[i].y/256)-192;
			
			oamSet(
				&oamSub, i+81,
				(niceUnicorns[i].x>>8),
				suby,
				0, 0,
				SpriteSize_64x64,
				SpriteColorFormat_256Color,
				unicornSprite,
				-1,
				true, false, niceUnicorns[i].dx > 0, false, false
			);
		}
	}

	for( int i = 0; i < 3; i++ ) {
		if(evilUnicorns[i].active == 1) {
			evilUnicorns[i].x += evilUnicorns[i].dx;
			evilUnicorns[i].y += evilUnicorns[i].dy;
			evilUnicorns[i].spc++;
			evilUnicorns[i].lrc++;
	
			if(evilUnicorns[i].lrc >= 60 && evilUnicorns[i].dead == 0) {
				evilUnicorns[i].lrmax++;

				if(evilUnicorns[i].lrmax < 4) {
					evilUnicorns[i].dx = -evilUnicorns[i].dx;
					evilUnicorns[i].dy = 0;
					evilUnicorns[i].lrc = 20;
				}
				else {
					evilUnicorns[i].dx = 0;
					evilUnicorns[i].dy = -1500;
				}
					
			}
			if( evilUnicorns[i].dead == 0 ) {
				if(evilUnicorns[i].spc >= evilUnicorns[i].sf) {
					evilUnicorns[i].spc = 0;
					spawnBullet(evilUnicorns[i].x/256+32-8,evilUnicorns[i].y/256+42,(t%550)-275,1200,0,0,1,2);
				}
			}
			if(evilUnicorns[i].x/256 > 256 || evilUnicorns[i].x/256 < -64 || evilUnicorns[i].y/256 < -64 ) {
				evilUnicorns[i].active = 0;
			}

			int suby = (evilUnicorns[i].y/256);
			int mainy = (evilUnicorns[i].y/256)-192;

			oamSet(
				&oamSub, i+86,
				(evilUnicorns[i].x>>8),
				suby,
				0, 0,
				SpriteSize_64x64,
				SpriteColorFormat_256Color,
				evilUnicorns[i].dead == 0 ? evilUnicornSprite : deadUnicornSprite,
				-1,
				true, false, false, false, false
			);
		}
	}

	if(evilUnicorns[3].active == 1) {
		evilUnicorns[3].x += evilUnicorns[3].dx;
		evilUnicorns[3].y += evilUnicorns[3].dy;
		evilUnicorns[3].spc++;
		evilUnicorns[3].lrc++;
		
		int suby = (evilUnicorns[3].y/256);


		if(evilUnicorns[3].spc > 10) {
			spawnBullet(evilUnicorns[3].x/256-8,evilUnicorns[3].y/256+42,(t%550)-275,1200,0,0,1,2);
			evilUnicorns[3].spc = 0;
		}
		
		if(evilUnicorns[3].lrc > 80) {
			evilUnicorns[3].lrc = 0;
			evilUnicorns[3].dx = -evilUnicorns[3].dx;
			evilUnicorns[3].dy = 0;
		}
		
		oamSet(
			&oamSub, 110,
			(evilUnicorns[3].x>>8)-32-32,
			suby,
			0, 0,
			SpriteSize_64x64,
			SpriteColorFormat_256Color,
			bossUnicornL,
			-1,
			true, false, false, false, false
		);
		oamSet(
			&oamSub, 111,
			(evilUnicorns[3].x>>8)+32-32,
			suby,
			0, 0,
			SpriteSize_64x64,
			SpriteColorFormat_256Color,
			bossUnicornR,
			-1,
			true, false, false, false, false
		);
		
	}
}
