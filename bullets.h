#ifndef _BULLETS_H
#define _BULLETS_H

void initBullets();
void spawnBullet(int x, int y, int dx, int dy, int16_t rot, int16_t drot, int alleg, int type );
void updateBullets();

#endif
