#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "types.h"
#include "assets.h"
#include "player.h"
#include "bullet.h"
#include "enemy.h"
#include "stage.h"
#include "campaign.h"
#include "quiz.h"
#include "pickup.h"
#include "quick_quiz.h"

typedef struct {
    GameState state;
    int       current_stage;
    int       score;
    int       narrative_screen;
    float     bg_scroll;

    Player     player;
    BulletPool bullets;
    EnemyGrid  grid;
    Quiz       quiz;
    PickupDrop pickup;
    QuickQuiz  pickup_quiz;
    QuickQuiz  memory_quiz;
    HiveTrialState hive_trial;
    Assets     assets;
    char       status_text[64];
    float      status_timer;
    float      hive_hazard_timer;
    int        reveal_room_index;
} Game;

void game_init(Game *g);

/* One call per frame — wraps BeginDrawing/EndDrawing internally. */
void game_frame(Game *g, float dt);

#endif /* GAME_STATE_H */
