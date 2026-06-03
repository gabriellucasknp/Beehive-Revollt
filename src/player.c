#include "player.h"
#include "types.h"
#include "raylib.h"
#include <math.h>

#define INVINCIBLE_DURATION 1.2f
#define THRUST_ANIM_SPEED   0.1f
#define SHIELD_BREAK_DURATION 0.35f

const char *player_temp_boost_name(TempBoostType boost) {
    switch (boost) {
        case TEMP_BOOST_DOUBLE_SHOT: return "TIRO DUPLO";
        case TEMP_BOOST_RAPID_SHOT:  return "TIRO RAPIDO";
        case TEMP_BOOST_SPEED:       return "HYPER VEL";
        case TEMP_BOOST_SHIELD:      return "ESCUDO";
        default:                     return "SEM BOOST";
    }
}

void player_clear_temp_boost(Player *p) {
    p->temp_boost = TEMP_BOOST_NONE;
    p->temp_boost_timer = 0.0f;
}

void player_apply_temp_boost(Player *p, TempBoostType boost, float duration) {
    p->temp_boost = boost;
    p->temp_boost_timer = duration;
}

void player_init(Player *p) {
    p->x = SCREEN_W / 2.0f;
    p->y = SCREEN_H - SPR_SIZE_16 * SPRITE_SCALE - 12.0f;
    p->lives = 3;
    for (int i = 0; i < POWER_COUNT; i++) p->powers[i] = 0;
    player_clear_temp_boost(p);
    p->shoot_timer      = 0.0f;
    p->thrust_anim      = 0.0f;
    p->invincible       = false;
    p->invincible_timer = 0.0f;
    p->shield_break_timer = 0.0f;
}

float player_shoot_cooldown(const Player *p) {
    float base = SHOOT_COOLDOWN;
    int lvl = p->powers[POWER_FAST_SHOT];
    float cooldown = base / (1.0f + 0.25f * lvl);
    if (p->temp_boost == TEMP_BOOST_RAPID_SHOT) cooldown *= 0.45f;
    return cooldown;
}

float player_speed(const Player *p) {
    int lvl = p->powers[POWER_SPEED];
    float speed = PLAYER_SPEED + 60.0f * lvl;
    if (p->temp_boost == TEMP_BOOST_SPEED) speed += 140.0f;
    return speed;
}

void player_update_effects(Player *p, float dt) {
    if (p->shoot_timer > 0.0f) p->shoot_timer -= dt;

    if (p->invincible) {
        p->invincible_timer -= dt;
        if (p->invincible_timer <= 0.0f) p->invincible = false;
    }

    if (p->temp_boost != TEMP_BOOST_NONE) {
        p->temp_boost_timer -= dt;
        if (p->temp_boost_timer <= 0.0f) player_clear_temp_boost(p);
    }

    if (p->shield_break_timer > 0.0f) p->shield_break_timer -= dt;
}

static void draw_temp_shield(const Player *p) {
    float pulse = 1.5f + 1.5f * sinf(p->thrust_anim * 10.0f);
    float radius = 30.0f + pulse;
    DrawCircleV((Vector2){p->x, p->y}, radius + 4.0f, (Color){80, 220, 255, 40});
    DrawCircleLines((int)p->x, (int)p->y, radius, (Color){130, 240, 255, 255});
    DrawCircleLines((int)p->x, (int)p->y, radius - 5.0f, (Color){130, 240, 255, 180});
}

static void draw_shield_break(const Player *p) {
    if (p->shield_break_timer <= 0.0f) return;

    float t = p->shield_break_timer / SHIELD_BREAK_DURATION;
    float inner = 18.0f + (1.0f - t) * 20.0f;
    for (int i = 0; i < 6; i++) {
        float angle = (float)i * 60.0f * DEG2RAD;
        Vector2 dir = {cosf(angle), sinf(angle)};
        Vector2 a = {p->x + dir.x * inner, p->y + dir.y * inner};
        Vector2 b = {p->x + dir.x * (inner + 10.0f), p->y + dir.y * (inner + 10.0f)};
        DrawLineEx(a, b, 3.0f, (Color){180, 245, 255, (unsigned char)(200 * t)});
    }
}

void player_update(Player *p, float dt) {
    float spd = player_speed(p);

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  p->x -= spd * dt;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) p->x += spd * dt;

    float half = (SPR_SIZE_16 * SPRITE_SCALE) / 2.0f;
    if (p->x < half)            p->x = half;
    if (p->x > SCREEN_W - half) p->x = SCREEN_W - half;

    p->thrust_anim += dt;
    player_update_effects(p, dt);
}

void player_draw(const Player *p, const Assets *a) {
    if (p->temp_boost == TEMP_BOOST_SHIELD) draw_temp_shield(p);
    draw_shield_break(p);

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

    if (p->temp_boost == TEMP_BOOST_SHIELD) {
        player_clear_temp_boost(p);
        p->shield_break_timer = SHIELD_BREAK_DURATION;
    } else {
        if (p->temp_boost != TEMP_BOOST_NONE) player_clear_temp_boost(p);
    }

    if (p->shield_break_timer <= 0.0f && p->powers[POWER_SHIELD] > 0) {
        p->powers[POWER_SHIELD]--;
    } else if (p->shield_break_timer <= 0.0f) {
        p->lives--;
    }

    p->invincible       = true;
    p->invincible_timer = INVINCIBLE_DURATION;
}
