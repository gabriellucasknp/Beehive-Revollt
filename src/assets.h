#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"

typedef struct {
    /* sprites */
    Texture2D player_bee;
    Texture2D player_thrust;
    Texture2D drone_enemy;
    Texture2D wasp_enemy;
    Texture2D stinger_jet;
    Texture2D queen_mini;
    Texture2D queen_boss;
    Texture2D honey_shot;
    Texture2D venom;
    Texture2D pollen;
    Texture2D explosion_1;
    Texture2D explosion_2;
    Texture2D heart;
    Texture2D hex_tile;
    Texture2D memory_shard;
    /* scenes */
    Texture2D stages[8];     /* stages[1..7] */
    Texture2D cutscene[4];   /* cutscene[1..3] */
} Assets;

void assets_load(Assets *a);
void assets_unload(Assets *a);

#endif /* ASSETS_H */
