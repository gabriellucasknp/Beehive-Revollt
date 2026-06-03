#include "collision.h"
#include "raylib.h"

#define EXPLOSION_DURATION 0.35f

static Rectangle rect_from(float cx, float cy, int native_px) {
    float sz = native_px * SPRITE_SCALE;
    return (Rectangle){cx - sz / 2.0f, cy - sz / 2.0f, sz, sz};
}

int collision_update(Player *p, EnemyGrid *g, BulletPool *bp) {
    int score = 0;
    Rectangle prec = rect_from(p->x, p->y, SPR_SIZE_16);

    /* player bullets vs enemies */
    for (int b = 0; b < MAX_PLAYER_BULLETS; b++) {
        if (!bp->player[b].active) continue;
        Rectangle brec = rect_from(bp->player[b].x, bp->player[b].y, SPR_SIZE_8);

        for (int e = 0; e < g->count; e++) {
            if (!g->enemies[e].active) continue;
            Rectangle erec = rect_from(g->enemies[e].x, g->enemies[e].y, SPR_SIZE_16);

            if (CheckCollisionRecs(brec, erec)) {
                bp->player[b].active = false;
                g->enemies[e].hp--;
                if (g->enemies[e].hp <= 0) {
                    g->enemies[e].active       = false;
                    g->enemies[e].explode_timer = EXPLOSION_DURATION;
                    score += 100;
                }
                break;
            }
        }
    }

    /* enemy bullets vs player */
    for (int b = 0; b < MAX_ENEMY_BULLETS; b++) {
        if (!bp->enemy[b].active) continue;
        Rectangle brec = rect_from(bp->enemy[b].x, bp->enemy[b].y, SPR_SIZE_8);

        if (CheckCollisionRecs(brec, prec)) {
            bp->enemy[b].active = false;
            player_take_damage(p);
        }
    }

    /* enemy body vs player */
    for (int e = 0; e < g->count; e++) {
        if (!g->enemies[e].active) continue;
        Rectangle erec = rect_from(g->enemies[e].x, g->enemies[e].y, SPR_SIZE_16);

        if (CheckCollisionRecs(erec, prec)) {
            g->enemies[e].active       = false;
            g->enemies[e].explode_timer = EXPLOSION_DURATION;
            player_take_damage(p);
            score += 50;
        }
    }

    return score;
}
