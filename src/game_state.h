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
    QuickQuiz  paper_quiz;
    HiveTrialState hive_trial;
    PaperTrialState paper_trial;
    Assets     assets;
    char       status_text[64];
    float      status_timer;
    float      hive_hazard_timer;
    float      archive_honey_timer;
    float      archive_honey_warning;
    float      archive_honey_active;
    float      archive_paper_x;
    float      archive_paper_y;
    int        archive_honey_zones[ARCHIVE_HAZARD_MAX_ACTIVE];
    int        archive_honey_zone_count;
    bool       archive_paper_active;
    int        reveal_room_index;
} Game;

void game_init(Game *g);

/* One call per frame — wraps BeginDrawing/EndDrawing internally. */
void game_frame(Game *g, float dt);

#endif /* GAME_STATE_H */
