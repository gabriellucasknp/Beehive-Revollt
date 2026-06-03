#include "enemy.h"
#include "raylib.h"
#include <stdlib.h>

#define EXPLOSION_DURATION 0.35f
#define FORMATION_START_Y  60.0f
#define FORMATION_PAD_X    (SPR_SIZE_16 * SPRITE_SCALE + 6.0f)
#define FORMATION_PAD_Y    (SPR_SIZE_16 * SPRITE_SCALE + 6.0f)

Texture2D enemy_texture(const Assets *a, EnemyType type) {
    switch (type) {
        case ENEMY_DRONE:      return a->drone_enemy;
        case ENEMY_WASP:       return a->wasp_enemy;
        case ENEMY_STINGER:    return a->stinger_jet;
        case ENEMY_QUEEN_MINI: return a->queen_mini;
        default:               return a->drone_enemy;
    }
}

void enemy_grid_init(EnemyGrid *g, EnemyType type, int cols, int rows,
                     float speed_mult, float shoot_interval) {
    g->count          = 0;
    g->dir            = 1.0f;
    g->move_timer     = 0.0f;
    g->shoot_timer    = shoot_interval * 0.5f;
    g->step_interval  = 0.6f / speed_mult;
    g->shoot_interval = shoot_interval;
    g->step_dx        = 18.0f;
    g->step_dy        = SPR_SIZE_16 * SPRITE_SCALE * 0.5f;

    float total_w = cols * FORMATION_PAD_X - 6.0f;
    float start_x = (SCREEN_W - total_w) / 2.0f + SPR_SIZE_16 * SPRITE_SCALE / 2.0f;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (g->count >= MAX_ENEMIES) break;
            Enemy *e = &g->enemies[g->count++];
            e->x            = start_x + c * FORMATION_PAD_X;
            e->y            = FORMATION_START_Y + r * FORMATION_PAD_Y;
            e->type         = type;
            e->active       = true;
            e->hp           = 1;
            e->explode_timer = 0.0f;
        }
    }
}

static bool any_at_edge(const EnemyGrid *g) {
    float half = SPR_SIZE_16 * SPRITE_SCALE / 2.0f;
    for (int i = 0; i < g->count; i++) {
        if (!g->enemies[i].active) continue;
        float nx = g->enemies[i].x + g->dir * g->step_dx;
        if (nx - half < 0 || nx + half > SCREEN_W) return true;
    }
    return false;
}

void enemy_grid_update(EnemyGrid *g, BulletPool *bp, float dt) {
    /* explosion timers */
    for (int i = 0; i < g->count; i++) {
        if (g->enemies[i].explode_timer > 0.0f)
            g->enemies[i].explode_timer -= dt;
    }

    /* lateral movement */
    g->move_timer += dt;
    if (g->move_timer >= g->step_interval) {
        g->move_timer = 0.0f;

        if (any_at_edge(g)) {
            /* drop down and reverse */
            for (int i = 0; i < g->count; i++) {
                if (!g->enemies[i].active) continue;
                g->enemies[i].y += g->step_dy;
            }
            g->dir = -g->dir;
        } else {
            for (int i = 0; i < g->count; i++) {
                if (!g->enemies[i].active) continue;
                g->enemies[i].x += g->dir * g->step_dx;
            }
        }
    }

    /* enemy shooting */
    g->shoot_timer -= dt;
    if (g->shoot_timer <= 0.0f) {
        g->shoot_timer = g->shoot_interval;

        /* pick a random active enemy */
        int alive[MAX_ENEMIES], n = 0;
        for (int i = 0; i < g->count; i++)
            if (g->enemies[i].active) alive[n++] = i;

        if (n > 0) {
            int idx = alive[GetRandomValue(0, n - 1)];
            bullet_spawn_enemy(bp, g->enemies[idx].x, g->enemies[idx].y);
        }
    }
}

void enemy_grid_draw(const EnemyGrid *g, const Assets *a) {
    float w = SPR_SIZE_16 * SPRITE_SCALE;
    float h = SPR_SIZE_16 * SPRITE_SCALE;

    for (int i = 0; i < g->count; i++) {
        const Enemy *e = &g->enemies[i];
        if (!e->active && e->explode_timer <= 0.0f) continue;

        if (e->explode_timer > 0.0f) {
            Texture2D ex = (e->explode_timer > EXPLOSION_DURATION / 2.0f)
                           ? a->explosion_1 : a->explosion_2;
            DrawTextureEx(ex,
                (Vector2){e->x - w / 2.0f, e->y - h / 2.0f},
                0.0f, SPRITE_SCALE, WHITE);
        } else {
            Texture2D tex = enemy_texture(a, e->type);
            DrawTextureEx(tex,
                (Vector2){e->x - w / 2.0f, e->y - h / 2.0f},
                0.0f, SPRITE_SCALE, WHITE);
        }
    }
}

int enemy_grid_alive_count(const EnemyGrid *g) {
    int n = 0;
    for (int i = 0; i < g->count; i++)
        if (g->enemies[i].active) n++;
    return n;
}
