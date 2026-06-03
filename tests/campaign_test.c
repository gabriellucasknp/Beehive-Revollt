#include "campaign.h"
#include <assert.h>
#include <stdio.h>

static void test_second_phase_enables_kamikaze_pressure(void) {
    const CampaignPhase *phase_one = campaign_phase_get(1);
    const CampaignPhase *phase_two = campaign_phase_get(2);

    assert(campaign_phase_count() == 4);
    assert(phase_one != NULL);
    assert(phase_two != NULL);
    assert(!phase_one->rush_enabled);
    assert(phase_two->rush_enabled);
}

static void test_correct_answer_unlocks_memory_pickup(void) {
    HiveTrialState hive;
    hive_trial_init(&hive);

    const HiveRoomDef *room = hive_trial_current_room(&hive);
    int wrong_option = (room->correct_option + 1) % HIVE_OPTION_COUNT;

    assert(room != NULL);
    assert(room->mode == HIVE_ROOM_WORLD);
    assert(!hive_trial_submit_answer(&hive, wrong_option));
    assert(!hive.memory_ready);

    assert(hive_trial_submit_answer(&hive, room->correct_option));
    assert(hive.memory_ready);
    assert(hive.memories_found == 0);
}

static void test_third_memory_finishes_the_hive_phase(void) {
    HiveTrialState hive;
    hive_trial_init(&hive);

    for (int room_idx = 0; room_idx < 3; room_idx++) {
        const HiveRoomDef *room = hive_trial_current_room(&hive);

        assert(room != NULL);
        assert(hive_trial_submit_answer(&hive, room->correct_option));
        assert(hive.memory_ready);
        hive_trial_collect_memory(&hive);
    }

    assert(hive.memories_found == 3);
    assert(hive.finished);
    assert(hive_trial_current_room(&hive) == NULL);
}

int main(void) {
    test_second_phase_enables_kamikaze_pressure();
    test_correct_answer_unlocks_memory_pickup();
    test_third_memory_finishes_the_hive_phase();
    puts("campaign_test: ok");
    return 0;
}
