#include "game_state.h"
#include "collision.h"
#include "ui.h"
#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define HIVE_WORLD_OPTION_Y 118.0f
#define HIVE_WORLD_PANEL_Y  84.0f
#define ARCHIVE_PAPER_Y     (SCREEN_H - 116.0f)

static void set_status(Game *g, const char *text);
static void hive_enter_room(Game *g);
static float next_archive_honey_delay(void);
static void archive_spawn_paper(Game *g);

static bool is_hive_phase(const Game *g) {
    return g->current_stage == 3;
}

static bool is_archive_phase(const Game *g) {
    return g->current_stage == 4;
}

static float hive_option_x(int idx) {
    return 140.0f + idx * 180.0f;
}

static Rectangle hive_option_rect(int idx) {
    return (Rectangle){hive_option_x(idx) - 72.0f, HIVE_WORLD_OPTION_Y, 144.0f, 82.0f};
}

static Rectangle archive_paper_rect(const Game *g) {
    return (Rectangle){g->archive_paper_x - 22.0f, g->archive_paper_y - 28.0f, 44.0f, 56.0f};
}

static Rectangle archive_honey_rect(const Game *g, int hazard_index) {
    if (hazard_index < 0 || hazard_index >= g->archive_honey_zone_count) {
        return (Rectangle){0.0f, 0.0f, 0.0f, 0.0f};
    }
    return archive_hazard_zone_rect(g->archive_honey_zones[hazard_index]);
}

static void draw_scene_cover(Texture2D bg, float scroll, float zoom, Color tint) {
    float screen_ratio = (float)SCREEN_W / (float)SCREEN_H;
    float tex_ratio = (float)bg.width / (float)bg.height;
    Rectangle src = {0.0f, 0.0f, (float)bg.width, (float)bg.height};

    if (tex_ratio > screen_ratio) {
        src.width = bg.height * screen_ratio;
        src.x = (bg.width - src.width) * 0.5f;
    } else {
        src.height = bg.width / screen_ratio;
        src.y = (bg.height - src.height) * 0.5f;
    }

    if (bg.height > src.height) {
        float drift = fmodf(scroll * 0.45f, bg.height - src.height);
        src.y = drift;
    }

    DrawTexturePro(bg, src,
                   (Rectangle){0.0f, 0.0f, SCREEN_W, SCREEN_H},
                   (Vector2){0.0f, 0.0f},
                   0.0f,
                   tint);
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){8, 10, 20, (unsigned char)(100.0f * zoom)});
}

static void draw_hex_layer(const Assets *a, float scroll, float speed, float scale, Color tint) {
    float size = SPR_SIZE_16 * scale;
    float step_x = size + 20.0f;
    float step_y = size * 0.78f;
    float offset = fmodf(scroll * speed, step_y * 2.0f);

    for (int row = -1; row < SCREEN_H / (int)step_y + 3; row++) {
        float y = row * step_y - offset;
        float x_offset = (row % 2 == 0) ? 0.0f : step_x * 0.5f;
        for (int col = -1; col < SCREEN_W / (int)step_x + 3; col++) {
            DrawTextureEx(a->hex_tile, (Vector2){col * step_x + x_offset, y}, 0.0f, scale, tint);
        }
    }
}

static void draw_background(const Game *g) {
    const CampaignPhase *phase = campaign_phase_get(g->current_stage);
    if (!phase) {
        ClearBackground((Color){10, 10, 20, 255});
        return;
    }

    ClearBackground((Color){8, 8, 18, 255});
    draw_scene_cover(g->assets.stages[phase->background_scene], g->bg_scroll, 1.0f,
                     (Color){255, 255, 255, 125});

    switch (phase->style) {
        case PHASE_STYLE_FORMATION:
            draw_hex_layer(&g->assets, g->bg_scroll, 0.6f, 3.2f, (Color){255, 218, 140, 30});
            DrawRectangleGradientV(0, 0, SCREEN_W, SCREEN_H,
                                   (Color){18, 26, 46, 0},
                                   (Color){12, 10, 24, 150});
            break;
        case PHASE_STYLE_ASSAULT:
            draw_hex_layer(&g->assets, g->bg_scroll, 1.1f, 3.4f, (Color){255, 138, 82, 44});
            DrawRectangleGradientV(0, 0, SCREEN_W, SCREEN_H,
                                   (Color){72, 20, 10, 30},
                                   (Color){18, 8, 14, 180});
            break;
        case PHASE_STYLE_HIVE:
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){58, 36, 12, 120});
            draw_hex_layer(&g->assets, g->bg_scroll, 0.8f, 3.8f, (Color){255, 195, 92, 34});
            DrawRectangleGradientV(0, 0, SCREEN_W, SCREEN_H,
                                   (Color){255, 210, 110, 12},
                                   (Color){24, 16, 8, 175});
            break;
        case PHASE_STYLE_ARCHIVE:
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){44, 28, 10, 110});
            draw_hex_layer(&g->assets, g->bg_scroll, 0.55f, 3.6f, (Color){255, 184, 96, 24});
            DrawRectangleGradientV(0, 0, SCREEN_W, SCREEN_H,
                                   (Color){255, 232, 182, 10},
                                   (Color){30, 16, 8, 190});
            break;
        case PHASE_STYLE_BOSS:
            draw_hex_layer(&g->assets, g->bg_scroll, 0.95f, 3.1f, (Color){255, 90, 90, 28});
            DrawRectangleGradientV(0, 0, SCREEN_W, SCREEN_H,
                                   (Color){50, 8, 10, 15},
                                   (Color){10, 2, 6, 200});
            break;
    }
}

static void begin_stage(Game *g, int n) {
    int saved[POWER_COUNT];
    for (int i = 0; i < POWER_COUNT; i++) saved[i] = g->player.powers[i];

    g->current_stage = n;
    player_init(&g->player);
    for (int i = 0; i < POWER_COUNT; i++) g->player.powers[i] = saved[i];
    bullets_init(&g->bullets);
    stage_load(&g->grid, n);
    pickup_init(&g->pickup);
    g->pickup_quiz.active = false;
    g->pickup_quiz.finished = false;
    g->memory_quiz.active = false;
    g->memory_quiz.finished = false;
    g->paper_quiz.active = false;
    g->paper_quiz.finished = false;
    g->bg_scroll = 0.0f;
    g->status_text[0] = '\0';
    g->status_timer = 0.0f;
    g->hive_hazard_timer = 0.9f;
    g->archive_honey_timer = next_archive_honey_delay();
    g->archive_honey_warning = 0.0f;
    g->archive_honey_active = 0.0f;
    g->archive_honey_zone_count = 0;
    g->archive_paper_active = false;
    g->archive_paper_x = SCREEN_W * 0.5f;
    g->archive_paper_y = ARCHIVE_PAPER_Y;
    g->reveal_room_index = -1;
    hive_trial_init(&g->hive_trial);
    paper_trial_init(&g->paper_trial);
    g->state = STATE_PLAYING;

    if (is_hive_phase(g)) {
        hive_enter_room(g);
    } else if (is_archive_phase(g)) {
        archive_spawn_paper(g);
        set_status(g, "Colete 5 papeis e responda cada pergunta.");
    }
}

static void draw_hive_world_room(const Game *g) {
    const HiveRoomDef *room = hive_trial_current_room(&g->hive_trial);
    if (!room || room->mode != HIVE_ROOM_WORLD) return;

    DrawRectangleRounded((Rectangle){46.0f, 30.0f, SCREEN_W - 92.0f, 64.0f},
                         0.18f, 12, (Color){22, 15, 8, 190});
    DrawText(room->title,
             SCREEN_W / 2 - MeasureText(room->title, 22) / 2,
             40, 22, (Color){255, 220, 110, 255});
    DrawText(room->prompt,
             SCREEN_W / 2 - MeasureText(room->prompt, 16) / 2,
             66, 16, WHITE);

    for (int i = 0; i < HIVE_OPTION_COUNT; i++) {
        Rectangle rect = hive_option_rect(i);
        DrawRectangleRounded(rect, 0.24f, 10, (Color){92, 56, 20, 220});
        DrawRectangleRoundedLinesEx(rect, 0.24f, 10, 2.0f, (Color){255, 212, 128, 255});
        DrawTextureEx(g->assets.memory_shard,
                      (Vector2){rect.x + rect.width / 2.0f - 18.0f, rect.y + 10.0f},
                      0.0f, 2.2f, (Color){255, 255, 255, 200});
        DrawText(room->options[i],
                 (int)(rect.x + rect.width / 2.0f - MeasureText(room->options[i], 16) / 2),
                 (int)(rect.y + 50.0f), 16, WHITE);
    }

    DrawText("Atire na resposta correta para expor a memoria.",
             SCREEN_W / 2 - MeasureText("Atire na resposta correta para expor a memoria.", 16) / 2,
             SCREEN_H - 82, 16, (Color){255, 240, 212, 255});
}

static void draw_memory_progress(const Game *g) {
    char buf[32];
    snprintf(buf, sizeof(buf), "MEMORIAS %d/3", g->hive_trial.memories_found);
    DrawText(buf, SCREEN_W - MeasureText(buf, 16) - 8, 28, 16, (Color){255, 224, 140, 255});
}

static void draw_archive_room(const Game *g) {
    DrawRectangleRounded((Rectangle){44.0f, 28.0f, SCREEN_W - 88.0f, 60.0f},
                         0.18f, 12, (Color){26, 18, 8, 188});
    DrawText("ARQUIVO DO MEL",
             SCREEN_W / 2 - MeasureText("ARQUIVO DO MEL", 22) / 2,
             38, 22, (Color){255, 224, 136, 255});
    DrawText("Desvie dos blocos de mel e pegue os papeis com perguntas.",
             SCREEN_W / 2 - MeasureText("Desvie dos blocos de mel e pegue os papeis com perguntas.", 16) / 2,
             64, 16, WHITE);

    if (g->archive_honey_warning > 0.0f || g->archive_honey_active > 0.0f) {
        Color warn = (Color){255, 150, 80, 110};
        Color active = (Color){255, 198, 72, 180};
        for (int i = 0; i < g->archive_honey_zone_count; i++) {
            Rectangle honey = archive_honey_rect(g, i);
            DrawRectangleRec(honey, g->archive_honey_active > 0.0f ? active : warn);
            DrawRectangleLinesEx(honey, 3.0f, (Color){255, 236, 180, 220});
        }
    }

    if (g->archive_paper_active) {
        Rectangle paper = archive_paper_rect(g);
        DrawRectangleRounded(paper, 0.12f, 8, (Color){248, 236, 210, 255});
        DrawRectangleRoundedLinesEx(paper, 0.12f, 8, 2.0f, (Color){120, 82, 24, 255});
        DrawText("?", (int)(paper.x + paper.width / 2.0f - MeasureText("?", 24) / 2),
                 (int)(paper.y + 14.0f), 24, (Color){82, 52, 12, 255});
    }

    char buf[32];
    snprintf(buf, sizeof(buf), "PAPEIS %d/5", g->paper_trial.papers_collected);
    DrawText(buf, SCREEN_W - MeasureText(buf, 16) - 8, 28, 16, (Color){255, 224, 140, 255});
}

static void draw_playfield(const Game *g) {
    draw_background(g);

    if (is_hive_phase(g)) draw_hive_world_room(g);
    if (is_archive_phase(g)) draw_archive_room(g);

    bullets_draw(&g->bullets, &g->assets);
    enemy_grid_draw(&g->grid, &g->assets);
    pickup_draw(&g->pickup, &g->assets);
    player_draw(&g->player, &g->assets);
    ui_draw_hud(&g->player, g->score, g->current_stage, &g->assets);

    if (is_hive_phase(g)) draw_memory_progress(g);

    if (g->status_timer > 0.0f && g->status_text[0]) {
        DrawText(g->status_text,
                 SCREEN_W / 2 - MeasureText(g->status_text, 18) / 2,
                 48, 18, (Color){255, 255, 255, 255});
    }
}

static TempBoostType random_temp_boost(void) {
    return (TempBoostType)GetRandomValue(0, TEMP_BOOST_COUNT - 1);
}

static float next_archive_honey_delay(void) {
    return (float)GetRandomValue(5, 10) * 0.1f;
}

static void archive_spawn_paper(Game *g) {
    g->archive_paper_active = true;
    g->archive_paper_x = (float)GetRandomValue(80, SCREEN_W - 80);
    g->archive_paper_y = (float)GetRandomValue(152, SCREEN_H - 84);
}

static void archive_pick_hazard_wave(Game *g) {
    int chosen = 0;
    int target = archive_hazard_wave_size() + GetRandomValue(0, 1);
    if (target > ARCHIVE_HAZARD_MAX_ACTIVE) target = ARCHIVE_HAZARD_MAX_ACTIVE;

    while (chosen < target) {
        int zone = GetRandomValue(0, archive_hazard_zone_count() - 1);
        bool already_used = false;

        for (int i = 0; i < chosen; i++) {
            if (g->archive_honey_zones[i] == zone) {
                already_used = true;
                break;
            }
        }

        if (already_used) continue;
        g->archive_honey_zones[chosen++] = zone;
    }

    g->archive_honey_zone_count = chosen;
}

static void set_status(Game *g, const char *text) {
    snprintf(g->status_text, sizeof(g->status_text), "%s", text);
    g->status_timer = 2.0f;
}

static void hive_start_overlay_quiz(Game *g, const HiveRoomDef *room) {
    quick_quiz_start_custom(
        &g->memory_quiz,
        room->title,
        "Responda para restaurar a proxima memoria da colmeia",
        room->prompt,
        room->options,
        room->correct_option,
        20.0f
    );
    g->state = STATE_MEMORY_QUIZ;
}

static void hive_begin_memory_reveal(Game *g) {
    g->reveal_room_index = g->hive_trial.room_index;
    g->memory_quiz.active = false;
    g->state = STATE_MEMORY_REVEAL;
}

static void archive_start_question(Game *g) {
    const PaperQuestionDef *question = paper_trial_current_question(&g->paper_trial);
    if (!question) return;

    quick_quiz_start_custom(
        &g->paper_quiz,
        question->title,
        "Responda para validar o papel encontrado",
        question->prompt,
        question->options,
        question->correct_option,
        18.0f
    );
    g->state = STATE_PAPER_QUIZ;
}

static void hive_enter_room(Game *g) {
    const HiveRoomDef *room = hive_trial_current_room(&g->hive_trial);
    bullets_init(&g->bullets);
    g->hive_hazard_timer = 0.9f;
    g->memory_quiz.active = false;
    g->memory_quiz.finished = false;

    if (!room) return;

    if (room->mode == HIVE_ROOM_OVERLAY) {
        hive_start_overlay_quiz(g, room);
    } else {
        g->state = STATE_PLAYING;
        set_status(g, room->title);
    }
}

static void hive_check_world_choices(Game *g) {
    const HiveRoomDef *room = hive_trial_current_room(&g->hive_trial);
    if (!room || room->mode != HIVE_ROOM_WORLD) return;

    for (int b = 0; b < MAX_PLAYER_BULLETS; b++) {
        if (!g->bullets.player[b].active) continue;

        Rectangle bullet_rect = {
            g->bullets.player[b].x - 6.0f,
            g->bullets.player[b].y - 12.0f,
            12.0f,
            24.0f
        };

        for (int i = 0; i < HIVE_OPTION_COUNT; i++) {
            if (!CheckCollisionRecs(bullet_rect, hive_option_rect(i))) continue;

            g->bullets.player[b].active = false;
            if (hive_trial_submit_answer(&g->hive_trial, i)) {
                set_status(g, "Memoria recuperada.");
                hive_begin_memory_reveal(g);
            } else {
                player_take_damage(&g->player);
                set_status(g, "Resposta instavel. A colmeia reagiu.");
            }
            return;
        }
    }
}

static void hive_spawn_hazard(Game *g, float dt) {
    g->hive_hazard_timer -= dt;
    if (g->hive_hazard_timer > 0.0f) return;

    int lane = GetRandomValue(0, HIVE_OPTION_COUNT - 1);
    bullet_spawn_enemy(&g->bullets, hive_option_x(lane), HIVE_WORLD_PANEL_Y);

    if (GetRandomValue(0, 100) > 58) {
        bullet_spawn_enemy(&g->bullets, hive_option_x((lane + 1) % HIVE_OPTION_COUNT), HIVE_WORLD_PANEL_Y + 18.0f);
    }

    g->hive_hazard_timer = 0.9f;
}

static void archive_update_honey(Game *g, float dt) {
    Rectangle player_rect = {
        g->player.x - (SPR_SIZE_16 * SPRITE_SCALE) / 2.0f,
        g->player.y - (SPR_SIZE_16 * SPRITE_SCALE) / 2.0f,
        SPR_SIZE_16 * SPRITE_SCALE,
        SPR_SIZE_16 * SPRITE_SCALE
    };

    if (g->archive_honey_warning > 0.0f) {
        g->archive_honey_warning -= dt;
        if (g->archive_honey_warning <= 0.0f) {
            g->archive_honey_warning = 0.0f;
            g->archive_honey_active = 0.95f;
        }
    } else if (g->archive_honey_active > 0.0f) {
        g->archive_honey_active -= dt;
        for (int i = 0; i < g->archive_honey_zone_count; i++) {
            if (CheckCollisionRecs(player_rect, archive_honey_rect(g, i))) {
                player_take_damage(&g->player);
                break;
            }
        }
        if (g->archive_honey_active <= 0.0f) {
            g->archive_honey_active = 0.0f;
            g->archive_honey_zone_count = 0;
            g->archive_honey_timer = next_archive_honey_delay();
        }
    } else {
        g->archive_honey_timer -= dt;
        if (g->archive_honey_timer <= 0.0f) {
            archive_pick_hazard_wave(g);
            g->archive_honey_warning = 0.45f;
        }
    }
}

static void archive_check_paper_collect(Game *g) {
    Rectangle player_rect = {
        g->player.x - (SPR_SIZE_16 * SPRITE_SCALE) / 2.0f,
        g->player.y - (SPR_SIZE_16 * SPRITE_SCALE) / 2.0f,
        SPR_SIZE_16 * SPRITE_SCALE,
        SPR_SIZE_16 * SPRITE_SCALE
    };

    if (!g->archive_paper_active) return;
    if (!CheckCollisionRecs(player_rect, archive_paper_rect(g))) return;

    g->archive_paper_active = false;
    archive_start_question(g);
}

static void frame_menu(Game *g) {
    if (ui_draw_menu()) {
        g->narrative_screen = 0;
        g->state = STATE_NARRATIVE;
    }
}

static void frame_narrative(Game *g) {
    if (ui_draw_narrative(&g->narrative_screen, &g->assets)) {
        g->score = 0;
        begin_stage(g, 1);
    }
}

static void frame_hive_playing(Game *g, float dt) {
    if (IsKeyPressed(KEY_ESCAPE)) g->state = STATE_PAUSED;

    if (g->state == STATE_PLAYING) {
        g->bg_scroll += 28.0f * dt;
        if (g->status_timer > 0.0f) g->status_timer -= dt;

        player_update(&g->player, dt);

        if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) &&
            g->player.shoot_timer <= 0.0f) {
            bullet_spawn_player(&g->bullets, g->player.x, g->player.y - 10.0f);
            g->player.shoot_timer = player_shoot_cooldown(&g->player);
        }

        bullets_update(&g->bullets, dt);
        hive_spawn_hazard(g, dt);
        g->score += collision_update(&g->player, &g->grid, &g->bullets);
        hive_check_world_choices(g);

        if (g->player.lives <= 0) {
            g->state = STATE_GAME_OVER;
        }
    }

    draw_playfield(g);

    if (g->state == STATE_MEMORY_REVEAL || g->state == STATE_GAME_OVER) return;

    if (g->state == STATE_PAUSED) {
        bool to_menu = ui_draw_pause();
        if (IsKeyPressed(KEY_ESCAPE)) g->state = STATE_PLAYING;
        if (to_menu) game_init(g);
    }
}

static void frame_archive_playing(Game *g, float dt) {
    if (IsKeyPressed(KEY_ESCAPE)) g->state = STATE_PAUSED;

    if (g->state == STATE_PLAYING) {
        g->bg_scroll += 18.0f * dt;
        if (g->status_timer > 0.0f) g->status_timer -= dt;

        player_update_free(&g->player, dt);
        archive_update_honey(g, dt);
        archive_check_paper_collect(g);

        if (g->player.lives <= 0) {
            g->state = STATE_GAME_OVER;
        }
    }

    draw_playfield(g);

    if (g->state == STATE_PAPER_QUIZ || g->state == STATE_GAME_OVER) return;

    if (g->state == STATE_PAUSED) {
        bool to_menu = ui_draw_pause();
        if (IsKeyPressed(KEY_ESCAPE)) g->state = STATE_PLAYING;
        if (to_menu) game_init(g);
    }
}

static void frame_playing(Game *g, float dt) {
    if (is_hive_phase(g)) {
        frame_hive_playing(g, dt);
        return;
    }

    if (is_archive_phase(g)) {
        frame_archive_playing(g, dt);
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) { g->state = STATE_PAUSED; }

    if (g->state == STATE_PLAYING) {
        g->bg_scroll += 40.0f * dt;
        if (g->status_timer > 0.0f) g->status_timer -= dt;
        player_update(&g->player, dt);

        if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) &&
             g->player.shoot_timer <= 0.0f) {
            if (g->player.temp_boost == TEMP_BOOST_DOUBLE_SHOT) {
                bullet_spawn_player(&g->bullets, g->player.x - 10.0f, g->player.y);
                bullet_spawn_player(&g->bullets, g->player.x + 10.0f, g->player.y);
            } else {
                bullet_spawn_player(&g->bullets, g->player.x, g->player.y);
            }
            g->player.shoot_timer = player_shoot_cooldown(&g->player);
        }

        bullets_update(&g->bullets, dt);
        enemy_grid_update(&g->grid, &g->bullets, &g->player, dt);
        g->score += collision_update(&g->player, &g->grid, &g->bullets);

        if (g->player.lives <= 0) {
            g->state = STATE_GAME_OVER;
        } else {
            pickup_update(&g->pickup, dt);

            if (pickup_try_collect(&g->pickup, g->player.x, g->player.y)) {
                quick_quiz_start(&g->pickup_quiz);
                g->state = STATE_PICKUP_QUIZ;
            }
        }
    }

    draw_playfield(g);

    if (g->state == STATE_PICKUP_QUIZ || g->state == STATE_GAME_OVER) return;

    if (g->state == STATE_PAUSED) {
        bool to_menu = ui_draw_pause();
        if (IsKeyPressed(KEY_ESCAPE)) g->state = STATE_PLAYING;
        if (to_menu) { game_init(g); return; }
        return;
    }

    if (g->player.lives <= 0) { g->state = STATE_GAME_OVER; return; }

    if (enemy_grid_alive_count(&g->grid) == 0) {
        bool exploding = false;
        for (int i = 0; i < g->grid.count; i++) {
            if (g->grid.enemies[i].explode_timer > 0.0f) {
                exploding = true;
                break;
            }
        }

        if (!exploding) {
            if (g->current_stage == campaign_phase_count()) {
                g->state = STATE_VICTORY;
            } else if (campaign_phase_has_power_choice(g->current_stage)) {
                quiz_init(&g->quiz, g->current_stage);
                g->state = STATE_QUIZ;
            } else {
                begin_stage(g, g->current_stage + 1);
            }
        }
    }
}

static void frame_quiz(Game *g) {
    quiz_update(&g->quiz);
    quiz_draw(&g->quiz);
    if (g->quiz.passed) g->state = STATE_POWER_CHOICE;
}

static void frame_pickup_quiz(Game *g, float dt) {
    draw_playfield(g);
    quick_quiz_update(&g->pickup_quiz, dt);
    quick_quiz_draw(&g->pickup_quiz);

    if (!g->pickup_quiz.finished) return;

    if (g->pickup_quiz.passed) {
        TempBoostType boost = random_temp_boost();
        player_apply_temp_boost(&g->player, boost, 10.0f);
        snprintf(g->status_text, sizeof(g->status_text), "BOOST: %s", player_temp_boost_name(boost));
        g->status_timer = 2.0f;
    } else if (g->pickup_quiz.timed_out) {
        set_status(g, "Tempo esgotado. Nenhum boost.");
    } else {
        set_status(g, "Resposta errada. Nenhum boost.");
    }

    g->state = STATE_PLAYING;
}

static void frame_memory_quiz(Game *g, float dt) {
    const HiveRoomDef *room = hive_trial_current_room(&g->hive_trial);

    draw_playfield(g);
    quick_quiz_update(&g->memory_quiz, dt);
    quick_quiz_draw(&g->memory_quiz);

    if (!g->memory_quiz.finished) return;

    if (g->memory_quiz.passed) {
        hive_trial_submit_answer(&g->hive_trial, room->correct_option);
        hive_begin_memory_reveal(g);
        return;
    }

    player_take_damage(&g->player);
    if (g->player.lives <= 0) {
        g->state = STATE_GAME_OVER;
        return;
    }

    set_status(g, g->memory_quiz.timed_out
                    ? "Tempo esgotado. A memoria recuou."
                    : "Resposta errada. A colmeia feriu voce.");
    hive_start_overlay_quiz(g, room);
}

static void frame_paper_quiz(Game *g, float dt) {
    const PaperQuestionDef *question = paper_trial_current_question(&g->paper_trial);

    draw_playfield(g);
    quick_quiz_update(&g->paper_quiz, dt);
    quick_quiz_draw(&g->paper_quiz);

    if (!g->paper_quiz.finished) return;

    if (question && g->paper_quiz.passed) {
        paper_trial_submit_answer(&g->paper_trial, question->correct_option);
        if (g->paper_trial.finished) {
            begin_stage(g, 5);
            return;
        }

        archive_spawn_paper(g);
        set_status(g, "Papel lido. Procure o proximo.");
        g->state = STATE_PLAYING;
        return;
    }

    player_take_damage(&g->player);
    if (g->player.lives <= 0) {
        g->state = STATE_GAME_OVER;
        return;
    }

    archive_spawn_paper(g);
    set_status(g, g->paper_quiz.timed_out
                    ? "Tempo esgotado. O papel escapou."
                    : "Resposta errada. Tente o papel de novo.");
    g->state = STATE_PLAYING;
}

static void frame_memory_reveal(Game *g) {
    const HiveRoomDef *room = hive_trial_room_at(g->reveal_room_index);
    bool last_memory = (g->hive_trial.memories_found + 1) >= 3;

    draw_background(g);
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){10, 8, 18, 170});
    DrawRectangleRounded((Rectangle){72.0f, 56.0f, SCREEN_W - 144.0f, SCREEN_H - 112.0f},
                         0.12f, 12, (Color){24, 16, 10, 235});

    DrawTextureEx(g->assets.memory_shard,
                  (Vector2){SCREEN_W / 2.0f - 48.0f, 84.0f},
                  0.0f, 6.0f, WHITE);

    DrawText("MEMORIA RECUPERADA",
             SCREEN_W / 2 - MeasureText("MEMORIA RECUPERADA", 28) / 2,
             156, 28, (Color){255, 222, 118, 255});
    DrawText(room ? room->title : "Fragmento",
             SCREEN_W / 2 - MeasureText(room ? room->title : "Fragmento", 20) / 2,
             194, 20, WHITE);
    DrawText(room ? room->lore : "",
             104, 238, 20, (Color){240, 232, 216, 255});

    DrawText(last_memory
                 ? "Pergunta final: quem lucra quando a colmeia esquece?"
                 : "Pergunta viva: o codigo serve a ordem ou a memoria?",
             96, 322, 18, (Color){255, 198, 120, 255});

    DrawText(last_memory
                 ? "[ ENTER ] confrontar a rainha"
                 : "[ ENTER ] avancar para a proxima memoria",
             SCREEN_W / 2 - MeasureText(last_memory
                                            ? "[ ENTER ] confrontar a rainha"
                                            : "[ ENTER ] avancar para a proxima memoria",
                                        18) / 2,
             392, 18, WHITE);

    if (!IsKeyPressed(KEY_ENTER) && !IsKeyPressed(KEY_SPACE)) return;

    if (hive_trial_collect_memory(&g->hive_trial)) {
        begin_stage(g, 4);
    } else {
        hive_enter_room(g);
    }
}

static void frame_power_choice(Game *g) {
    int choice = ui_draw_power_choice();
    if (choice >= 0 && choice < POWER_COUNT) {
        g->player.powers[choice]++;
        begin_stage(g, g->current_stage + 1);
    }
}

static void frame_game_over(Game *g) {
    if (ui_draw_game_over(g->score, g->current_stage)) {
        int saved[POWER_COUNT];
        for (int i = 0; i < POWER_COUNT; i++) saved[i] = g->player.powers[i];
        begin_stage(g, g->current_stage);
        for (int i = 0; i < POWER_COUNT; i++) g->player.powers[i] = saved[i];
    }
}

static void frame_victory(Game *g) {
    if (ui_draw_victory(g->score)) {
        game_init(g);
    }
}

void game_init(Game *g) {
    player_init(&g->player);
    bullets_init(&g->bullets);
    g->state            = STATE_MENU;
    g->current_stage    = 1;
    g->score            = 0;
    g->narrative_screen = 0;
    g->bg_scroll        = 0.0f;
    pickup_init(&g->pickup);
    g->pickup_quiz.active = false;
    g->pickup_quiz.finished = false;
    g->memory_quiz.active = false;
    g->memory_quiz.finished = false;
    g->paper_quiz.active = false;
    g->paper_quiz.finished = false;
    g->status_text[0] = '\0';
    g->status_timer = 0.0f;
    g->hive_hazard_timer = 0.9f;
    g->archive_honey_timer = next_archive_honey_delay();
    g->archive_honey_warning = 0.0f;
    g->archive_honey_active = 0.0f;
    g->archive_honey_zone_count = 0;
    g->archive_paper_active = false;
    g->archive_paper_x = SCREEN_W * 0.5f;
    g->archive_paper_y = ARCHIVE_PAPER_Y;
    g->reveal_room_index = -1;
    hive_trial_init(&g->hive_trial);
    paper_trial_init(&g->paper_trial);
}

void game_frame(Game *g, float dt) {
    BeginDrawing();

    switch (g->state) {
        case STATE_MENU:          frame_menu(g);             break;
        case STATE_NARRATIVE:     frame_narrative(g);        break;
        case STATE_PLAYING:
        case STATE_PAUSED:        frame_playing(g, dt);      break;
        case STATE_QUIZ:          frame_quiz(g);             break;
        case STATE_PICKUP_QUIZ:   frame_pickup_quiz(g, dt);  break;
        case STATE_MEMORY_QUIZ:   frame_memory_quiz(g, dt);  break;
        case STATE_MEMORY_REVEAL: frame_memory_reveal(g);    break;
        case STATE_PAPER_QUIZ:    frame_paper_quiz(g, dt);   break;
        case STATE_POWER_CHOICE:  frame_power_choice(g);     break;
        case STATE_GAME_OVER:     frame_game_over(g);        break;
        case STATE_VICTORY:       frame_victory(g);          break;
    }

    EndDrawing();
}
