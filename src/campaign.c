#include "campaign.h"

static const CampaignPhase CAMPAIGN_PHASES[CAMPAIGN_PHASE_COUNT + 1] = {
    {0},
    {1, "Campo Aberto", PHASE_STYLE_FORMATION, ENEMY_DRONE, 5, 2, 1.0f, 2.4f, 0, false, 1},
    {2, "Vespa de Choque", PHASE_STYLE_ASSAULT, ENEMY_STINGER, 5, 3, 1.4f, 1.8f, 0, true, 5},
    {3, "Memorias da Colmeia", PHASE_STYLE_HIVE, ENEMY_DRONE, 0, 0, 1.0f, 0.0f, 0, false, 6},
    {4, "Arquivo do Mel", PHASE_STYLE_ARCHIVE, ENEMY_DRONE, 0, 0, 1.0f, 0.0f, 0, false, 4},
    {5, "Trono da Rainha", PHASE_STYLE_BOSS, ENEMY_BOSS, 1, 1, 1.1f, 1.0f, 36, false, 7},
};

static const HiveRoomDef HIVE_ROOMS[] = {
    {
        HIVE_ROOM_WORLD,
        "Sala dos Ponteiros",
        "Qual simbolo segue o endereco certo?",
        "Memoria 1: a rainha reescreveu trilhas antigas\npara apagar quem ensinou as operarias.",
        {"p->nectar", "&nectar", "*nectar"},
        0
    },
    {
        HIVE_ROOM_OVERLAY,
        "Sala dos Lacos",
        "Qual loop visita tres favos sem sair cedo demais?",
        "Memoria 2: as guardas aprendiam repeticao\ncomo disciplina, nao como liberdade.",
        {"for (i = 0; i < 3; i++)", "for (i = 1; i <= 3; i--)", "while (i > 3)"},
        0
    },
    {
        HIVE_ROOM_WORLD,
        "Sala dos Desvios",
        "Qual condicao abre a rota das memorias?",
        "Memoria 3: a revolta nasceu quando alguem\nperguntou quem decidia as regras do enxame.",
        {"if (truth_found)", "if (truth_found = 1)", "switch truth_found && 1"},
        0
    }
};

static const PaperQuestionDef PAPER_QUESTIONS[] = {
    {
        "Papel 1",
        "Qual declaracao cria um ponteiro para int?",
        {"int *p;", "int p*;", "pointer int p;"},
        0
    },
    {
        "Papel 2",
        "Qual operador acessa campo via ponteiro?",
        {".", "->", "::"},
        1
    },
    {
        "Papel 3",
        "Qual loop para quando a condicao fica falsa?",
        {"while", "struct", "typedef"},
        0
    },
    {
        "Papel 4",
        "Qual indice acessa o primeiro item do array?",
        {"1", "0", "-1"},
        1
    },
    {
        "Papel 5",
        "Qual palavra retorna um valor de funcao?",
        {"break", "goto", "return"},
        2
    }
};

static const Rectangle ARCHIVE_HAZARD_ZONES[] = {
    {36.0f, 104.0f, 118.0f, 92.0f},
    {196.0f, 86.0f, 124.0f, 92.0f},
    {360.0f, 110.0f, 120.0f, 88.0f},
    {82.0f, 246.0f, 122.0f, 92.0f},
    {256.0f, 222.0f, 130.0f, 96.0f},
    {432.0f, 264.0f, 112.0f, 90.0f}
};

const CampaignPhase *campaign_phase_get(int phase_number) {
    if (phase_number < 1 || phase_number > CAMPAIGN_PHASE_COUNT) return 0;
    return &CAMPAIGN_PHASES[phase_number];
}

int campaign_phase_count(void) {
    return CAMPAIGN_PHASE_COUNT;
}

bool campaign_phase_has_power_choice(int phase_number) {
    return phase_number >= 1 && phase_number <= 2;
}

void hive_trial_init(HiveTrialState *state) {
    state->room_index = 0;
    state->memories_found = 0;
    state->memory_ready = false;
    state->finished = false;
}

const HiveRoomDef *hive_trial_current_room(const HiveTrialState *state) {
    if (state->finished) return 0;
    return hive_trial_room_at(state->room_index);
}

const HiveRoomDef *hive_trial_room_at(int room_index) {
    if (room_index < 0 || room_index >= (int)(sizeof(HIVE_ROOMS) / sizeof(HIVE_ROOMS[0]))) {
        return 0;
    }
    return &HIVE_ROOMS[room_index];
}

bool hive_trial_submit_answer(HiveTrialState *state, int option) {
    const HiveRoomDef *room = hive_trial_current_room(state);
    if (!room || state->memory_ready) return false;

    if (option == room->correct_option) {
        state->memory_ready = true;
        return true;
    }

    return false;
}

bool hive_trial_collect_memory(HiveTrialState *state) {
    if (!state->memory_ready || state->finished) return false;

    state->memory_ready = false;
    state->memories_found++;
    state->room_index++;

    if (state->memories_found >= (int)(sizeof(HIVE_ROOMS) / sizeof(HIVE_ROOMS[0]))) {
        state->finished = true;
        return true;
    }

    return false;
}

void paper_trial_init(PaperTrialState *state) {
    state->current_index = 0;
    state->papers_collected = 0;
    state->finished = false;
}

const PaperQuestionDef *paper_trial_current_question(const PaperTrialState *state) {
    if (state->finished) return 0;
    if (state->current_index < 0 || state->current_index >= (int)(sizeof(PAPER_QUESTIONS) / sizeof(PAPER_QUESTIONS[0]))) {
        return 0;
    }
    return &PAPER_QUESTIONS[state->current_index];
}

bool paper_trial_submit_answer(PaperTrialState *state, int option) {
    const PaperQuestionDef *question = paper_trial_current_question(state);
    if (!question) return false;

    if (option != question->correct_option) return false;

    state->papers_collected++;
    state->current_index++;
    if (state->papers_collected >= (int)(sizeof(PAPER_QUESTIONS) / sizeof(PAPER_QUESTIONS[0]))) {
        state->finished = true;
    }
    return true;
}

int archive_hazard_zone_count(void) {
    return (int)(sizeof(ARCHIVE_HAZARD_ZONES) / sizeof(ARCHIVE_HAZARD_ZONES[0]));
}

int archive_hazard_wave_size(void) {
    return 2;
}

Rectangle archive_hazard_zone_rect(int zone_index) {
    if (zone_index < 0 || zone_index >= archive_hazard_zone_count()) {
        return (Rectangle){0.0f, 0.0f, 0.0f, 0.0f};
    }
    return ARCHIVE_HAZARD_ZONES[zone_index];
}
