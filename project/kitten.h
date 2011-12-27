#ifndef _KITTEN_H
#define _KITTEN_H

enum KittenState { STANDING=0, WALKING=2 };

typedef struct 
{
	uint_least8_t x;
	uint_least8_t y;
 
	uint16_t* spriteWalking[4];
	uint16_t* spriteStanding[4];
	uint16_t* current_sprite;
	
	uint8_t   frametime;
	
	uint16_t* palette;
	
	enum KittenState lastState;
 
	enum KittenState state; //walking, standing
	int frame;
} Kitten;

int KittenUpdate(Kitten* cat);
void KittenInit(Kitten* cat);

#endif