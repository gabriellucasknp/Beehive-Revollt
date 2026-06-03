#ifndef CAMPAIGN_H
#define CAMPAIGN_H

#include "types.h"
#include <stdbool.h>

#define CAMPAIGN_PHASE_COUNT 5
#define HIVE_OPTION_COUNT 3
#define ARCHIVE_HAZARD_MAX_ACTIVE 3

typedef enum {
    PHASE_STYLE_FORMATION,
    PHASE_STYLE_ASSAULT,
    PHASE_STYLE_HIVE,
    PHASE_STYLE_ARCHIVE,
    PHASE_STYLE_BOSS
} CampaignPhaseStyle;

typedef enum {
    HIVE_ROOM_WORLD,
    HIVE_ROOM_OVERLAY
} HiveRoomMode;

typedef struct {
    int number;
    const char *name;
    CampaignPhaseStyle style;
    EnemyType enemy_type;
    int cols;
    int rows;
    float speed_mult;
    float shoot_interval;
    int boss_hp;
    bool rush_enabled;
    int background_scene;
} CampaignPhase;

typedef struct {
    HiveRoomMode mode;
    const char *title;
    const char *prompt;
    const char *lore;
    const char *options[HIVE_OPTION_COUNT];
    int correct_option;
} HiveRoomDef;

typedef struct {
    int room_index;
    int memories_found;
    bool memory_ready;
    bool finished;
} HiveTrialState;

typedef struct {
    const char *title;
    const char *prompt;
    const char *options[HIVE_OPTION_COUNT];
    int correct_option;
} PaperQuestionDef;

typedef struct {
    int current_index;
    int papers_collected;
    bool finished;
} PaperTrialState;

const CampaignPhase *campaign_phase_get(int phase_number);
int campaign_phase_count(void);
bool campaign_phase_has_power_choice(int phase_number);

void hive_trial_init(HiveTrialState *state);
const HiveRoomDef *hive_trial_current_room(const HiveTrialState *state);
const HiveRoomDef *hive_trial_room_at(int room_index);
bool hive_trial_submit_answer(HiveTrialState *state, int option);
bool hive_trial_collect_memory(HiveTrialState *state);

void paper_trial_init(PaperTrialState *state);
const PaperQuestionDef *paper_trial_current_question(const PaperTrialState *state);
bool paper_trial_submit_answer(PaperTrialState *state, int option);

int archive_hazard_zone_count(void);
int archive_hazard_wave_size(void);
Rectangle archive_hazard_zone_rect(int zone_index);

#endif /* CAMPAIGN_H */
