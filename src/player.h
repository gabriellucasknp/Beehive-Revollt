#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"
#include "assets.h"

typedef struct {
    float x, y;
    int   lives;
    int   powers[POWER_COUNT];
    float shoot_timer;      /* countdown to next allowed shot */
    float thrust_anim;      /* alternates sprite frame */
    bool  invincible;       /* brief window after taking damage */
    float invincible_timer;
} Player;

void player_init(Player *p);
void player_update(Player *p, float dt);
void player_draw(const Player *p, const Assets *a);
void player_take_damage(Player *p);

float player_shoot_cooldown(const Player *p);
float player_speed(const Player *p);

#endif /* PLAYER_H */
