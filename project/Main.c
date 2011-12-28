#include <nds.h>
#include <stdio.h>

#include <nds/ndstypes.h>
#include <nds/interrupts.h>
#include <string.h>
#include "game_stats.h"

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

static char card_id[5] = { 0,0,0,0,0 }; // for saving

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

int diapos = 0;
char* dialog[] = {
	" Hero cat! This is Cat HQ \n calling!",
	" The evil UNIVILLAIN has \n scrambled an army and is \n trying to ENSLAVE MANKIND!",
	" We are short on missiles, so \n you must collect marshmallows \n from friendly unicorns!",
	" This is the only way to \n destroy them! You are our \n last hope!",
	" Hero Cat is... GO!!!",
	" p.s. greets to Mercury, K2, \n Nuance and RNO.\n",
	0
};

uint16_t* diaboxa;
uint16_t* diaboxb;

void showdiabox() {
	oamSet(
		&oamMain, 120,
		0,
		192-64,
		1, 0,
		SpriteSize_64x64,
		SpriteColorFormat_256Color,
		diaboxa,
		-1,
		true, false, false, false, false
	);

	oamSet(
		&oamMain, 121,
		64,
		192-64,
		1, 0,
		SpriteSize_64x64,
		SpriteColorFormat_256Color,
		diaboxb,
		-1,
		true, false, false, false, false
	);

	oamSet(
		&oamMain, 122,
		128,
		192-64,
		1, 0,
		SpriteSize_64x64,
		SpriteColorFormat_256Color,
		diaboxb,
		-1,
		true, false, false, false, false
	);

	oamSet(
		&oamMain, 123,
		192,
		192-64,
		1, 0,
		SpriteSize_64x64,
		SpriteColorFormat_256Color,
		diaboxa,
		-1,
		true, false, true, false, false
	);
}

void hidediabox() {
	oamSet(
		&oamMain, 120,
		257,
		192-64,
		1, 0,
		SpriteSize_64x64,
		SpriteColorFormat_256Color,
		diaboxa,
		-1,
		true, false, false, false, false
	);

	oamSet(
		&oamMain, 121,
		257,
		192-64,
		1, 0,
		SpriteSize_64x64,
		SpriteColorFormat_256Color,
		diaboxb,
		-1,
		true, false, false, false, false
	);

	oamSet(
		&oamMain, 122,
		257,
		192-64,
		1, 0,
		SpriteSize_64x64,
		SpriteColorFormat_256Color,
		diaboxb,
		-1,
		true, false, false, false, false
	);

	oamSet(
		&oamMain, 123,
		257,
		192-64,
		1, 0,
		SpriteSize_64x64,
		SpriteColorFormat_256Color,
		diaboxa,
		-1,
		true, false, true, false, false
	);
}

  /* Return true if the card id is 'PASS' */
  static int is_homebrew_cartridge() {
    return 
      card_id[0] == 'P' &&
      card_id[1] == 'A' &&
      card_id[2] == 'S' &&
      card_id[3] == 'S';
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
	
	/* Copy contents of the 4 character ROM identifier into a local
       variable. This should be 'PASS' for all homebrew ROM's. The
       identifier is located at 0x080000AC.*/
	memcpy(card_id, (char*)0x080000AC, 4);

	uint8_t *wram=(uint8_t *)0x3000000;
//	memset(wram,0,128*96);

	mmInitDefault( "nitro:/zik/music.bin" );
	
	mmLoad( MOD_SVATGGAME );
	mmLoadEffect( SFX_LASER	);
	mmLoadEffect( SFX_MEOW );
	mmLoadEffect( SFX_POWERUP );
	mmLoadEffect( SFX_SLAUGHTER1 );
	mmLoadEffect( SFX_SLAUGHTER2 );
	mmLoadEffect( SFX_SLAUGHTER3 );
	mmLoadEffect( SFX_SLAUGHTER4 );
	
	mmStart( MOD_SVATGGAME, MM_PLAY_ONCE );
	
	POWCNT1 = POWCNT1_ALL;

	initGame();
	resetGame();
	diaboxa = loadSpriteA64("nitro:/gfx/textboxleft.img.bin");
	diaboxb = loadSpriteA64("nitro:/gfx/textboxcenter.img.bin");

	while(1) {
		menu_init();
		int menuRunning = 1;
		while(menuRunning == 1) {
			oamClear(&oamMain,0,0);
			oamClear(&oamSub,0,0);
			menu_update(t++);
			scanKeys();
			int keys = keysDown();
			if(keys & KEY_A) {
				menuRunning = 0;
				resetGame();
				t = 0;
				diapos = 0;
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
			consoleClear();
			gotoxy(1,4);
			iprintf("t = %d\n", t);
			if( mode != 0 ) {
				t++;
				spc++;
				espc++;
				stopfloor = 0;
				updateBullets();
				updateAllUnicorns(t);
				scoreAdd(1);				
				KittenUpdate(&Cat);
				killCatIfDying(t);
				oamClear(&oamMain,0,0);
				oamClear(&oamSub,0,0);
				if( catRecentlyDied() == 1 ) {
					gameRunning = 0;
					oamClear(&oamMain,0,0);
					oamClear(&oamSub,0,0);
				}
			}
			else {
				if(dialog[diapos] != 0) {
					gotoxy(0,17);
					iprintf(dialog[diapos]);
				}
				
				stopfloor = 1;
				scanKeys();
				int keys = keysDown();
				showdiabox();
				if(keys & KEY_A) {
					scanKeys();
					diapos++;
					if(dialog[diapos] == 0) {
						stopfloor = 0;
						diapos++;
						mode = 1;
						hidediabox();
					}
				}
				oamUpdate(&oamMain);
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


