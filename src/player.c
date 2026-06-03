#include "player.h"
#include "types.h"
#include "raylib.h"

#define INVINCIBLE_DURATION 1.2f
#define THRUST_ANIM_SPEED   0.1f

void player_init(Player *p) {
    p->x = SCREEN_W / 2.0f;
    p->y = SCREEN_H - SPR_SIZE_16 * SPRITE_SCALE - 12.0f;
    p->lives = 3;
    for (int i = 0; i < POWER_COUNT; i++) p->powers[i] = 0;
    p->shoot_timer      = 0.0f;
    p->thrust_anim      = 0.0f;
    p->invincible       = false;
    p->invincible_timer = 0.0f;
}

float player_shoot_cooldown(const Player *p) {
    float base = SHOOT_COOLDOWN;
    int lvl = p->powers[POWER_FAST_SHOT];
    return base / (1.0f + 0.25f * lvl);
}

float player_speed(const Player *p) {
    int lvl = p->powers[POWER_SPEED];
    return PLAYER_SPEED + 60.0f * lvl;
}

void player_update(Player *p, float dt) {
    float spd = player_speed(p);

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  p->x -= spd * dt;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) p->x += spd * dt;

    float half = (SPR_SIZE_16 * SPRITE_SCALE) / 2.0f;
    if (p->x < half)            p->x = half;
    if (p->x > SCREEN_W - half) p->x = SCREEN_W - half;

    if (p->shoot_timer > 0.0f) p->shoot_timer -= dt;

    p->thrust_anim += dt;

    if (p->invincible) {
        p->invincible_timer -= dt;
        if (p->invincible_timer <= 0.0f) p->invincible = false;
    }
}

void player_draw(const Player *p, const Assets *a) {
    /* blink when invincible */
    if (p->invincible && (int)(p->invincible_timer * 10) % 2 == 0) return;

    bool thrusting = IsKeyDown(KEY_A) || IsKeyDown(KEY_D) ||
                     IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT);
    Texture2D tex = thrusting ? a->player_thrust : a->player_bee;

    float w = SPR_SIZE_16 * SPRITE_SCALE;
    float h = SPR_SIZE_16 * SPRITE_SCALE;
    DrawTextureEx(tex, (Vector2){p->x - w / 2.0f, p->y - h / 2.0f},
                  0.0f, SPRITE_SCALE, WHITE);
}

void player_take_damage(Player *p) {
    if (p->invincible) return;

    if (p->powers[POWER_SHIELD] > 0) {
        p->powers[POWER_SHIELD]--;
    } else {
        p->lives--;
    }

    p->invincible       = true;
    p->invincible_timer = INVINCIBLE_DURATION;
}
