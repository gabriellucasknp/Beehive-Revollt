#ifndef COLLISION_H
#define COLLISION_H

#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "assets.h"

/* Returns points earned this frame */
int collision_update(Player *p, EnemyGrid *g, BulletPool *bp);

#endif /* COLLISION_H */
