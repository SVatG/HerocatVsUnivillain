// AO Cubes

#include <nds.h>
#include "game_stats.h"

#include "DS3D/Utils.h"
#include "DS3D/DS3D.h"
#include "DS3D/Vector/Vector.h"
#include "Hardware.h"
#include "Loader.h"
#include "RainbowTable.h"
#include "perlin.h"

void lowerscreen_init() {
	
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
	
	for( int x = -RESX/2; x <= RESX/2; x++ ) {
		for( int y = -RESY/2; y <= RESY/2; y++ ) {
			noise[x+RESX/2][y+RESY/2] = (noise_at((x+RESX/2)/40.0,-(y-0.1*20.0-RESY/2)/40.0,1*0.01)*5.0);
		}
	}
	
	//DISPCNT_A=DISPCNT_MODE_5|DISPCNT_3D|DISPCNT_BG0_ON|DISPCNT_BG3_ON|DISPCNT_ON|DISPCNT_OBJ_ON;
	//DISPCNT_A=DISPCNT_MODE_0|DISPCNT_3D|DISPCNT_BG0_ON|DISPCNT_BG3_ON|DISPCNT_ON|DISPCNT_OBJ_ON;
	VRAMCNT_B = VRAMCNT_B_BG_VRAM_A;

	DSInit3D();
	DSViewport(0,0,255,191);

	DSSetControl(DS_ANTIALIAS);
	DSClearParams(26,26,26,0,63);

	DSMatrixMode(DS_PROJECTION);
	DSLoadIdentity();
	DSPerspectivef(100,256.0/192.0,1,1024);

	BG3CNT_A = BGxCNT_EXTENDED_BITMAP_8 | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_OVERFLOW_TRANSPARENT | BGxCNT_BITMAP_BASE_32K;
	BG3CNT_A = (BG3CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_2;
	BG3PA_A = (1 << 8);
	BG3PB_A = 0;
	BG3PC_A = 0;
	BG3PD_A = (1 << 8);
	BG3X_A = 0;
	BG3Y_A = 0;
	
	load8bVRAMIndirect( "nitro:/gfx/starfielda.img.bin", VRAM_B_OFFS_0K,256*192);
	loadVRAMIndirect( "nitro:/gfx/starfielda.pal.bin", PALRAM_B,256*2);

	BG_PALETTE_SUB[255] = RGB15(31,31,31);
	consoleInit(0, 1,BgType_Text4bpp, BgSize_T_256x256, 39,0, true, true);
	BG0CNT_A = (BG0CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_1;

	load8bVRAMIndirect( "nitro:/gfx/starfieldb.img.bin", VRAM_A_OFFS_32K,256*192);
	loadVRAMIndirect( "nitro:/gfx/starfieldb.pal.bin", PALRAM_A,256*2-1);
}

void gameGfx(int t) {
	static uint8_t ri = 0;
	DSMatrixMode(DS_POSITION_AND_VECTOR);
	DSLoadIdentity();
	vec3_t lightd = vec3(10,10,0);
	lightd = vec3norm(lightd);
	
	DSLight3f(0,0xffff,lightd.x,lightd.y,lightd.z);
	
	// Move things
	DSTranslatef32(DSf32(0),DSf32(0),-DSf32(5));

	//DSLight(0,0xffff,0x200);
	int scale = 1024;
	//DSScalef32(DSf32(scale),DSf32(scale),DSf32(scale));

	DSPolygonAttributes(DS_POLY_MODE_MODULATION|DS_POLY_CULL_NONE|DS_POLY_LIGHT0|DS_POLY_ALPHA(31));

	//DSRotateYf(t);
	//DSRotateXf(0);

	int movf = (t<<10)%(1<<10);
	if( movf == 0 ) {
		yp++;
		for( int x = -RESX/2; x <= RESX/2; x++ ) {
			noise[x+RESX/2][(yp)%RESY] = (noise_at((x+RESX/2)/40.0,-(-yp-0.1*20.0-RESY)/40.0,1*0.01)*5.0);
		}
	}
			
	DSBegin(DS_QUADS);
	for( int x = -RESX/2; x < RESX/2; x++ ) {
		for( int y = -RESY/2; y < RESY/2; y++ ) {
			int yv = (y<<10)-movf;
			int yv1 = ((y+1)<<10)-movf;
			int yt = (yv+(RESY<<9)) >> 8;
			int yt1 = (yv1+(RESY<<9)) >> 8;
			int ys = (yt*yt);
			int ys1 = (yt1*yt1);

			int shift = 15000;
			int na = noise[x+RESX/2][(y+RESY/2+yp)%RESY];
			int nb = noise[x+RESX/2][(y+1+RESY/2+yp)%RESY];
			int nc = noise[x+1+RESX/2][(y+1+RESY/2+yp)%RESY];
			int nd = noise[x+1+RESX/2][(y+RESY/2+yp)%RESY];
			int ya = (na<<10)+shift;
			int yb = (nb<<10)+shift;
			int yc = (nc<<10)+shift;
			int yd = (nd<<10)+shift;
			
			vec3_t a = vec3((0<<10), ya-yb, -(1<<10));
			vec3_t b = vec3(-(1<<10), ya-yc, -(1<<10));
			vec3_t n = vec3cross(a,b);
			n = vec3norm(n);

			ya -= (abs(x)<<8)+ys;
			yb -= (abs(x)<<8)+ys1;
			yc -= (abs(x+1)<<8)+ys1;
			yd -= (abs(x+1)<<8)+ys;

			if(na > -2 || nb > -2 || nc > -2 || nd > -2) {
				DSMaterialDiffuseAndAmbient(MakeRGB15(6,3,0)|0x8000,0);
				DSNormal3f(-n.x,-n.y,-n.z);
			}
			else {
				DSColor3b(18,0,0);
				//DSMaterialDiffuseAndAmbient(MakeRGB15(14,0,0))|0x8000,0);
			}			
			DSVertex3v16( (x)<<10, yv, ya );
			DSVertex3v16( (x)<<10, yv1, yb );
			DSVertex3v16( (x+1)<<10, yv1, yc );
			DSVertex3v16( (x+1)<<10, yv, yd );
		}
	}
	DSEnd();
	
	DSSwapBuffers(0);
}

uint8_t lowerscreen_update( uint32_t t ) {
	gameGfx(t);
	return 0;
}


void lowerscreen_destroy() {
	// Nothing
}

