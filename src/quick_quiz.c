#include "quick_quiz.h"
#include "types.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>

#define QUIZ_TIME_LIMIT 15.0f
#define FONT_SIZE_PROMPT 22
#define FONT_SIZE_OPTION 18

typedef struct {
    const char *prompt;
    const char *options[QUICK_QUIZ_OPTION_COUNT];
    int correct_option;
} QuickQuizDef;

static const QuickQuizDef QUICK_QUIZ_DEFS[] = {
    {
        "Qual estrutura repete um bloco\num numero conhecido de vezes?",
        {"for", "if", "switch"},
        0
    },
    {
        "Qual operador compara igualdade\nem C?",
        {"=", "==", "!="},
        1
    },
    {
        "O que um ponteiro guarda?",
        {"Um endereco de memoria", "Um texto fixo", "Uma funcao pronta"},
        0
    },
    {
        "Qual destas estruturas armazena\nvarios valores do mesmo tipo?",
        {"array", "return", "break"},
        0
    },
    {
        "Qual palavra encerra um loop\nantes do fim natural?",
        {"continue", "break", "case"},
        1
    },
    {
        "Uma funcao com tipo int deve,\nem geral, retornar...",
        {"um numero inteiro", "um texto", "um ponteiro nulo sempre"},
        0
    },
    {
        "Qual estrutura toma decisoes\nentre caminhos diferentes?",
        {"if/else", "for", "typedef"},
        0
    },
    {
        "Qual palavra declara uma constante\nem muitas linguagens C-like?",
        {"const", "loop", "void*"},
        0
    },
};

static Rectangle option_rect(int idx) {
    return (Rectangle){70.0f, 180.0f + idx * 78.0f, SCREEN_W - 140.0f, 56.0f};
}

void quick_quiz_start_custom(QuickQuiz *q,
                             const char *title,
                             const char *subtitle,
                             const char *prompt,
                             const char *const options[QUICK_QUIZ_OPTION_COUNT],
                             int correct_option,
                             float time_limit) {
    memset(q, 0, sizeof(*q));
    strncpy(q->title, title, sizeof(q->title) - 1);
    strncpy(q->subtitle, subtitle, sizeof(q->subtitle) - 1);
    strncpy(q->prompt, prompt, sizeof(q->prompt) - 1);
    for (int i = 0; i < QUICK_QUIZ_OPTION_COUNT; i++)
        strncpy(q->options[i], options[i], sizeof(q->options[i]) - 1);
    q->correct_option = correct_option;
    q->selected_option = -1;
    q->time_limit = time_limit > 0.0f ? time_limit : QUIZ_TIME_LIMIT;
    q->time_left = q->time_limit;
    q->active = true;
}

void quick_quiz_start(QuickQuiz *q) {
    int idx = GetRandomValue(0, (int)(sizeof(QUICK_QUIZ_DEFS) / sizeof(QUICK_QUIZ_DEFS[0])) - 1);
    const QuickQuizDef *def = &QUICK_QUIZ_DEFS[idx];

    quick_quiz_start_custom(
        q,
        "QUIZ RELAMPAGO",
        "Responda em ate 15s para ganhar um boost",
        def->prompt,
        def->options,
        def->correct_option,
        QUIZ_TIME_LIMIT
    );
}

void quick_quiz_update(QuickQuiz *q, float dt) {
    if (!q->active || q->finished) return;

    q->time_left -= dt;
    if (q->time_left <= 0.0f) {
        q->time_left = 0.0f;
        q->finished = true;
        q->timed_out = true;
        q->passed = false;
        return;
    }

    if (IsKeyPressed(KEY_ONE)) q->selected_option = 0;
    if (IsKeyPressed(KEY_TWO)) q->selected_option = 1;
    if (IsKeyPressed(KEY_THREE)) q->selected_option = 2;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        for (int i = 0; i < QUICK_QUIZ_OPTION_COUNT; i++) {
            if (CheckCollisionPointRec(mouse, option_rect(i))) {
                q->selected_option = i;
                break;
            }
        }
    }

    if (q->selected_option < 0) return;

    q->finished = true;
    q->passed = (q->selected_option == q->correct_option);
}

void quick_quiz_draw(const QuickQuiz *q) {
    if (!q->active) return;

    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){5, 5, 15, 235});
    DrawText(q->title,
             SCREEN_W / 2 - MeasureText(q->title, 28) / 2,
             36, 28, (Color){255, 220, 90, 255});
    DrawText(q->subtitle,
             SCREEN_W / 2 - MeasureText(q->subtitle, 16) / 2,
             74, 16, (Color){200, 200, 220, 255});

    DrawText(q->prompt, 70, 108, FONT_SIZE_PROMPT, WHITE);

    for (int i = 0; i < QUICK_QUIZ_OPTION_COUNT; i++) {
        Rectangle r = option_rect(i);
        bool hover = CheckCollisionPointRec(GetMousePosition(), r);
        Color bg = hover ? (Color){230, 130, 0, 255} : (Color){95, 95, 120, 255};
        DrawRectangleRec(r, bg);
        DrawRectangleLinesEx(r, 2, WHITE);

        char label[112];
        snprintf(label, sizeof(label), "%d. %s", i + 1, q->options[i]);
        DrawText(label, (int)r.x + 14, (int)r.y + 18, FONT_SIZE_OPTION, WHITE);
    }

    DrawRectangle(70, SCREEN_H - 64, SCREEN_W - 140, 18, (Color){45, 45, 60, 255});
    DrawRectangle(70, SCREEN_H - 64,
                  (int)((SCREEN_W - 140) * (q->time_left / q->time_limit)), 18,
                  (Color){255, 210, 50, 255});
    DrawRectangleLines(70, SCREEN_H - 64, SCREEN_W - 140, 18, WHITE);

    char timer[32];
    snprintf(timer, sizeof(timer), "%.1fs", q->time_left);
    DrawText(timer, SCREEN_W / 2 - MeasureText(timer, 18) / 2, SCREEN_H - 90, 18, WHITE);
}
