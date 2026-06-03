#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"
#include "assets.h"

typedef struct {
    float x, y;
    int   lives;
    int   powers[POWER_COUNT];
    TempBoostType temp_boost;
    float temp_boost_timer;
    float shoot_timer;      /* countdown to next allowed shot */
    float thrust_anim;      /* alternates sprite frame */
    bool  invincible;       /* brief window after taking damage */
    float invincible_timer;
    float shield_break_timer;
} Player;

void player_init(Player *p);
void player_update(Player *p, float dt);
void player_update_effects(Player *p, float dt);
void player_draw(const Player *p, const Assets *a);
void player_take_damage(Player *p);
void player_apply_temp_boost(Player *p, TempBoostType boost, float duration);
void player_clear_temp_boost(Player *p);
const char *player_temp_boost_name(TempBoostType boost);

float player_shoot_cooldown(const Player *p);
float player_speed(const Player *p);

#endif /* PLAYER_H */
