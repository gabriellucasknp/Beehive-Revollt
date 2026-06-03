#include "bullet.h"
#include "raylib.h"

void bullets_init(BulletPool *bp) {
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) bp->player[i].active = false;
    for (int i = 0; i < MAX_ENEMY_BULLETS;  i++) bp->enemy[i].active  = false;
}

static void update_pool(Bullet *pool, int count, float dt) {
    for (int i = 0; i < count; i++) {
        if (!pool[i].active) continue;
        pool[i].y += pool[i].vy * dt;
        if (pool[i].y < -16.0f || pool[i].y > SCREEN_H + 16.0f)
            pool[i].active = false;
    }
}

void bullets_update(BulletPool *bp, float dt) {
    update_pool(bp->player, MAX_PLAYER_BULLETS, dt);
    update_pool(bp->enemy,  MAX_ENEMY_BULLETS,  dt);
}

static void draw_pool(const Bullet *pool, int count, const Texture2D *tex) {
    float w = SPR_SIZE_8 * SPRITE_SCALE;
    float h = SPR_SIZE_8 * SPRITE_SCALE;
    for (int i = 0; i < count; i++) {
        if (!pool[i].active) continue;
        DrawTextureEx(*tex,
            (Vector2){pool[i].x - w / 2.0f, pool[i].y - h / 2.0f},
            0.0f, SPRITE_SCALE, WHITE);
    }
}

void bullets_draw(const BulletPool *bp, const Assets *a) {
    draw_pool(bp->player, MAX_PLAYER_BULLETS, &a->honey_shot);
    draw_pool(bp->enemy,  MAX_ENEMY_BULLETS,  &a->venom);
}

bool bullet_spawn_player(BulletPool *bp, float x, float y) {
    for (int i = 0; i < MAX_PLAYER_BULLETS; i++) {
        if (!bp->player[i].active) {
            bp->player[i] = (Bullet){x, y, -BULLET_SPEED, true};
            return true;
        }
    }
    return false;
}

bool bullet_spawn_enemy(BulletPool *bp, float x, float y) {
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (!bp->enemy[i].active) {
            bp->enemy[i] = (Bullet){x, y, ENEMY_BULLET_SPEED, true};
            return true;
        }
    }
    return false;
}
