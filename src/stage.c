#include "stage.h"

void stage_load(EnemyGrid *g, int phase_number) {
    const CampaignPhase *phase = campaign_phase_get(phase_number);

    if (!phase) {
        enemy_grid_init(g, ENEMY_DRONE, 0, 0, 1.0f, 1.0f, false);
        return;
    }

    enemy_grid_init(g,
                    phase->enemy_type,
                    phase->cols,
                    phase->rows,
                    phase->speed_mult,
                    phase->shoot_interval,
                    phase->rush_enabled);

    if (phase->boss_hp > 0 && g->count == 1) {
        g->enemies[0].hp = phase->boss_hp;
    }
}
