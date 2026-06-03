#ifndef ENEMY_H
#define ENEMY_H

#include "types.h"
#include "assets.h"
#include "bullet.h"
#include "player.h"
#include <stdbool.h>

typedef enum {
    ENEMY_FORMATION,
    ENEMY_RUSHING
} EnemyMotion;

typedef struct {
    float     x, y;
    float     vx, vy;
    EnemyType type;
    bool      active;
    int       hp;
    EnemyMotion motion;
    float     explode_timer; /* > 0 = showing explosion */
} Enemy;

typedef struct {
    Enemy enemies[MAX_ENEMIES];
    int   count;            /* total slots used */
    float dir;              /* +1 or -1 horizontal */
    float move_timer;       /* controls step timing */
    float shoot_timer;      /* cooldown for group shot */
    float step_interval;    /* seconds per lateral step */
    float shoot_interval;
    float step_dx;          /* pixels per step */
    float step_dy;          /* pixels to drop on edge */
    bool  rush_enabled;
    float rush_timer;
} EnemyGrid;

void enemy_grid_init(EnemyGrid *g, EnemyType type, int cols, int rows,
                     float speed_mult, float shoot_interval, bool rush_enabled);
void enemy_grid_update(EnemyGrid *g, BulletPool *bp, const Player *player, float dt);
void enemy_grid_draw(const EnemyGrid *g, const Assets *a);

int  enemy_grid_alive_count(const EnemyGrid *g);

/* returns the texture for a given enemy type */
Texture2D enemy_texture(const Assets *a, EnemyType type);

#endif /* ENEMY_H */
