#ifndef BULLET_H
#define BULLET_H

#include "types.h"
#include "assets.h"
#include <stdbool.h>

typedef struct {
    float x, y;
    float vy;       /* negative = up (player), positive = down (enemy) */
    bool  active;
} Bullet;

/* pools */
typedef struct {
    Bullet player[MAX_PLAYER_BULLETS];
    Bullet enemy[MAX_ENEMY_BULLETS];
} BulletPool;

void bullets_init(BulletPool *bp);
void bullets_update(BulletPool *bp, float dt);
void bullets_draw(const BulletPool *bp, const Assets *a);

/* returns true if spawned */
bool bullet_spawn_player(BulletPool *bp, float x, float y);
bool bullet_spawn_enemy(BulletPool *bp, float x, float y);

#endif /* BULLET_H */
