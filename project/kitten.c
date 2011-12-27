#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <fcntl.h>
#include <unistd.h>

#include "nitrofs.h"

#include "kitten.h"
#include "Hardware.h"

#define FRAME_DELAY 8

int nitroLoad(char *path, uint16_t* buffer, uint32_t size) {
  int fd_reuse = open(path, O_RDONLY);
  read(fd_reuse, buffer, size);
  close(fd_reuse);
}

// Load an 32x32 sprite into A/B OBJ RAM, return pointer.
static uint16_t* loadSpriteA32( char* path ) {
	uint16_t* newSprite = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
	nitroLoad( path, newSprite, 32*32 );
	return( newSprite );
}

static void KittenGoLeft(Kitten* cat) {
  if (cat->x > 0) { cat->x-=2; }
}

static void KittenGoRight(Kitten* cat) {
  if (cat->x < SCREEN_WIDTH-32-2) { cat->x+=2; }
}

void KittenInit(Kitten* cat) {
  DISPCNT_A = DISPCNT_MODE_0 | DISPCNT_BG3_ON | DISPCNT_OBJ_ON | DISPCNT_ON;
  VRAMCNT_D = VRAMCNT_D_BG_VRAM_A_OFFS_128K;
  VRAMCNT_B = VRAMCNT_B_BG_VRAM_A_OFFS_0K;

  BG3CNT_A = BGxCNT_EXTENDED_BITMAP_16 | BGxCNT_BITMAP_SIZE_256x256 | BGxCNT_OVERFLOW_WRAP | BGxCNT_BITMAP_BASE_0K;
  BG3CNT_A = (BG3CNT_A&~BGxCNT_PRIORITY_MASK)|BGxCNT_PRIORITY_1;
  
  VRAMCNT_A = VRAMCNT_A_OBJ_VRAM_A;
  
  oamInit(&oamMain, SpriteMapping_1D_128, false);
  oamInit(&oamSub, SpriteMapping_1D_128, false);
  
  cat->state=STANDING;
  cat->palette=NULL;
  cat->x=SCREEN_WIDTH/2-16;
  cat->y=SCREEN_HEIGHT - 64;
  cat->frame=0;
  cat->frametime=0;
  
  cat->spriteWalking[0] = loadSpriteA32("nitro:/gfx/lil_cat3_walk1.img.bin");
  cat->spriteWalking[1] = loadSpriteA32("nitro:/gfx/lil_cat3_walk2.img.bin");
  cat->spriteWalking[2] = loadSpriteA32("nitro:/gfx/lil_cat3_walk3.img.bin");
  cat->spriteWalking[3] = loadSpriteA32("nitro:/gfx/lil_cat3_walk4.img.bin");
  
  cat->spriteStanding[0] = loadSpriteA32("nitro:/gfx/lil_cat3_tail-L.img.bin");
  cat->spriteStanding[1] = loadSpriteA32("nitro:/gfx/lil_cat3_tail-C.img.bin");
  cat->spriteStanding[2] = loadSpriteA32("nitro:/gfx/lil_cat3_tail-R.img.bin");
  cat->spriteStanding[3] = cat->spriteStanding[1];
  
  cat->palette = malloc(512);
  nitroLoad("nitro:/gfx/lil_cat3_walk1.pal.bin", cat->palette, 512);
  dmaCopy(cat->palette, SPRITE_PALETTE, 512); //copy the sprites palette
}

static int KittenAnimate(Kitten* cat) {
  if(cat->frametime++ > FRAME_DELAY) {
    cat->frametime=0;
    if (WALKING == cat->state) {
      if(cat->frame++ < sizeof(cat->spriteStanding)/sizeof(uint16_t*)-1 ) {
	cat->current_sprite = cat->spriteWalking[cat->frame];
      } else {
	cat->frame = 0;
	cat->current_sprite = cat->spriteWalking[cat->frame];
      }
    } else if (STANDING == cat->state) {
      if(cat->frame++ < sizeof(cat->spriteWalking)/sizeof(uint16_t*)-1 ) {
	cat->current_sprite = cat->spriteStanding[cat->frame];
      } else {
	cat->frame = 0;
	cat->current_sprite = cat->spriteStanding[cat->frame];
      }
    }
  }
}

int KittenUpdate(Kitten* cat) {
    int i,j,k;
    static int t=0;
    t++;
    
    touchPosition touch;
    scanKeys();
    
    int keys = keysHeld();
    
    if(keys & KEY_TOUCH) {
      touchRead(&touch);
      if ( touch.px != 0x0  && touch.py != 0x0 )
      {
	//BG_GFX[touch.px + touch.py * 256] = rand();
	//bg_buffer[touch.px + touch.py*SCREEN_WIDTH] = RGB15(5,5,31) | BIT(15);
	for(i=-2;i<3;i++)
	{
	  for(j=-2;j<3;j++) {
	    //bg_buffer_sub[touch.px+i + (touch.py+j)* SCREEN_WIDTH] = RGB15(5,10,31) | BIT(15);	
	  }
	}
      }
    }
    
    if(keys & KEY_LEFT) {
	KittenGoLeft(cat);
	cat->state = WALKING;
      oamRotateScale(
	    &oamMain,
	    1,
	    270,
	    252,
	    252
      );
    } else if(keys & KEY_RIGHT) {
	KittenGoRight(cat);
	cat->state = WALKING;
      int swoop = (t<<13)/50;
      oamRotateScale(
	    &oamMain,
	    1,
	    -270,
	    252,
	    252
      );
    } else if (WALKING == cat->lastState) {
      cat->state = STANDING;
    } else {
      cat->state = STANDING;
      if (cat->frame > sizeof(cat->spriteWalking)/sizeof(uint16_t*)-1)
      { cat->frame=0; }
      oamRotateScale(
	    &oamMain,
	    1,
	    0,
	    256,
	    256
      );  
    }
  KittenAnimate(cat);
  cat->lastState = cat->state;
}