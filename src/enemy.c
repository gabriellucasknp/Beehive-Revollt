#include "enemy.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>

#define EXPLOSION_DURATION 0.35f
#define FORMATION_START_Y  60.0f
#define FORMATION_PAD_X    (SPR_SIZE_16 * SPRITE_SCALE + 6.0f)
#define FORMATION_PAD_Y    (SPR_SIZE_16 * SPRITE_SCALE + 6.0f)
#define RUSH_SPEED         235.0f

Texture2D enemy_texture(const Assets *a, EnemyType type) {
    switch (type) {
        case ENEMY_DRONE:      return a->drone_enemy;
        case ENEMY_WASP:       return a->wasp_enemy;
        case ENEMY_STINGER:    return a->stinger_jet;
        case ENEMY_QUEEN_MINI: return a->queen_mini;
        case ENEMY_BOSS:       return a->queen_boss;
        default:               return a->drone_enemy;
    }
}

void enemy_grid_init(EnemyGrid *g, EnemyType type, int cols, int rows,
                     float speed_mult, float shoot_interval, bool rush_enabled) {
    g->count          = 0;
    g->dir            = 1.0f;
    g->move_timer     = 0.0f;
    g->shoot_timer    = shoot_interval * 0.5f;
    g->step_interval  = 0.6f / speed_mult;
    g->shoot_interval = shoot_interval;
    g->step_dx        = 18.0f;
    g->step_dy        = SPR_SIZE_16 * SPRITE_SCALE * 0.5f;
    g->rush_enabled   = rush_enabled;
    g->rush_timer     = rush_enabled ? 1.2f : 0.0f;

    float total_w = cols * FORMATION_PAD_X - 6.0f;
    float start_x = (SCREEN_W - total_w) / 2.0f + SPR_SIZE_16 * SPRITE_SCALE / 2.0f;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (g->count >= MAX_ENEMIES) break;
            Enemy *e = &g->enemies[g->count++];
            e->x            = start_x + c * FORMATION_PAD_X;
            e->y            = FORMATION_START_Y + r * FORMATION_PAD_Y;
            e->vx           = 0.0f;
            e->vy           = 0.0f;
            e->type         = type;
            e->active       = true;
            e->hp           = 1;
            e->motion       = ENEMY_FORMATION;
            e->explode_timer = 0.0f;
        }
    }
}

static bool any_at_edge(const EnemyGrid *g) {
    float half = SPR_SIZE_16 * SPRITE_SCALE / 2.0f;
    for (int i = 0; i < g->count; i++) {
        if (!g->enemies[i].active || g->enemies[i].motion != ENEMY_FORMATION) continue;
        float nx = g->enemies[i].x + g->dir * g->step_dx;
        if (nx - half < 0 || nx + half > SCREEN_W) return true;
    }
    return false;
}

static void launch_rusher(EnemyGrid *g, const Player *player) {
    int candidates[MAX_ENEMIES];
    int n = 0;

    for (int i = 0; i < g->count; i++) {
        if (!g->enemies[i].active || g->enemies[i].motion != ENEMY_FORMATION) continue;
        candidates[n++] = i;
    }

    if (n == 0) return;

    Enemy *e = &g->enemies[candidates[GetRandomValue(0, n - 1)]];
    float dx = player->x - e->x;
    float dy = player->y - e->y;
    float len = sqrtf(dx * dx + dy * dy);
    if (len <= 0.01f) len = 1.0f;

    e->motion = ENEMY_RUSHING;
    e->vx = (dx / len) * RUSH_SPEED;
    e->vy = (dy / len) * RUSH_SPEED;
}

void enemy_grid_update(EnemyGrid *g, BulletPool *bp, const Player *player, float dt) {
    /* explosion timers */
    for (int i = 0; i < g->count; i++) {
        if (g->enemies[i].explode_timer > 0.0f)
            g->enemies[i].explode_timer -= dt;
    }

    for (int i = 0; i < g->count; i++) {
        Enemy *e = &g->enemies[i];
        if (!e->active || e->motion != ENEMY_RUSHING) continue;
        e->x += e->vx * dt;
        e->y += e->vy * dt;
        if (e->x < -40.0f || e->x > SCREEN_W + 40.0f ||
            e->y < -40.0f || e->y > SCREEN_H + 40.0f) {
            e->active = false;
        }
    }

    /* lateral movement */
    g->move_timer += dt;
    if (g->move_timer >= g->step_interval) {
        g->move_timer = 0.0f;

        if (any_at_edge(g)) {
            /* drop down and reverse */
            for (int i = 0; i < g->count; i++) {
                if (!g->enemies[i].active || g->enemies[i].motion != ENEMY_FORMATION) continue;
                g->enemies[i].y += g->step_dy;
            }
            g->dir = -g->dir;
        } else {
            for (int i = 0; i < g->count; i++) {
                if (!g->enemies[i].active || g->enemies[i].motion != ENEMY_FORMATION) continue;
                g->enemies[i].x += g->dir * g->step_dx;
            }
        }
    }

    if (g->rush_enabled) {
        g->rush_timer -= dt;
        if (g->rush_timer <= 0.0f) {
            launch_rusher(g, player);
            g->rush_timer = 1.35f;
        }
    }

    /* enemy shooting */
    g->shoot_timer -= dt;
    if (g->shoot_timer <= 0.0f) {
        g->shoot_timer = g->shoot_interval;

        /* pick a random active enemy */
        int alive[MAX_ENEMIES], n = 0;
        for (int i = 0; i < g->count; i++)
            if (g->enemies[i].active && g->enemies[i].motion == ENEMY_FORMATION) alive[n++] = i;

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
