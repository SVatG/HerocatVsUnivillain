// AO Cubes

#include <nds.h>

#include "DS3D/Utils.h"
#include "DS3D/DS3D.h"
#include "DS3D/Vector/Vector.h"
#include "Hardware.h"
#include "Loader.h"
#include "RainbowTable.h"
#include "perlin.h"

void lowerscreen_init() {
	DISPCNT_A=DISPCNT_MODE_5|DISPCNT_3D|DISPCNT_BG0_ON|DISPCNT_BG3_ON|DISPCNT_ON;

	VRAMCNT_A = VRAMCNT_A_LCDC;

	int dx = icos(128)/26.5;
	int dy = isin(128)/26.5;
	BG2PA_B = dx;
	BG2PB_B = dy;
	BG2PC_B = -dy;
	BG2PD_B = dx;
	BG2X_B = 4000;
	BG2Y_B = 10000;

	// Set up voxelcubes
	VRAMCNT_D=VRAMCNT_D_LCDC;
	VRAMCNT_F=VRAMCNT_F_LCDC;

	DSInit3D();
	DSViewport(0,0,255,191);

	DSSetControl(DS_ANTIALIAS);
	DSClearParams(26,26,26,0,63);

	DSSetPaletteOffset(0,DS_TEX_FORMAT_PAL4);

	DSMatrixMode(DS_PROJECTION);
	DSLoadIdentity();
	DSPerspectivef(100,256.0/192.0,1,1024);

	// Background
	VRAMCNT_B = VRAMCNT_B_BG_VRAM_A_OFFS_0K;
	VRAMCNT_C = VRAMCNT_C_BG_VRAM_A_OFFS_128K;

	BG3CNT_A = BGxCNT_EXTENDED_BITMAP_8 | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_OVERFLOW_TRANSPARENT | BGxCNT_BITMAP_BASE_0K;
	BG3CNT_A = (BG3CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_0;
	BG3PA_A = (1 << 8);
	BG3PB_A = 0;
	BG3PC_A = 0;
	BG3PD_A = (1 << 8);
	BG3X_A = 0;
	BG3Y_A = 0;

	load8bVRAMIndirect( "nitro:/gfx/aocubes.img.bin", VRAM_A_OFFS_0K,256*192*2);
	loadVRAMIndirect( "nitro:/gfx/aocubes.pal.bin", PALRAM_A,256*2);
}

void voxelSpiral(int t) {
	static uint8_t ri = 0;
	DSMatrixMode(DS_POSITION_AND_VECTOR);
	DSLoadIdentity();
	DSLight3f(0,0xffff,0,0.99,0);
	
	// Move things
	DSTranslatef32(DSf32(0),DSf32(0),-DSf32(5));

	//DSLight(0,0xffff,0x200);
	int scale = 1024;
	DSScalef32(DSf32(scale),DSf32(scale),DSf32(scale));

	DISP3DCNT|=DS_OUTLINE;
	DSPolygonAttributes(DS_POLY_MODE_MODULATION|DS_POLY_CULL_NONE|DS_POLY_LIGHT0|DS_POLY_ALPHA(31));

	//DSRotateYf(t);
	//DSRotateXf(t);
	
	DSBegin(DS_QUADS);
	for( int x = -10; x <= 10; x++ ) {
		for( int y = -10; y <= 10; y++ ) {
			int ya = (noise_at((x+10)/40.0,(y+10)/40.0,t*0.01)*5.0)-4.0;
			int yb = (noise_at((x+10)/40.0,(y+1+10)/40.0,t*0.01)*5.0)-4.0;
			int yc = (noise_at((x+1+10)/40.0,(y+1+10)/40.0,t*0.01)*5.0)-4.0;
			int yd = (noise_at((x+1+10)/40.0,(y+10)/40.0,t*0.01)*5.0)-4.0;
			
			DSMaterialDiffuseAndAmbient(rainbowTable[x+y+20]|0x8000,0);
			vec3_t a = vec3(0, ya-yb, -1);
			vec3_t b = vec3(-1, ya-yc, -1);
			vec3_t n = vec3cross(a,b);
			n = vec3norm(n);
			DSNormal3f(-n.x,-n.y,-n.z);
			
			DSVertex3v16( x,   ya, y   );
			DSVertex3v16( x,   yb, y+1 );
			DSVertex3v16( x+1, yc, y+1 );
			DSVertex3v16( x+1, yd, y   );
		}
	}
	DSEnd();
	
	DSSwapBuffers(0);
}

uint8_t lowerscreen_update( uint32_t t ) {
	voxelSpiral(t);
	return 0;
}


void lowerscreen_destroy() {
	// Nothing
}

