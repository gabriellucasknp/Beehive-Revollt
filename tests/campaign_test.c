#include "campaign.h"
#include <assert.h>
#include <stdio.h>

static void test_second_phase_enables_kamikaze_pressure(void) {
    const CampaignPhase *phase_one = campaign_phase_get(1);
    const CampaignPhase *phase_two = campaign_phase_get(2);
    const CampaignPhase *phase_four = campaign_phase_get(4);

    assert(campaign_phase_count() == 5);
    assert(phase_one != NULL);
    assert(phase_two != NULL);
    assert(phase_four != NULL);
    assert(!phase_one->rush_enabled);
    assert(phase_two->rush_enabled);
    assert(phase_four->style == PHASE_STYLE_ARCHIVE);
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

static void test_fifth_paper_finishes_archive_phase(void) {
    PaperTrialState archive;
    paper_trial_init(&archive);

    for (int i = 0; i < 5; i++) {
        const PaperQuestionDef *paper = paper_trial_current_question(&archive);
        assert(paper != NULL);
        assert(!paper_trial_submit_answer(&archive, (paper->correct_option + 1) % HIVE_OPTION_COUNT));
        assert(paper_trial_submit_answer(&archive, paper->correct_option));
        assert(archive.papers_collected == i + 1);
    }

    assert(archive.finished);
    assert(paper_trial_current_question(&archive) == NULL);
}

static void test_archive_hazards_cover_the_room_and_spawn_in_waves(void) {
    int interior_zones = 0;

    assert(archive_hazard_zone_count() >= 6);
    assert(archive_hazard_wave_size() >= 2);

    for (int i = 0; i < archive_hazard_zone_count(); i++) {
        Rectangle rect = archive_hazard_zone_rect(i);
        assert(rect.x >= 0.0f);
        assert(rect.y >= 0.0f);
        assert(rect.x + rect.width <= SCREEN_W);
        assert(rect.y + rect.height <= SCREEN_H);

        if (rect.x > 0.0f && rect.x + rect.width < SCREEN_W) interior_zones++;
    }

    assert(interior_zones >= 4);
}

int main(void) {
    test_second_phase_enables_kamikaze_pressure();
    test_correct_answer_unlocks_memory_pickup();
    test_third_memory_finishes_the_hive_phase();
    test_fifth_paper_finishes_archive_phase();
    test_archive_hazards_cover_the_room_and_spawn_in_waves();
    puts("campaign_test: ok");
    return 0;
}
