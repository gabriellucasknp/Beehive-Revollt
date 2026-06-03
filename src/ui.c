#include "ui.h"
#include "raylib.h"
#include <stdio.h>

/* 8-bit palette */
#define COL_BG      (Color){10,  10,  20,  255}
#define COL_YELLOW  (Color){255, 210,  50,  255}
#define COL_AMBER   (Color){230, 130,   0,  255}
#define COL_WHITE   WHITE
#define COL_RED     (Color){220,  60,  60,  255}
#define COL_GREEN   (Color){ 60, 200,  80,  255}
#define COL_GREY    (Color){120, 120, 140,  255}

#define FONT_SIZE_BIG   32
#define FONT_SIZE_MED   20
#define FONT_SIZE_SM    16
#define CX              (SCREEN_W / 2)

static bool button(const char *label, int y, int font_size, Color col) {
    int w = MeasureText(label, font_size) + 24;
    Rectangle r = {CX - w/2, y, w, font_size + 12};
    bool hover = CheckCollisionPointRec(GetMousePosition(), r);
    DrawRectangleRec(r, hover ? COL_AMBER : COL_GREY);
    DrawRectangleLinesEx(r, 2, col);
    DrawText(label, CX - MeasureText(label, font_size)/2, y + 6, font_size, COL_WHITE);
    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

/* ---- MENU ---- */
bool ui_draw_menu(void) {
    ClearBackground(COL_BG);
    DrawText("BEEHIVE REVOLT",
             CX - MeasureText("BEEHIVE REVOLT", FONT_SIZE_BIG) / 2,
             100, FONT_SIZE_BIG, COL_YELLOW);
    DrawText("by lixei",
             CX - MeasureText("by lixei", FONT_SIZE_SM) / 2,
             142, FONT_SIZE_SM, COL_GREY);
    DrawText("A / D    mover    |    W    atirar    |    ESC    pausa",
             CX - MeasureText("A / D    mover    |    W    atirar    |    ESC    pausa", FONT_SIZE_SM) / 2,
             SCREEN_H - 40, FONT_SIZE_SM, COL_GREY);
    return button("[ JOGAR ]", 240, FONT_SIZE_BIG, COL_YELLOW);
}

/* ---- PAUSE ---- */
bool ui_draw_pause(void) {
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0, 0, 0, 140});
    DrawText("PAUSADO",
             CX - MeasureText("PAUSADO", FONT_SIZE_BIG) / 2,
             160, FONT_SIZE_BIG, COL_YELLOW);
    DrawText("ESC para continuar",
             CX - MeasureText("ESC para continuar", FONT_SIZE_SM) / 2,
             210, FONT_SIZE_SM, COL_GREY);
    return button("[ MENU PRINCIPAL ]", 280, FONT_SIZE_MED, COL_AMBER);
}

/* ---- GAME OVER ---- */
bool ui_draw_game_over(int score, int stage) {
    ClearBackground(COL_BG);
    DrawText("GAME OVER",
             CX - MeasureText("GAME OVER", FONT_SIZE_BIG) / 2,
             120, FONT_SIZE_BIG, COL_RED);

    char buf[64];
    snprintf(buf, sizeof(buf), "Pontos: %d    Fase: %d", score, stage);
    DrawText(buf, CX - MeasureText(buf, FONT_SIZE_MED) / 2,
             180, FONT_SIZE_MED, COL_WHITE);

    return button("[ TENTAR NOVAMENTE ]", 280, FONT_SIZE_MED, COL_AMBER);
}

/* ---- VICTORY ---- */
bool ui_draw_victory(int score) {
    ClearBackground(COL_BG);
    DrawText("VITORIA!",
             CX - MeasureText("VITORIA!", FONT_SIZE_BIG) / 2,
             80, FONT_SIZE_BIG, COL_YELLOW);

    const char *msg =
        "Voce e seus guerrilheiros derrubaram\n"
        "o governo tirânico da abelha rainha.\n"
        "A colmeia e livre!";
    DrawText(msg, 60, 150, FONT_SIZE_MED, COL_WHITE);

    char buf[32];
    snprintf(buf, sizeof(buf), "Pontos finais: %d", score);
    DrawText(buf, CX - MeasureText(buf, FONT_SIZE_MED) / 2,
             300, FONT_SIZE_MED, COL_GREEN);

    return button("[ MENU ]", 380, FONT_SIZE_MED, COL_AMBER);
}

/* ---- HUD ---- */
void ui_draw_hud(const Player *p, int score, int stage, const Assets *a) {
    /* hearts */
    float hw = SPR_SIZE_8 * SPRITE_SCALE;
    for (int i = 0; i < p->lives; i++)
        DrawTextureEx(a->heart, (Vector2){8.0f + i * (hw + 4.0f), 8.0f},
                      0.0f, SPRITE_SCALE, WHITE);

    /* score */
    char buf[32];
    snprintf(buf, sizeof(buf), "%07d", score);
    DrawText(buf, CX - MeasureText(buf, FONT_SIZE_MED) / 2, 8, FONT_SIZE_MED, COL_YELLOW);

    /* stage */
    snprintf(buf, sizeof(buf), "FASE %d", stage);
    DrawText(buf, SCREEN_W - MeasureText(buf, FONT_SIZE_SM) - 8, 8, FONT_SIZE_SM, COL_WHITE);

    /* powers */
    const char *pnames[] = {"TRP", "ESC", "VEL"};
    Color pcols[] = {COL_AMBER, COL_GREEN, COL_YELLOW};
    for (int i = 0; i < POWER_COUNT; i++) {
        if (p->powers[i] <= 0) continue;
        char pb[16];
        snprintf(pb, sizeof(pb), "%s x%d", pnames[i], p->powers[i]);
        DrawText(pb, 8, 40 + i * 18, FONT_SIZE_SM, pcols[i]);
    }
}

/* ---- POWER CHOICE ---- */
int ui_draw_power_choice(void) {
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0, 0, 10, 220});
    DrawText("ESCOLHA UM PODER",
             CX - MeasureText("ESCOLHA UM PODER", FONT_SIZE_BIG) / 2,
             80, FONT_SIZE_BIG, COL_YELLOW);

    const char *labels[] = {
        "[ TIRO RAPIDO ]",
        "[ ESCUDO ]",
        "[ MAIS VELOCIDADE ]"
    };
    for (int i = 0; i < POWER_COUNT; i++) {
        if (button(labels[i], 200 + i * 70, FONT_SIZE_MED, COL_AMBER))
            return i;
    }
    return -1;
}

/* ---- NARRATIVE ---- */
bool ui_draw_narrative(int *screen_idx, const Assets *a) {
    /* 3 cutscenes + 1 text-only intro */
    static const char *texts[] = {
        "Era uma vez uma colmeia oprimida.\n"
        "A abelha rainha governava com mão\n"
        "de ferro, controlando todo o mel\n"
        "produzido pelas operárias.",

        NULL, /* cutscene_1 image */
        NULL, /* cutscene_2 image */
        "A revolta começou. Pilote sua abelha\n"
        "e derrube o governo tirânico!\n\n"
        "Boa sorte, comandante."
    };
    int total = 4;

    ClearBackground(COL_BG);

    if (*screen_idx == 1 || *screen_idx == 2) {
        Texture2D cut = a->cutscene[*screen_idx];
        /* scale cutscene to fit screen */
        float scale = (float)SCREEN_W / cut.width;
        DrawTextureEx(cut, (Vector2){0, (SCREEN_H - cut.height * scale) / 2.0f},
                      0.0f, scale, WHITE);
    } else {
        const char *txt = texts[*screen_idx];
        DrawText(txt, 60, 120, FONT_SIZE_MED, COL_WHITE);
    }

    bool advance = button("[ CONTINUAR ]", SCREEN_H - 80, FONT_SIZE_MED, COL_AMBER);
    if (advance) {
        (*screen_idx)++;
        if (*screen_idx >= total) return true;
    }
    return false;
}
