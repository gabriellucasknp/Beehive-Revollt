#include "stage.h"

const StageConfig STAGES[8] = {
    {0}, /* unused index 0 */
    {1, ENEMY_DRONE,      5, 2, 1.0f, 2.5f, 0},
    {2, ENEMY_DRONE,      5, 3, 1.2f, 2.2f, 0},
    {3, ENEMY_WASP,       6, 2, 1.3f, 2.0f, 0},
    {4, ENEMY_WASP,       6, 3, 1.5f, 1.8f, 0},
    {5, ENEMY_STINGER,    5, 3, 1.7f, 1.5f, 0},
    {6, ENEMY_QUEEN_MINI, 4, 3, 2.0f, 1.2f, 0},
    {7, ENEMY_BOSS,       1, 1, 1.0f, 1.0f, 30},
};

void stage_load(EnemyGrid *g, int n) {
    const StageConfig *cfg = &STAGES[n];
    enemy_grid_init(g, cfg->enemy_type, cfg->cols, cfg->rows,
                    cfg->speed_mult, cfg->shoot_interval);
    /* boss gets extra HP */
    if (cfg->boss_hp > 0 && g->count == 1) {
        g->enemies[0].hp = cfg->boss_hp;
    }
}
