#ifndef PICKUP_H
#define PICKUP_H

#include "assets.h"
#include "types.h"
#include <stdbool.h>

typedef struct {
    float x, y;
    float vy;
    float pulse;
    float spawn_timer;
    bool  active;
} PickupDrop;

void pickup_init(PickupDrop *p);
void pickup_update(PickupDrop *p, float dt);
void pickup_draw(const PickupDrop *p, const Assets *a);
bool pickup_try_collect(PickupDrop *p, float px, float py);

#endif /* PICKUP_H */
