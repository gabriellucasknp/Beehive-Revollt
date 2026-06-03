#ifndef STAGE_H
#define STAGE_H

#include "types.h"
#include "enemy.h"

typedef struct {
    int       number;
    EnemyType enemy_type;
    int       cols;
    int       rows;
    float     speed_mult;
    float     shoot_interval;
    int       boss_hp;        /* 0 = not a boss stage */
} StageConfig;

extern const StageConfig STAGES[8]; /* STAGES[1..7] */

void stage_load(EnemyGrid *g, int stage_number);

#endif /* STAGE_H */
