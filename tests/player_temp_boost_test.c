#include "player.h"
#include <assert.h>
#include <stdio.h>

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

int main(void) {
    test_boost_expires_after_ten_seconds();
    test_temp_shield_absorbs_damage_and_breaks();
    test_other_boost_breaks_on_damage();
    puts("player_temp_boost_test: ok");
    return 0;
}
