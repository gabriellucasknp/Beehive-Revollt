#include "quiz.h"
#include "types.h"
#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_W   200
#define BLOCK_H    44
#define SLOT_X    (SCREEN_W / 2 - BLOCK_W / 2)
#define SLOT_Y_0  160
#define SLOT_PAD   50

#define COL_BG     (Color){10,  10,  20,  255}
#define COL_AMBER  (Color){230, 130,   0,  255}
#define COL_GREY   (Color){100, 100, 120,  255}
#define COL_WHITE  WHITE
#define COL_GREEN  (Color){ 60, 200,  80,  255}
#define COL_RED    (Color){220,  60,  60,  255}
#define COL_YELLOW (Color){255, 210,  50,  255}

/* ------------------------------------------------------------------ */
/* Quiz definitions — one per stage transition (stages 1→6)           */
/* ------------------------------------------------------------------ */

typedef struct {
    const char *prompt;
    const char *blocks[QUIZ_MAX_BLOCKS];
    int         order[QUIZ_MAX_BLOCKS];
    int         count;
} QuizDef;

static const QuizDef QUIZ_DEFS[7] = {
    {0}, /* unused index 0 */
    /* Stage 1 → 2: variáveis */
    {
        "Monte o código que declara\ne imprime uma variável:",
        {"int main()", "int x = 10;", "printf(\"%d\", x);", "return 0;", "{", "}"},
        {0, 4, 1, 2, 3, 5}, 6
    },
    /* Stage 2 → 3: if */
    {
        "Monte o if que verifica\nse x é maior que 5:",
        {"if (x > 5)", "printf(\"maior\");", "{", "}", "int x = 8;", ""},
        {4, 0, 2, 1, 3, -1}, 5
    },
    /* Stage 3 → 4: loops */
    {
        "Monte o loop que conta\nde 0 a 4:",
        {"for (int i=0; i<5; i++)", "printf(\"%d\", i);", "{", "}", "int i;", ""},
        {0, 2, 1, 3, -1, -1}, 4
    },
    /* Stage 4 → 5: arrays */
    {
        "Monte o acesso correto\nao primeiro elemento do array:",
        {"int arr[3] = {1,2,3};", "arr[0]", "printf(\"%d\",", ");", "arr[1]", ""},
        {0, 2, 1, 3, -1, -1}, 4
    },
    /* Stage 5 → 6: funções */
    {
        "Monte a função que\nretorna a soma de a e b:",
        {"int soma(int a, int b)", "return a + b;", "{", "}", "int x;", ""},
        {0, 2, 1, 3, -1, -1}, 4
    },
    /* Stage 6 → 7: ponteiros */
    {
        "Monte o código que usa\num ponteiro para x:",
        {"int x = 42;", "int *p = &x;", "printf(\"%d\", *p);", "return 0;", "{", "}"},
        {0, 1, 2, 3, -1, -1}, 4
    },
};

/* ------------------------------------------------------------------ */

static void shuffle_blocks(Quiz *q) {
    /* Fisher-Yates on current_order */
    for (int i = q->block_count - 1; i > 0; i--) {
        int j = GetRandomValue(0, i);
        int tmp = q->current_order[i];
        q->current_order[i] = q->current_order[j];
        q->current_order[j] = tmp;
    }
    /* lay blocks out in slots */
    for (int s = 0; s < q->block_count; s++) {
        int bi = q->current_order[s];
        q->blocks[bi].x = SLOT_X;
        q->blocks[bi].y = SLOT_Y_0 + s * SLOT_PAD;
    }
}

void quiz_init(Quiz *q, int stage_number) {
    const QuizDef *def = &QUIZ_DEFS[stage_number];

    q->stage      = stage_number;
    q->drag_idx   = -1;
    q->verified   = false;
    q->passed     = false;
    q->feedback[0] = '\0';

    strncpy(q->prompt, def->prompt, sizeof(q->prompt) - 1);

    q->block_count = def->count;
    for (int i = 0; i < def->count; i++) {
        strncpy(q->blocks[i].text, def->blocks[i], sizeof(q->blocks[i].text) - 1);
        q->blocks[i].dragging = false;
        q->correct_order[i]   = def->order[i];
        q->current_order[i]   = i;
    }

    shuffle_blocks(q);
}

void quiz_update(Quiz *q) {
    Vector2 mouse = GetMousePosition();

    /* start drag */
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && q->drag_idx < 0) {
        for (int i = 0; i < q->block_count; i++) {
            Rectangle r = {q->blocks[i].x, q->blocks[i].y, BLOCK_W, BLOCK_H};
            if (CheckCollisionPointRec(mouse, r)) {
                q->drag_idx = i;
                q->blocks[i].dragging = true;
                break;
            }
        }
    }

    /* continue drag */
    if (q->drag_idx >= 0) {
        q->blocks[q->drag_idx].x = mouse.x - BLOCK_W / 2;
        q->blocks[q->drag_idx].y = mouse.y - BLOCK_H / 2;

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            /* snap to nearest slot */
            int best_slot = 0;
            float best_dist = 1e9f;
            for (int s = 0; s < q->block_count; s++) {
                float sy = SLOT_Y_0 + s * SLOT_PAD;
                float dy = q->blocks[q->drag_idx].y - sy;
                if (dy < 0) dy = -dy;
                if (dy < best_dist) { best_dist = dy; best_slot = s; }
            }

            /* find who currently owns that slot and swap */
            int dropped_idx = q->drag_idx;
            for (int i = 0; i < q->block_count; i++) {
                if (i == dropped_idx) continue;
                if (q->current_order[/* slot of i */0] == i) { /* find slot of i */
                    /* locate slot index for i */
                    for (int s = 0; s < q->block_count; s++) {
                        if (q->current_order[s] == i) {
                            /* swap current_order[best_slot] and current_order[s] */
                            int owner_slot = s;
                            /* find dragged slot */
                            int drag_slot = 0;
                            for (int ss = 0; ss < q->block_count; ss++)
                                if (q->current_order[ss] == dropped_idx) { drag_slot = ss; break; }

                            if (owner_slot == best_slot) {
                                int tmp = q->current_order[drag_slot];
                                q->current_order[drag_slot] = q->current_order[best_slot];
                                q->current_order[best_slot] = tmp;
                            }
                            break;
                        }
                    }
                }
            }
            /* simpler: just find drag_slot and best_slot and swap */
            {
                int drag_slot = 0;
                for (int s = 0; s < q->block_count; s++)
                    if (q->current_order[s] == dropped_idx) { drag_slot = s; break; }
                int tmp = q->current_order[drag_slot];
                q->current_order[drag_slot] = q->current_order[best_slot];
                q->current_order[best_slot] = tmp;
            }

            /* re-lay out all blocks */
            for (int s = 0; s < q->block_count; s++) {
                int bi = q->current_order[s];
                q->blocks[bi].x = SLOT_X;
                q->blocks[bi].y = SLOT_Y_0 + s * SLOT_PAD;
                q->blocks[bi].dragging = false;
            }
            q->drag_idx = -1;
        }
    }

    /* verify button */
    Rectangle vbtn = {SCREEN_W - 180, SCREEN_H - 60, 160, 40};
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse, vbtn) && q->drag_idx < 0) {

        bool ok = true;
        for (int s = 0; s < q->block_count; s++) {
            if (q->correct_order[s] < 0) continue; /* wildcard */
            if (q->current_order[s] != q->correct_order[s]) { ok = false; break; }
        }

        q->verified = true;
        if (ok) {
            q->passed = true;
            strncpy(q->feedback, "Correto! Escolha seu poder.", sizeof(q->feedback) - 1);
        } else {
            strncpy(q->feedback, "Errado — tente novamente!", sizeof(q->feedback) - 1);
            shuffle_blocks(q);
            q->verified = false;
        }
    }
}

void quiz_draw(const Quiz *q) {
    ClearBackground(COL_BG);

    DrawText("DESAFIO DE CODIGO",
             SCREEN_W/2 - MeasureText("DESAFIO DE CODIGO", 24)/2,
             20, 24, COL_YELLOW);

    DrawText(q->prompt, 40, 60, 16, COL_WHITE);

    /* slot guides */
    for (int s = 0; s < q->block_count; s++) {
        Rectangle guide = {SLOT_X, SLOT_Y_0 + s * SLOT_PAD, BLOCK_W, BLOCK_H};
        DrawRectangleLinesEx(guide, 1, COL_GREY);
    }

    /* blocks */
    for (int i = 0; i < q->block_count; i++) {
        const QuizBlock *b = &q->blocks[i];
        Color bg = b->dragging ? COL_AMBER : COL_GREY;
        DrawRectangle((int)b->x, (int)b->y, BLOCK_W, BLOCK_H, bg);
        DrawRectangleLinesEx((Rectangle){b->x, b->y, BLOCK_W, BLOCK_H}, 2, COL_WHITE);
        DrawText(b->text, (int)b->x + 8, (int)b->y + 12, 16, COL_WHITE);
    }

    /* verify button */
    Rectangle vbtn = {SCREEN_W - 180, SCREEN_H - 60, 160, 40};
    bool hover = CheckCollisionPointRec(GetMousePosition(), vbtn);
    DrawRectangleRec(vbtn, hover ? COL_AMBER : COL_GREY);
    DrawRectangleLinesEx(vbtn, 2, COL_WHITE);
    DrawText("VERIFICAR",
             SCREEN_W - 180 + 80 - MeasureText("VERIFICAR", 16)/2,
             SCREEN_H - 60 + 12, 16, COL_WHITE);

    /* feedback */
    if (q->feedback[0]) {
        Color fc = q->passed ? COL_GREEN : COL_RED;
        DrawText(q->feedback, 40, SCREEN_H - 60, 16, fc);
    }
}
