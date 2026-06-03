#include "assets.h"
#include "raylib.h"
#include <stdio.h>

static Texture2D load_spr(const char *path) {
    Texture2D t = LoadTexture(path);
    SetTextureFilter(t, TEXTURE_FILTER_POINT);
    return t;
}

void assets_load(Assets *a) {
    a->player_bee   = load_spr("assets/sprites/player_bee.png");
    a->player_thrust= load_spr("assets/sprites/player_thrust.png");
    a->drone_enemy  = load_spr("assets/sprites/drone_enemy.png");
    a->wasp_enemy   = load_spr("assets/sprites/wasp_enemy.png");
    a->stinger_jet  = load_spr("assets/sprites/stinger_jet.png");
    a->queen_mini   = load_spr("assets/sprites/queen_mini.png");
    a->queen_boss   = load_spr("assets/sprites/queen_boss.png");
    a->honey_shot   = load_spr("assets/sprites/honey_shot.png");
    a->venom        = load_spr("assets/sprites/venom.png");
    a->pollen       = load_spr("assets/sprites/pollen.png");
    a->explosion_1  = load_spr("assets/sprites/explosion_1.png");
    a->explosion_2  = load_spr("assets/sprites/explosion_2.png");
    a->heart        = load_spr("assets/sprites/heart.png");
    a->hex_tile     = load_spr("assets/sprites/hex_tile.png");

    char buf[128];
    for (int i = 1; i <= 7; i++) {
        snprintf(buf, sizeof(buf), "assets/scenes/stage_%d.png", i);
        a->stages[i] = load_spr(buf);
    }
    const char *cuts[] = {
        "",
        "assets/scenes/cutscene_1_oppression.png",
        "assets/scenes/cutscene_2_revolt.png",
        "assets/scenes/cutscene_3_victory.png"
    };
    for (int i = 1; i <= 3; i++) {
        a->cutscene[i] = load_spr(cuts[i]);
    }
}

void assets_unload(Assets *a) {
    UnloadTexture(a->player_bee);
    UnloadTexture(a->player_thrust);
    UnloadTexture(a->drone_enemy);
    UnloadTexture(a->wasp_enemy);
    UnloadTexture(a->stinger_jet);
    UnloadTexture(a->queen_mini);
    UnloadTexture(a->queen_boss);
    UnloadTexture(a->honey_shot);
    UnloadTexture(a->venom);
    UnloadTexture(a->pollen);
    UnloadTexture(a->explosion_1);
    UnloadTexture(a->explosion_2);
    UnloadTexture(a->heart);
    UnloadTexture(a->hex_tile);
    for (int i = 1; i <= 7; i++) UnloadTexture(a->stages[i]);
    for (int i = 1; i <= 3; i++) UnloadTexture(a->cutscene[i]);
}
