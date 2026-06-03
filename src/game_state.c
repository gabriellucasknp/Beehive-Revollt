#include "game_state.h"
#include "collision.h"
#include "ui.h"
#include "raylib.h"
#include <math.h>

/* ------------------------------------------------------------------ */
/* Internal helpers                                                     */
/* ------------------------------------------------------------------ */

static void begin_stage(Game *g, int n) {
    g->current_stage = n;
    player_init(&g->player);
    bullets_init(&g->bullets);
    stage_load(&g->grid, n);
    g->bg_scroll = 0.0f;
    g->state     = STATE_PLAYING;
}

static void draw_background(const Game *g) {
    int s = g->current_stage;
    if (s < 1 || s > 7) { ClearBackground((Color){10, 10, 20, 255}); return; }
    Texture2D bg = g->assets.stages[s];
    float scale = (float)SCREEN_W / bg.width;
    float th    = bg.height * scale;
    float y     = fmodf(g->bg_scroll * scale, th);
    DrawTextureEx(bg, (Vector2){0,      y},      0.0f, scale, WHITE);
    DrawTextureEx(bg, (Vector2){0, y - th},      0.0f, scale, WHITE);
}

/* ------------------------------------------------------------------ */
/* Per-state frame functions (update + draw, called inside Begin/End)  */
/* ------------------------------------------------------------------ */

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

static void frame_playing(Game *g, float dt) {
    /* --- update --- */
    if (IsKeyPressed(KEY_ESCAPE)) { g->state = STATE_PAUSED; }

    if (g->state == STATE_PLAYING) {
        g->bg_scroll += 40.0f * dt;
        player_update(&g->player, dt);

        if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) &&
             g->player.shoot_timer <= 0.0f) {
            bullet_spawn_player(&g->bullets, g->player.x, g->player.y);
            g->player.shoot_timer = player_shoot_cooldown(&g->player);
        }

        bullets_update(&g->bullets, dt);
        enemy_grid_update(&g->grid, &g->bullets, dt);
        g->score += collision_update(&g->player, &g->grid, &g->bullets);
    }

    /* --- draw --- */
    draw_background(g);
    bullets_draw(&g->bullets, &g->assets);
    enemy_grid_draw(&g->grid, &g->assets);
    player_draw(&g->player, &g->assets);
    ui_draw_hud(&g->player, g->score, g->current_stage, &g->assets);

    if (g->state == STATE_PAUSED) {
        bool to_menu = ui_draw_pause();
        if (IsKeyPressed(KEY_ESCAPE)) g->state = STATE_PLAYING;
        if (to_menu) { game_init(g); return; }
        return;
    }

    /* --- state transitions --- */
    if (g->player.lives <= 0) { g->state = STATE_GAME_OVER; return; }

    if (enemy_grid_alive_count(&g->grid) == 0) {
        bool exploding = false;
        for (int i = 0; i < g->grid.count; i++)
            if (g->grid.enemies[i].explode_timer > 0.0f) { exploding = true; break; }
        if (!exploding) {
            if (g->current_stage == 7) {
                g->state = STATE_VICTORY;
            } else {
                quiz_init(&g->quiz, g->current_stage);
                g->state = STATE_QUIZ;
            }
        }
    }
}

static void frame_quiz(Game *g) {
    quiz_update(&g->quiz);
    quiz_draw(&g->quiz);
    if (g->quiz.passed) g->state = STATE_POWER_CHOICE;
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

/* ------------------------------------------------------------------ */
/* Public API                                                           */
/* ------------------------------------------------------------------ */

void game_init(Game *g) {
    g->state            = STATE_MENU;
    g->current_stage    = 1;
    g->score            = 0;
    g->narrative_screen = 0;
    g->bg_scroll        = 0.0f;
}

void game_frame(Game *g, float dt) {
    BeginDrawing();

    switch (g->state) {
        case STATE_MENU:         frame_menu(g);           break;
        case STATE_NARRATIVE:    frame_narrative(g);      break;
        case STATE_PLAYING:
        case STATE_PAUSED:       frame_playing(g, dt);    break;
        case STATE_QUIZ:         frame_quiz(g);           break;
        case STATE_POWER_CHOICE: frame_power_choice(g);   break;
        case STATE_GAME_OVER:    frame_game_over(g);      break;
        case STATE_VICTORY:      frame_victory(g);        break;
    }

    EndDrawing();
}
