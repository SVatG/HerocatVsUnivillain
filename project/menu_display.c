// AO Cubes

#include <nds.h>

#include "DS3D/Utils.h"
#include "DS3D/DS3D.h"
#include "DS3D/Vector/Vector.h"
#include "Hardware.h"
#include "Loader.h"
#include "RainbowTable.h"
#include "perlin.h"

#define RESX 40
#define RESY 40

int noise[RESX+1][RESY+1];

int highscore = 66666;

void menu_init() {

	DISPCNT_B = DISPCNT_MODE_5 | DISPCNT_BG2_ON | DISPCNT_ON;
	VRAMCNT_C = VRAMCNT_C_BG_VRAM_B;
	
	BG2CNT_B = BGxCNT_EXTENDED_BITMAP_8 | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_OVERFLOW_TRANSPARENT | BGxCNT_BITMAP_BASE_0K;
	BG2CNT_B = (BG2CNT_B&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_0;
	BG2PA_B = (1 << 8);
	BG2PB_B = 0;
	BG2PC_B = 0;
	BG2PD_B = (1 << 8);
	BG2X_B = 0;
	BG2Y_B = 0;

	DISPCNT_A = DISPCNT_MODE_5|DISPCNT_BG1_ON|DISPCNT_BG3_ON|DISPCNT_ON;
	VRAMCNT_B = VRAMCNT_B_BG_VRAM_A;

	BG3CNT_A = BGxCNT_EXTENDED_BITMAP_8 | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_OVERFLOW_TRANSPARENT | BGxCNT_BITMAP_BASE_32K;
	BG3CNT_A = (BG3CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_0;
	BG3PA_A = (1 << 8);
	BG3PB_A = 0;
	BG3PC_A = 0;
	BG3PD_A = (1 << 8);
	BG3X_A = 0;
	BG3Y_A = 0;

	load8bVRAMIndirect( "nitro:/gfx/starfieldb.img.bin", VRAM_A_OFFS_32K,256*192);
	loadVRAMIndirect( "nitro:/gfx/starfieldb.pal.bin", PALRAM_A,256*2);

	load8bVRAMIndirect( "nitro:/gfx/starfielda.img.bin", VRAM_B_OFFS_0K,256*192);
	loadVRAMIndirect( "nitro:/gfx/starfielda.pal.bin", PALRAM_B,256*2);

	BG_PALETTE_SUB[255] = RGB15(31,31,31);

	const int tile_base = 0;
	const int map_base = 20;

	consoleInit(0, 1,BgType_Text4bpp, BgSize_T_256x256, 39,0, true, true);

	load8bVRAMIndirect( "nitro:/gfx/starfieldb.img.bin", VRAM_A_OFFS_32K,256*192);
	loadVRAMIndirect( "nitro:/gfx/starfieldb.pal.bin", PALRAM_A,256*2-1);
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

