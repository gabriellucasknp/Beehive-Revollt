#include "player.h"
#include <assert.h>
#include <stdio.h>

static bool key_down[512];

bool IsKeyDown(int key) {
    if (key < 0 || key >= (int)(sizeof(key_down) / sizeof(key_down[0]))) return false;
    return key_down[key];
}

void DrawCircleV(Vector2 center, float radius, Color color) {
    (void)center;
    (void)radius;
    (void)color;
}

void DrawCircleLines(int centerX, int centerY, float radius, Color color) {
    (void)centerX;
    (void)centerY;
    (void)radius;
    (void)color;
}

void DrawLineEx(Vector2 startPos, Vector2 endPos, float thick, Color color) {
    (void)startPos;
    (void)endPos;
    (void)thick;
    (void)color;
}

void DrawTextureEx(Texture2D texture, Vector2 position, float rotation, float scale, Color tint) {
    (void)texture;
    (void)position;
    (void)rotation;
    (void)scale;
    (void)tint;
}

static void clear_keys(void) {
    for (int i = 0; i < (int)(sizeof(key_down) / sizeof(key_down[0])); i++) key_down[i] = false;
}

static void test_boost_expires_after_ten_seconds(void) {
    Player p;
    player_init(&p);

    player_apply_temp_boost(&p, TEMP_BOOST_DOUBLE_SHOT, 10.0f);
    assert(p.temp_boost == TEMP_BOOST_DOUBLE_SHOT);

    player_update_effects(&p, 5.0f);
    assert(p.temp_boost == TEMP_BOOST_DOUBLE_SHOT);

    player_update_effects(&p, 5.1f);
    assert(p.temp_boost == TEMP_BOOST_NONE);
}

static void test_temp_shield_absorbs_damage_and_breaks(void) {
    Player p;
    player_init(&p);

    player_apply_temp_boost(&p, TEMP_BOOST_SHIELD, 10.0f);
    player_take_damage(&p);

    assert(p.lives == 3);
    assert(p.temp_boost == TEMP_BOOST_NONE);
}

static void test_other_boost_breaks_on_damage(void) {
    Player p;
    player_init(&p);

    player_apply_temp_boost(&p, TEMP_BOOST_SPEED, 10.0f);
    player_take_damage(&p);

    assert(p.lives == 2);
    assert(p.temp_boost == TEMP_BOOST_NONE);
}

static void test_free_movement_uses_y_axis_and_clamps_to_screen(void) {
    Player p;
    float initial_y;
    float half = (SPR_SIZE_16 * SPRITE_SCALE) / 2.0f;

    player_init(&p);
    p.y = SCREEN_H * 0.5f;
    initial_y = p.y;

    clear_keys();
    key_down[KEY_S] = true;
    player_update_free(&p, 0.25f);
    assert(p.y > initial_y);

    clear_keys();
    key_down[KEY_W] = true;
    player_update_free(&p, 10.0f);
    assert(p.y == half);

    clear_keys();
    key_down[KEY_DOWN] = true;
    player_update_free(&p, 10.0f);
    assert(p.y == SCREEN_H - half);
}

int main(void) {
    test_boost_expires_after_ten_seconds();
    test_temp_shield_absorbs_damage_and_breaks();
    test_other_boost_breaks_on_damage();
    test_free_movement_uses_y_axis_and_clamps_to_screen();
    puts("player_temp_boost_test: ok");
    return 0;
}
