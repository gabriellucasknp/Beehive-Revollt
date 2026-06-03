#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"
#include <stdbool.h>

#define SCREEN_W      640
#define SCREEN_H      480
#define SPRITE_SCALE  4.0f
#define TARGET_FPS    60

/* --- Sprites nativos (16x16 ou 8x8) --- */
#define SPR_SIZE_16   16
#define SPR_SIZE_8    8

/* --- Object pool limits --- */
#define MAX_PLAYER_BULLETS  32
#define MAX_ENEMY_BULLETS   32
#define MAX_ENEMIES         30

/* --- Player base stats --- */
#define PLAYER_SPEED        220.0f
#define BULLET_SPEED        400.0f
#define ENEMY_BULLET_SPEED  180.0f
#define SHOOT_COOLDOWN      0.25f

/* --- Game states --- */
typedef enum {
    STATE_MENU,
    STATE_NARRATIVE,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_QUIZ,
    STATE_PICKUP_QUIZ,
    STATE_MEMORY_QUIZ,
    STATE_MEMORY_REVEAL,
    STATE_POWER_CHOICE,
    STATE_GAME_OVER,
    STATE_VICTORY
} GameState;

/* --- Enemy types (maps to sprite) --- */
typedef enum {
    ENEMY_DRONE,
    ENEMY_WASP,
    ENEMY_STINGER,
    ENEMY_QUEEN_MINI,
    ENEMY_BOSS
} EnemyType;

/* --- Power-up indices --- */
typedef enum {
    POWER_FAST_SHOT = 0,
    POWER_SHIELD    = 1,
    POWER_SPEED     = 2,
    POWER_COUNT     = 3
} PowerType;

typedef enum {
    TEMP_BOOST_NONE = -1,
    TEMP_BOOST_DOUBLE_SHOT = 0,
    TEMP_BOOST_RAPID_SHOT  = 1,
    TEMP_BOOST_SPEED       = 2,
    TEMP_BOOST_SHIELD      = 3,
    TEMP_BOOST_COUNT       = 4
} TempBoostType;

#endif /* TYPES_H */
