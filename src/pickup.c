#include "pickup.h"
#include "raylib.h"
#include <math.h>

#define PICKUP_SPEED 110.0f
#define PICKUP_MARGIN 32

static float next_spawn_delay(void) {
    return (float)GetRandomValue(7, 13);
}

static void pickup_reset(PickupDrop *p) {
    p->active = false;
    p->pulse = 0.0f;
    p->spawn_timer = next_spawn_delay();
}

static void pickup_spawn(PickupDrop *p) {
    p->active = true;
    p->x = (float)GetRandomValue(PICKUP_MARGIN, SCREEN_W - PICKUP_MARGIN);
    p->y = -20.0f;
    p->vy = PICKUP_SPEED;
    p->pulse = 0.0f;
}

void pickup_init(PickupDrop *p) {
    p->x = 0.0f;
    p->y = 0.0f;
    p->vy = PICKUP_SPEED;
    p->pulse = 0.0f;
    p->active = false;
    p->spawn_timer = 5.0f;
}

void pickup_update(PickupDrop *p, float dt) {
    if (!p->active) {
        p->spawn_timer -= dt;
        if (p->spawn_timer <= 0.0f) pickup_spawn(p);
        return;
    }

    p->y += p->vy * dt;
    p->pulse += dt;

    if (p->y > SCREEN_H + 24.0f) pickup_reset(p);
}

void pickup_draw(const PickupDrop *p, const Assets *a) {
    if (!p->active) return;

    float scale = SPRITE_SCALE;
    DrawTextureEx(a->pollen,
        (Vector2){p->x - (SPR_SIZE_8 * scale) / 2.0f, p->y - (SPR_SIZE_8 * scale) / 2.0f},
        0.0f, scale, WHITE);
}

bool pickup_try_collect(PickupDrop *p, float px, float py) {
    if (!p->active) return false;

    Rectangle pickup = {p->x - 14.0f, p->y - 14.0f, 28.0f, 28.0f};
    Rectangle player = {
        px - (SPR_SIZE_16 * SPRITE_SCALE) / 2.0f,
        py - (SPR_SIZE_16 * SPRITE_SCALE) / 2.0f,
        SPR_SIZE_16 * SPRITE_SCALE,
        SPR_SIZE_16 * SPRITE_SCALE
    };

    if (!CheckCollisionRecs(pickup, player)) return false;

    pickup_reset(p);
    return true;
}
