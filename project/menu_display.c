// AO Cubes

#include <nds.h>

#include "DS3D/Utils.h"
#include "DS3D/DS3D.h"
#include "DS3D/Vector/Vector.h"
#include "Hardware.h"
#include "Loader.h"
#include "RainbowTable.h"
#include "perlin.h"

#include "game_stats.h"

#define RESX 40
#define RESY 40

void menu_init() {
	load8bVRAMIndirect( "nitro:/gfx/titlescreen1.img.bin", VRAM_B_OFFS_0K,256*192);
	loadVRAMIndirect( "nitro:/gfx/titlescreen1.pal.bin", PALRAM_B,256*2);

	load8bVRAMIndirect( "nitro:/gfx/starfieldb.img.bin", VRAM_A_OFFS_32K,256*192);
	loadVRAMIndirect( "nitro:/gfx/starfieldb.pal.bin", PALRAM_A,256*2-1);

	BG_PALETTE_SUB[255] = RGB15(31,31,31);
	consoleInit(0, 1,BgType_Text4bpp, BgSize_T_256x256, 39,0, true, true);
}

void gotoxy(int x, int y){
    iprintf("%c[%d;%df",0x1B,y,x);
}

void menuGfx(int t) {
	consoleClear();
	gotoxy(1,1);
	iprintf("Highscore: %d\n", highscore);
	
	gotoxy(9,11);
	if((t>>4)%2 == 0) {
		iprintf("> Start game <\n");
	}
	else {
		iprintf("  Start game \n");
	}
}

uint8_t menu_update( uint32_t t ) {
	menuGfx(t);
	return 0;
}


void menu_destroy() {
	// Nothing
}

