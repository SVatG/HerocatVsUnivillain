#include "game_stats.h"

int noise[RESX+1][RESY+1];
int yp = 0;
Kitten Cat;
int highscore;
int score;

void resetGame() {
	yp = 0;
	KittenInit(&Cat);
	lowerscreen_init();
}

int floorLeftHeight() {
	return( noise[(Cat.x-3)/6+4][(yp+13)%40] );
}

int floorCenterHeight() {
	return( noise[(Cat.x)/6+4][(yp+13)%40] );
}

int floorRightHeight() {
	return( noise[(Cat.x+3)/6+4][(yp+13)%40] );
}

int catOnLava() {
	return(
		floorLeftHeight() <= -2 &&
		floorCenterHeight() <= -2 &&
		floorRightHeight() <= -2
	);
}

void printOSD() {
	gotoxy(1,1);
	iprintf("Score: %d\n", score);
	if(catOnLava()) {
		gotoxy(1,2);
		iprintf("Cat status: Dead\n");
	}
}

void scoreAdd(int howMuch) {
	score += howMuch;
}
