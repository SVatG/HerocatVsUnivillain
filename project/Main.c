#include <nds.h>
#include <stdio.h>

#include <nds/ndstypes.h>
#include <nds/interrupts.h>
#include <string.h>
#include <game_stats.h>

// Utilities to make development easier
#include "Utils.h"

// NitroFS access
#include "nitrofs.h"

// Effects!
#include "lowerscreen_display.h"

// Sound!
#include <maxmod9.h>
#include "music.h"

#include "kitten.h"
#include "bullets.h"

volatile uint32_t t;
static void vblank();

extern int tempImage;
extern int noise[41][41];
extern int yp;
extern int stopfloor;

uint8_t ATTR_DTCM dtcm_buffer[12288];

void fadeout(int t, int b) {
	uint16_t* master_bright = (uint16_t*)(0x400006C);
	if( t > b-16 ) {
		uint16_t val = 18-(b-t);
		memset( master_bright, (1<<7) | val, 2 );
	}
// 	else {
// 		memset( master_bright, (1<<7) | 15, 2 );
// 	}
}

void fadein(int t, int b) {
	uint16_t* master_bright = (uint16_t*)(0x400006C);
	if( t < b+16 ) {
		uint16_t val = (b+17-t);
		memset( master_bright, (1<<7) | val, 2 );
	}
	else {
		memset( master_bright, (1<<7) | 0, 2 );
	}
}


int main()
{
	// Turn on everything.
	POWCNT1 = POWCNT1_ALL_SWAP;
	irqEnable( IRQ_VBLANK );
	irqSet(IRQ_VBLANK,vblank);

//	ClaimWRAM();

	// Init NitroFS for data loading.
	nitroFSInitAdv( BINARY_NAME );
	tempImage = malloc(256*256*2);

	uint8_t *wram=(uint8_t *)0x3000000;
//	memset(wram,0,128*96);

	mmInitDefault( "nitro:/zik/music.bin" );
	mmLoad( MOD_SVATGGAME );
	mmStart( MOD_SVATGGAME, MM_PLAY_ONCE );

	POWCNT1 = POWCNT1_ALL;

	initGame();
	resetGame();


	while(1) {
		menu_init();
		int menuRunning = 1;
		while(menuRunning == 1) {
			menu_update(t++);
			scanKeys();
			int keys = keysHeld();
			if(keys & KEY_A) {
				menuRunning = 0;
				resetGame();
				t = 0;
				scanKeys();
			}
			swiWaitForVBlank();
			
		}
		int spc = 0;
		int espc = 0;
		int unifreq = 200;
		int unilast = 0; // 0 -> left, 1 -> right;
		int mode = 0;
		int gameRunning = 1;
		while(gameRunning == 1) {

			if( mode != 0 ) {
				t++;
				spc++;
				espc++;
				stopfloor = 0;
				updateBullets();
				updateAllUnicorns(t);
				KittenUpdate(&Cat);
			}
			else {
				stopfloor = 1;
			}

			if(spc >= 100) {
				spc = 0;
				if(unilast == 0) {
					unicornRight();
					unilast = 1;
				}
				else {
					unicornLeft();
					unilast = 0;
				}
			}

			if(espc >= unifreq) {
				if(unifreq > 60) {
					unifreq -= 20;
				}
				espc = 0;
				if(Random()%2 == 0) {
					evilLeft();
				}
				else {
					evilRight();
				}
			}

			int keys = keysHeld();
			if(keys & KEY_A) {
				catShoot(t);
			}
			consoleClear();
			gotoxy(3,3);
			iprintf("t = %d\n", t);
			scoreAdd(1);
			printOSD();
			if(t >= 25000) {
				lowerscreen_update(25000);
			}
			else {
				lowerscreen_update(t);
			}
			swiWaitForVBlank();
		}
	}
	for(;;);

	return 0;
}

static void vblank() { t; }


