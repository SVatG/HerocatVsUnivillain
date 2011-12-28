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

	t = 0;

	uint8_t *wram=(uint8_t *)0x3000000;
//	memset(wram,0,128*96);

	mmInitDefault( "nitro:/zik/music.bin" );
	mmLoad( MOD_SVATGGAME );
	mmStart( MOD_SVATGGAME, MM_PLAY_ONCE );

	POWCNT1 = POWCNT1_ALL;

	resetGame();

	int spc = 0;
	while(1) {
		t++;
		spc++;
		if(spc >= 50) {
			spc = 0;
			spawnUnicorn(256,-64,-1200,600);
			spawnEvilUnicorn(-64,10,1200,500);
		}

		int keys = keysHeld();
		if(keys & KEY_A) {
			catShoot(t);
		}
		consoleClear();
		scoreAdd(2);
		printOSD();
		lowerscreen_update(t);
		updateBullets();
		updateAllUnicorns(t);
		KittenUpdate(&Cat);
 		swiWaitForVBlank();
	}
	
	for(;;);

	return 0;
}

static void vblank() { t++; }


